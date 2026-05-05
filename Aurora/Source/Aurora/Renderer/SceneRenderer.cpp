#include "aupch.h"

#include "SceneRenderer.h"

#include <tracy/Tracy.hpp>

#include "BaseColorPass.h"
#include "FrameAllocator.h"
#include "ParticleEmitPass.h"
#include "ParticleInitPass.h"
#include "ParticleRenderPass.h"
#include "ParticleUpdatePass.h"
#include "PostProcessPass.h"
#include "RenderCommand.h"
#include "Renderer3D.h"
#include "RendererAPI.h"
#include "RenderGraph.h"
#include "Aurora/Scene/Entity.h"
#include "Platform/DirectX/Renderer/DirectX12StructuredBuffer.h"

namespace Aurora {
	void SceneRenderer::Render(Scene* scene) {
		ZoneScoped;
		Entity cameraEntity = scene->GetPrimaryCameraEntity();
		if (!cameraEntity) return;
		PerspectiveCamera& camera = cameraEntity.GetComponent<CameraComponent>().Camera;

		SceneData data;

		RenderCommand::BeginScene();
		data.MainView = RenderCommand::CreateRenderView(math::Mat4(camera.GetView()), math::Mat4(camera.GetProj()), camera.GetPosition());

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

		auto dx12Context = RenderCommand::GetContextAs<DirectX12Context>();
		if (!m_GraphAllocator) {
			m_GraphAllocator = std::make_unique<DirectX12GraphAllocator>(dx12Context, RenderCommand::GetTextureManager());
		}
		if (!m_FrameAllocator) {
			m_FrameAllocator = std::make_unique<FrameAllocator>(2 * 1024 * 1024);
		}

		m_FrameAllocator->Reset();

		auto backbuffer = dx12Context->CurrentBackBuffer();
		uint32_t currentWidth = static_cast<uint32_t>(backbuffer->GetDesc().Width);
		uint32_t currentHeight = static_cast<uint32_t>(backbuffer->GetDesc().Height);

		static uint32_t s_LastWidth = currentWidth, s_LastHeight = currentHeight;
		if (currentWidth != s_LastWidth || currentHeight != s_LastHeight) {
			m_GraphAllocator->ClearPool();
			s_LastWidth = currentWidth;
			s_LastHeight = currentHeight;
		}

		RenderGraph graph(*m_FrameAllocator);

		GraphResourceID backbufferID = graph.ImportTexture("Backbuffer", dx12Context->CurrentBackBuffer(), dx12Context->CurrentBackBufferView().ptr, 0, D3D12_RESOURCE_STATE_PRESENT);
		
		auto& colorPass = graph.AddPass<BaseColorPass>("BaseColor", currentWidth, currentHeight);

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
				colorPass.ColorTargetID,
				colorPass.DepthTargetID,
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

		auto& postProcessPass = graph.AddPass<PostProcessPass>("PostProcess", colorPass.ColorTargetID, backbufferID);

		m_GraphAllocator->BeginFrame();
		graph.Compile(m_GraphAllocator.get());

		RenderCommand::UpdateBuffers();

		DirectX12CommandList graphCmdList(dx12Context);
		graph.Execute(&graphCmdList, data);

		graph.SaveStates(m_GraphAllocator.get());

		RenderCommand::EndScene();
	}
}
