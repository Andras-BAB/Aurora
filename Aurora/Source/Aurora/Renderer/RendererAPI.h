#pragma once

#include "IGraphicsContext.h"
#include "RenderProxyData.h"
#include "Aurora/Math/Math.h"

#include "Platform/DirectX/Utils/d3dUtil.h"

namespace Aurora {
	class Entity;

	enum class RenderQueue {
		Opaque = 0,
		Transparent,
		ShadowCaster,
		Count
	};

	struct RenderView {
		uint32_t ViewID = 0;
		math::Mat4 ViewMatrix;
		math::Mat4 ProjectionMatrix;
		math::Vec3 EyePosition;
	};

	struct SceneData {
		RenderView MainView;
		// std::vector<RenderView> ShadowViews;
		// DirectionalLight MainLight;
		// float EnvironmentIntensity;
	};

	class RendererAPI {
	public:
		enum class API {
			None = 0,
			Vulkan = 1,
			DirectX12 = 2,
			DirectX11 = 3
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void ResizeSwapchainImage(uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const math::Vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<d3dUtil::MeshGeometry>& meshGeo) = 0;

		virtual void SubmitProxy(const RenderView& view, RenderQueue queue, const RenderProxyData& proxyData) = 0;

		virtual void DrawQueue(RenderQueue queue, const RenderView& view) = 0;

		virtual void SetLineWidth(float width) = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual MeshAllocation CreateMesh(const MeshData& meshData) = 0;
		virtual RenderView CreateRenderView(const math::Mat4& view, const math::Mat4& proj, const math::Vec3& eyePos) = 0;

		virtual void UpdateConstantBuffers() = 0;

		virtual void SetContext(IGraphicsContext* context) {  }
		virtual IGraphicsContext* GetContext() const { return nullptr; }

		static API GetAPI() { return s_API; }
		static std::unique_ptr<RendererAPI> Create();

	private:
		static API s_API;
	};
}
