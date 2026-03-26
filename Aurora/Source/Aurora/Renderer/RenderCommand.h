#pragma once

#include "IGraphicsContext.h"
#include "Aurora/Core/Base.h"

#include "Aurora/Renderer/RendererAPI.h"
#include "Aurora/Scene/Entity.h"
#include "Platform/DirectX/Renderer/DirectX12RendererAPI.h"

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

		static void BeginScene() {
			s_RendererAPI->BeginFrame();
		}

		static void EndScene() {
			s_RendererAPI->EndFrame();
		}

		static MeshAllocation AllocateMesh(const MeshData& meshData) {
			return s_RendererAPI->CreateMesh(meshData);
		}

		static void UpdateBuffers() {
			s_RendererAPI->UpdateConstantBuffers();
		}

		// TODO: make an interface for TextureManager
		static DirectX12TextureManager* GetTextureManager() {
			auto dx12API = dynamic_cast<DirectX12RendererAPI*>(s_RendererAPI.get());
			return dx12API->GetTextureManager();
		}

		static void SubmitProxy(const RenderView& view, RenderQueue queue, const RenderProxyData& proxyData) {
			s_RendererAPI->SubmitProxy(view, queue, proxyData);
		}

		static void DrawQueue(RenderQueue queue, const RenderView& view) {
			s_RendererAPI->DrawQueue(queue, view);
		}

		static RenderView CreateRenderView(const math::Mat4& view, const math::Mat4& proj, const math::Vec3& eyePos) {
			return s_RendererAPI->CreateRenderView(view, proj, eyePos);
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

		template<std::derived_from<RendererAPI> T>
		static T* GetRendererAPIAs() {
			if (RendererAPI* p = s_RendererAPI.get()) {
				return static_cast<T*>(p);
			}
			assert(false);
			return nullptr;
		}

	private:
		static std::unique_ptr<RendererAPI> s_RendererAPI;

	};
}
