#pragma once

#include "Aurora/Renderer/Buffer.h"

#include <memory>

namespace Aurora {
	
	class MeshAsset {
	public:
		MeshAsset(const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer);
		virtual ~MeshAsset() = default;
		
	private:
		// std::unique_ptr<MeshNode> m_RootNode;
		
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};
	
}
