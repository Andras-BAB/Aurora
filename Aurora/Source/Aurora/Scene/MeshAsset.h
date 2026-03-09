#pragma once

#include "Aurora/Renderer/Buffer.h"

#include <memory>

namespace Aurora {
	
	class MeshAsset {
	public:
		MeshAsset() = default;
		MeshAsset(const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer);
		virtual ~MeshAsset() = default;

		void BindBuffers() const;
		
		void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
		
	public:
		// std::unique_ptr<MeshNode> m_RootNode;
		
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
	
}
