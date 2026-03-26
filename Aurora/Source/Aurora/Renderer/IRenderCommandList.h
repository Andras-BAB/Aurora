#pragma once
#include <cstdint>

#include "RenderGraph.h"

namespace Aurora {
	class Pipeline;
	class Buffer;
	class DescriptorSet;

	enum class IndexFormat { Uint16, Uint32 };
	enum class ShaderStage { Vertex, Pixel, Compute, All };
	enum class LoadOp { Load, Clear, DontCare };

	struct RenderAttachment {
		GraphResourceID ResourceId = INVALID_RESOURCE_ID;
		LoadOp LoadAction = LoadOp::Clear;
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	};

	struct RenderPassInfo {
		std::vector<RenderAttachment> ColorAttachments;
		RenderAttachment DepthAttachment;
		bool HasDepth = false;
	};

	struct ImageBarrier {
		GraphResourceID ResourceId = INVALID_RESOURCE_ID;
		uint32_t OldState;
		uint32_t NewState;
	};

	class IRenderCommandList {
	public:
		virtual ~IRenderCommandList() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void SetPipeline(std::shared_ptr<Pipeline> pipeline) = 0;
		virtual void SetViewport(float x, float y, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) = 0;
		virtual void SetScissor(int x, int y, int width, int height) = 0;

		virtual void SetVertexBuffer(uint32_t slot, std::shared_ptr<Buffer> buffer, uint64_t offset = 0, uint32_t stride = 0) = 0;
		virtual void SetIndexBuffer(std::shared_ptr<Buffer> buffer, uint64_t offset = 0, IndexFormat format = IndexFormat::Uint32) = 0;

		virtual void BindConstants(uint32_t slot, const void* data, size_t size) = 0;

		virtual void BindDescriptorSet(uint32_t slot, DescriptorSet* descriptorSet) = 0;

		virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
		virtual void DrawIndexedInstanced(
			uint32_t indexCount,
			uint32_t instanceCount,
			uint32_t startIndexLocation,
			int32_t baseVertexLocation,
			uint32_t startInstanceLocation
		) = 0;
		
		virtual void BeginRendering(const RenderPassInfo& renderPassInfo) = 0;
		virtual void EndRendering() = 0;

		virtual void PipelineImageBarrier(const ImageBarrier& barrier) = 0;

		virtual void* GetNative() const = 0;
	};
}
