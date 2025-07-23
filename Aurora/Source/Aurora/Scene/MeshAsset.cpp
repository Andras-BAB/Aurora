#include "aupch.h"

#include "MeshAsset.h"

namespace Aurora {
	MeshAsset::MeshAsset(const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer) :
		m_VertexBuffer(vertexBuffer), m_IndexBuffer(indexBuffer) {
		
	}

	void MeshAsset::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
		m_VertexBuffer = vertexBuffer;
	}

	void MeshAsset::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
		m_IndexBuffer = indexBuffer;
	}
}
