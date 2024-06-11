#pragma once

#include "Renderer/VertexArray.h"

namespace Aurora {
	class VulkanVertexArray : public VertexArray {
		//void Bind() const override;
		//void Unbind() const override;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override;
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override;
	};
}
