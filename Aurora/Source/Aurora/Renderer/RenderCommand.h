#pragma once

#include "GraphicsContext.h"
#include "VertexArray.h"
#include "glm/vec4.hpp"

#include "Aurora/Renderer/RendererAPI.h"
#include "Aurora/Renderer/Buffer.h"

namespace Aurora {
	class RenderCommand {
	public:
		static void Init() {
			s_RendererAPI->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color) {
			s_RendererAPI->SetClearColor(color);
		}

		static void Clear() {
			s_RendererAPI->Clear();
		}

		static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) {
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		static void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) {
			s_RendererAPI->DrawLines(vertexArray, vertexCount);
		}

		static void SetLineWidth(float width) {
			s_RendererAPI->SetLineWidth(width);
		}

		static void SetContext(GraphicsContext* context) {
			s_RendererAPI->SetContext(context);
		}
		template<typename T>
		static T* GetContextAs() {
			if(GraphicsContext* p = s_RendererAPI->GetContext()) {
				if(std::is_base_of_v<GraphicsContext, T>) {
					return dynamic_cast<T*>(p);
				}
			}
			AU_CORE_INFO("Context is NULL");
			return nullptr;
		}

	private:
		static std::unique_ptr<RendererAPI> s_RendererAPI;

	};
}