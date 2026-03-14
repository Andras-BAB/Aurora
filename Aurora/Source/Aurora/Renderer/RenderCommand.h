#pragma once

#include "IGraphicsContext.h"
#include "Aurora/Core/Base.h"

#include "Aurora/Renderer/RendererAPI.h"
#include "Aurora/Renderer/Buffer.h"
#include "Aurora/Scene/Entity.h"

namespace Aurora {
	class RenderCommand {
	public:
		static void Init() {
			s_RendererAPI->Init();
		}

		static void Shutdown() {
			s_RendererAPI->Shutdown();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_RendererAPI->SetScissors(x, y, width, height);
		}

		static void SetClearColor(const math::Vec4& color) {
			s_RendererAPI->SetClearColor(color);
		}

		static void Clear() {
			s_RendererAPI->Clear();
		}

		static void BeginScene(const SceneData& sceneData) {
			s_RendererAPI->BeginFrame(sceneData);
		}

		static void EndScene() {
			s_RendererAPI->EndFrame();
		}

		static MeshAllocation AllocateMesh(const MeshData& meshData) {
			return s_RendererAPI->CreateMesh(meshData);
		}

		static void DrawIndexed(const std::shared_ptr<d3dUtil::MeshGeometry>& meshGeo) {
			s_RendererAPI->DrawIndexed(meshGeo);
		}

		static void SubmitForDraw(Entity entity) {
			s_RendererAPI->SubmitEntity(entity);
		}

		static void SubmitProxy(const RenderProxyData& proxyData) {
			s_RendererAPI->SubmitProxy(proxyData);
		}

		static void SetLineWidth(float width) {
			s_RendererAPI->SetLineWidth(width);
		}

		static void SetContext(IGraphicsContext* context) {
			s_RendererAPI->SetContext(context);
		}
		template<std::derived_from<IGraphicsContext> T>
		static T* GetContextAs() {
			if (IGraphicsContext* p = s_RendererAPI->GetContext()) {
				return static_cast<T*>(p);
			}
			assert(false);
			return nullptr;
		}

	private:
		static std::unique_ptr<RendererAPI> s_RendererAPI;

	};
}
