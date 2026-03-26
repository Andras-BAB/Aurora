#include "aupch.h"

#include "SceneRenderer.h"

#include "BaseColorPass.h"
#include "FrameAllocator.h"
#include "PostProcessPass.h"
#include "RenderCommand.h"
#include "Renderer3D.h"
#include "RendererAPI.h"
#include "RenderGraph.h"
#include "Aurora/Scene/Entity.h"

namespace Aurora {
	void SceneRenderer::Render(Scene* scene) {
		Entity cameraEntity = scene->GetPrimaryCameraEntity();
		if (!cameraEntity) return;
		PerspectiveCamera& camera = cameraEntity.GetComponent<CameraComponent>().Camera;

		SceneData data;

		RenderCommand::BeginScene();
		data.MainView = RenderCommand::CreateRenderView(math::Mat4(camera.GetView4x4f()), math::Mat4(camera.GetProj4x4f()), camera.GetPosition3f());

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
		GraphResourceID depthBufferID = graph.ImportTexture("DepthBuffer", nullptr, 0, dx12Context->DepthStencilView().ptr, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		auto& colorPass = graph.AddPass<BaseColorPass>("BaseColor", depthBufferID, currentWidth, currentHeight);
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
