#include "aupch.h"

#include "SceneRenderer.h"

#include "RenderPasses/BaseColorPass.h"
#include "FrameAllocator.h"
#include "RenderPasses/ParticleEmitPass.h"
#include "RenderPasses/ParticleInitPass.h"
#include "RenderPasses/ParticleRenderPass.h"
#include "RenderPasses/ParticleUpdatePass.h"

#include "RenderPasses/MSAAResolvePass.h"

#include "RenderPasses/PostProcessPass.h"

#include "RenderCommand.h"
#include "Renderer3D.h"
#include "RendererAPI.h"
#include "RenderGraph.h"
#include "Aurora/Scene/Entity.h"

#include "Platform/DirectX/Renderer/DirectX12StructuredBuffer.h"

#include <tracy/Tracy.hpp>

namespace Aurora {
	TextureHandle SceneRenderer::Render(Scene* scene, float viewportWidth, float viewportHeight) {
		ZoneScoped;
		Entity cameraEntity = scene->GetPrimaryCameraEntity();
		if (!cameraEntity) return{ INVALID_RESOURCE_ID };
		PerspectiveCamera& camera = cameraEntity.GetComponent<CameraComponent>().Camera;

		auto dx12Context = RenderCommand::GetContextAs<DirectX12Context>();
		DirectX12CommandList cmdList(dx12Context);

		LightManager* lightManager = scene->GetLightManager();
		lightManager->GatherLights(scene);
		lightManager->UploadLightsToGPU(&cmdList);

		SceneData data;
		data.PointLightCount = lightManager->GetPointLightCount();
		data.PointLightBufferID = lightManager->GetCurrentPointLightBuffer()->GetSRV();

		auto lightEntities = scene->GetAllEntitiesWith<DirectionalLightComponent>();
		uint8_t dirLCount = 0;
		for (auto entity : lightEntities) {
			auto dirL = lightEntities.get<DirectionalLightComponent>(entity);
			data.DirectionalLights[dirLCount].Direction = dirL.Direction;
			data.DirectionalLights[dirLCount].Strength = dirL.Strength;
			dirLCount++;
		}
		data.DirectionalLightCount = dirLCount;

		RenderCommand::BeginScene();

		data.MainView = RenderCommand::CreateRenderView(camera.GetView(), camera.GetProj(), camera.GetPosition());
		RenderCommand::BuildPassConstants(data);

		auto view = scene->GetAllEntitiesWith<MeshComponent, WorldTransformComponent>();

		for (auto entity : view) {
			auto [mesh, transform] = view.get<MeshComponent, WorldTransformComponent>(entity);

			if (!mesh.Mesh) continue;

			bool isHierarchical = scene->GetRegistry().all_of<RelationshipComponent>(entity);
			const std::vector<SubmeshInstance>& instances = mesh.Mesh->GetMesh()->GetSubmeshInstances();

			for (const auto& instance : instances) {
				RenderProxyData proxyData;
				proxyData.ObjectID = static_cast<uint32_t>(entity);
				proxyData.Mesh = mesh.Mesh;
				proxyData.SubmeshIndex = instance.SubmeshIndex;

				math::Mat4 entityWorld = transform.Transform;

				if (isHierarchical) {
					proxyData.Transform = entityWorld;
				} else {
					math::Mat4 submeshLocal = instance.LocalTransform;
					proxyData.Transform = submeshLocal * entityWorld;
				}

				Renderer3D::SubmitProxy(data.MainView, RenderQueue::Opaque, proxyData);
			}
		}

		if (!m_GraphAllocator) {
			m_GraphAllocator = std::make_unique<DirectX12GraphAllocator>(dx12Context, RenderCommand::GetTextureManager());
		}
		if (!m_FrameAllocator) {
			m_FrameAllocator = std::make_unique<FrameAllocator>(2 * 1024 * 1024);
		}

		m_FrameAllocator->Reset();

		auto backbuffer = dx12Context->CurrentBackBuffer();
		//uint32_t currentWidth = static_cast<uint32_t>(backbuffer->GetDesc().Width);
		//uint32_t currentHeight = static_cast<uint32_t>(backbuffer->GetDesc().Height);

		uint32_t currentWidth = viewportWidth > 0 ? viewportWidth : static_cast<uint32_t>(backbuffer->GetDesc().Width);
		uint32_t currentHeight = viewportHeight > 0 ? viewportHeight : static_cast<uint32_t>(backbuffer->GetDesc().Height);

		static uint32_t s_LastWidth = currentWidth, s_LastHeight = currentHeight;
		if (currentWidth != s_LastWidth || currentHeight != s_LastHeight) {
			m_GraphAllocator->ClearPool();
			s_LastWidth = currentWidth;
			s_LastHeight = currentHeight;
		}

		RenderGraph graph(*m_FrameAllocator);

		GraphResourceID backbufferID = graph.ImportTexture("Backbuffer", dx12Context->CurrentBackBuffer(), dx12Context->CurrentBackBufferView().ptr, 0, D3D12_RESOURCE_STATE_PRESENT);
		
		//GraphResourceID pointLightBufID = graph.ImportBuffer(
		//	"PointLightBuffer",
		//	lightManager->GetPointLightBuffer()->GetRawResource(),
		//	lightManager->GetPointLightBuffer()->GetSRV(),
		//	lightManager->GetPointLightBuffer()->GetUAV(),
		//	lightManager->GetPointLightBuffer()->GetCurrentState()
		//);
		auto currentPointLightBuffer = lightManager->GetCurrentPointLightBuffer();
		GraphResourceID pointLightBufID = graph.ImportBuffer(
			"PointLightBuffer",
			currentPointLightBuffer->GetRawResource(),
			currentPointLightBuffer->GetSRV(),
			currentPointLightBuffer->GetUAV(),
			currentPointLightBuffer->GetCurrentState()
		);

		uint32_t sampleCount = 4;
		uint32_t sampleQuality = 0;

		GraphTextureDesc msaaColorDesc = { currentWidth, currentHeight, ImageFormat::RGBA16F, sampleCount, sampleQuality, "MSAA_Color" };
		GraphResourceID msaaColorID = graph.CreateTexture(msaaColorDesc);

		GraphTextureDesc msaaDepthDesc = { currentWidth, currentHeight, ImageFormat::Depth, sampleCount, sampleQuality, "MSAA_Depth" };
		GraphResourceID msaaDepthID = graph.CreateTexture(msaaDepthDesc);

		GraphTextureDesc resolvedColorDesc = { currentWidth, currentHeight, ImageFormat::RGBA16F, 1, 0, "Resolved_Color" };
		GraphResourceID resolvedColorID = graph.CreateTexture(resolvedColorDesc);

		GraphTextureDesc finalViewportDesc = { currentWidth, currentHeight, ImageFormat::RGBA8, 1, 0, "Viewport_Output" };
		GraphResourceID viewportOutputID = graph.CreateTexture(finalViewportDesc);

		auto& colorPass = graph.AddPass<BaseColorPass>("BaseColor", currentWidth, currentHeight, msaaColorID, msaaDepthID);

		//auto& particleComputePass = graph.AddPass<ParticleComputePass>("ParticleCompute", 100'000);
		//auto& particleRenderPass = graph.AddPass<ParticleRenderPass>(
		//	"ParticleRender",
		//	particleComputePass.ParticleBufferID,
		//	colorPass.ColorTargetID,
		//	colorPass.DepthTargetID,
		//	100'000
		//);

		auto emitterView = scene->GetAllEntitiesWith<ParticleEmitterComponent, WorldTransformComponent>();
		// TODO: reorder the pass creations to minimize state changes (all emit, all update and all render)
		for (auto entity : emitterView) {
			auto [emitter, transform] = emitterView.get<ParticleEmitterComponent, WorldTransformComponent>(entity);

			if (!emitter.IsActive) continue;

			if (!emitter.ParticleBuffer) {
				emitter.ParticleBuffer = StructuredBuffer::Create(emitter.MaxParticles, sizeof(GPUParticle));
				emitter.DeadListBuffer = StructuredBuffer::Create(emitter.MaxParticles, sizeof(uint32_t));
				emitter.CounterBuffer = StructuredBuffer::Create(1, sizeof(uint32_t));

				emitter.IsInitialized = false;
				emitter.LeftoverSpawn = 0.0f;
			}

			if (emitter.IsGradientDirty || !emitter.GradientTexture) {
				const int TEX_WIDTH = 256;
				std::vector<uint8_t> pixelData(TEX_WIDTH * 4);

				for (int x = 0; x < TEX_WIDTH; ++x) {
					float t = static_cast<float>(x) / static_cast<float>(TEX_WIDTH - 1);

					math::Vec4 color = emitter.Gradient.Evaluate(t);

					pixelData[(x * 4) + 0] = static_cast<uint8_t>(std::clamp(color.x * 255.0f, 0.0f, 255.0f));
					pixelData[(x * 4) + 1] = static_cast<uint8_t>(std::clamp(color.y * 255.0f, 0.0f, 255.0f));
					pixelData[(x * 4) + 2] = static_cast<uint8_t>(std::clamp(color.z * 255.0f, 0.0f, 255.0f));
					pixelData[(x * 4) + 3] = static_cast<uint8_t>(std::clamp(color.w * 255.0f, 0.0f, 255.0f));
				}

				if (!emitter.GradientTexture) {
					TextureSpecification spec{};
					spec.Width = TEX_WIDTH;
					spec.Height = 1;
					spec.Format = ImageFormat::RGBA8;
					spec.GenerateMips = false;
					emitter.GradientTexture = ITexture2D::Create(spec);
				}

				if (!emitter.Texture) {
					emitter.Texture = ITexture2D::Create("textures/particle.png");
				}

				emitter.GradientTexture->SetData(pixelData.data(), static_cast<uint32_t>(pixelData.size()));
				emitter.IsGradientDirty = false;
			}


			//GraphResourceID bufferID = graph.ImportBuffer(
			//	"EmitterBuffer",
			//	emitter.ParticleBuffer->GetRawResource(),
			//	emitter.ParticleBuffer->GetSRV(),
			//	emitter.ParticleBuffer->GetUAV(),
			//	emitter.ParticleBuffer->GetCurrentState()
			//);

			GraphResourceID particleBufID = graph.ImportBuffer("ParticleBuffer", emitter.ParticleBuffer->GetRawResource(), emitter.ParticleBuffer->GetSRV(), emitter.ParticleBuffer->GetUAV(), emitter.ParticleBuffer->GetCurrentState());
			GraphResourceID deadListBufID = graph.ImportBuffer("DeadListBuffer", emitter.DeadListBuffer->GetRawResource(), emitter.DeadListBuffer->GetSRV(), emitter.DeadListBuffer->GetUAV(), emitter.DeadListBuffer->GetCurrentState());
			GraphResourceID counterBufID = graph.ImportBuffer("CounterBuffer", emitter.CounterBuffer->GetRawResource(), emitter.CounterBuffer->GetSRV(), emitter.CounterBuffer->GetUAV(), emitter.CounterBuffer->GetCurrentState());

			float dt = 0.016f / 4.f; // TODO: from SceneData

			math::Vec3 worldPos = transform.Transform.GetTranslation();
			ParticleSystemParams params;
			params.DeltaTime = dt;
			params.EmitterPosition = worldPos;
			params.EmitterVelocity = emitter.Velocity;
			params.LifeTime = emitter.LifeTime;
			params.ColorBegin = emitter.ColorBegin;
			params.ColorEnd = emitter.ColorEnd;
			params.SizeBegin = emitter.SizeBegin;
			params.SizeEnd = emitter.SizeEnd;
			params.VelocityVariation = emitter.VelocityVariation;
			params.RandomSeed = rand();
			params.SpawnExtents = emitter.SpawnExtents;
			params.VelocityRadial = emitter.VelocityRadial;
			params.MaxParticles = emitter.MaxParticles;

			params.ParticleBufferIdx = emitter.ParticleBuffer->GetUAV().Index;
			params.DeadListBufferIdx = emitter.DeadListBuffer->GetUAV().Index;
			params.CounterBufferIdx = emitter.CounterBuffer->GetUAV().Index;

			if (!emitter.IsInitialized) {
				graph.AddPass<ParticleInitPass>("ParticleInit", deadListBufID, counterBufID, params);

				emitter.ParticleBuffer->SetCurrentState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				emitter.DeadListBuffer->SetCurrentState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				emitter.CounterBuffer->SetCurrentState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

				emitter.IsInitialized = true;
			}

			uint32_t particlesToSpawn = 0;

			if (emitter.EmissionRate > 0.0f) {
				float spawnFloat = (emitter.EmissionRate * dt) + emitter.LeftoverSpawn;
				particlesToSpawn = static_cast<uint32_t>(std::floor(spawnFloat));
				emitter.LeftoverSpawn = spawnFloat - static_cast<float>(particlesToSpawn);
			}

			if (particlesToSpawn > 0) {
				params.EmitCount = particlesToSpawn;
				graph.AddPass<ParticleEmitPass>("ParticleEmit", particleBufID, deadListBufID, counterBufID, params, particlesToSpawn);
			}

			graph.AddPass<ParticleUpdatePass>("ParticleUpdate", particleBufID, deadListBufID, counterBufID, params, emitter.MaxParticles);

			auto& renderPass = graph.AddPass<ParticleRenderPass>(
				"ParticleRender",
				particleBufID,
				msaaColorID, //colorPass.ColorTargetID,
				msaaDepthID, //colorPass.DepthTargetID,
				&emitter
			);

			// save the states for the next frame
			emitter.ParticleBuffer->SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			// dead list and counter list are not used by any other shader
			emitter.DeadListBuffer->SetCurrentState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			emitter.CounterBuffer->SetCurrentState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

			//math::Vec3 worldPos = transform.Transform.GetTranslation();

			//auto& computePass = graph.AddPass<ParticleComputePass>("ParticleCompute", bufferID, &emitter, worldPos);

			//auto& renderPass = graph.AddPass<ParticleRenderPass>(
			//	"ParticleRender",
			//	bufferID,
			//	colorPass.ColorTargetID,
			//	colorPass.DepthTargetID,
			//	&emitter
			//);

			//emitter.ParticleBuffer->SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}

		auto& resolvePass = graph.AddPass<MSAAResolvePass>("MSAAResolve", msaaColorID, resolvedColorID, DXGI_FORMAT_R16G16B16A16_FLOAT);
		//auto& postProcessPass = graph.AddPass<PostProcessPass>("PostProcess", colorPass.ColorTargetID, backbufferID, currentWidth, currentHeight);
		//auto& postProcessPass = graph.AddPass<PostProcessPass>("PostProcess", resolvedColorID, backbufferID, currentWidth, currentHeight);
		auto& postProcessPass = graph.AddPass<PostProcessPass>("PostProcess", resolvedColorID, viewportOutputID, currentWidth, currentHeight);

		m_GraphAllocator->BeginFrame();
		graph.Compile(m_GraphAllocator.get());

		RenderCommand::UpdateBuffers();

		graph.Execute(&cmdList, data);

		TextureHandle resultHandle = { INVALID_RESOURCE_ID };

		if (viewportWidth > 0 && viewportHeight > 0) {
			resultHandle = graph.GetOutputTextureHandle(viewportOutputID);

			auto nativeCmdList = static_cast<ID3D12GraphicsCommandList*>(cmdList.GetNative());
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = static_cast<ID3D12Resource*>(graph.GetPhysicalResource(viewportOutputID));
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			nativeCmdList->ResourceBarrier(1, &barrier);

			graph.SetCurrentState(viewportOutputID, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}

		graph.SaveStates(m_GraphAllocator.get());

		RenderCommand::EndScene();

		return resultHandle;
	}
}
