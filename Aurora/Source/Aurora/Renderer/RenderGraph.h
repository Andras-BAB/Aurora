#pragma once

#include "ArenaVector.h"
#include "FrameAllocator.h"
#include "Texture.h"

#include "RenderGraphTypes.h"

namespace Aurora {
	struct SceneData;
	class IRenderCommandList;
	class IRenderPass;

	class IRenderGraphBuilder {
	public:
		virtual ~IRenderGraphBuilder() = default;

		virtual GraphResourceID CreateTexture(const GraphTextureDesc& desc) = 0;
		virtual GraphResourceID CreateBuffer(const GraphBufferDesc& desc) = 0;

		virtual GraphResourceID ReadTexture(GraphResourceID id) = 0;
		virtual GraphResourceID ReadBuffer(GraphResourceID id) = 0;

		virtual GraphResourceID WriteRenderTarget(GraphResourceID id) = 0;
		virtual GraphResourceID WriteDepthStencil(GraphResourceID id) = 0;

		virtual GraphResourceID WriteTextureCompute(GraphResourceID id) = 0;

		// when we want a texture that comes from outside the graph (e.g. swapchain backbuffer)
		// usually doesn't call by a pass, instead from the graph building scope
		virtual GraphResourceID ImportTexture(std::string_view name, void* physicalResource, uint64_t rtvHandle, uint64_t dsvHandle, uint32_t currentState) = 0;
	};

	class IRenderGraphResources {
	public:
		virtual ~IRenderGraphResources() = default;

		// called from Execute, returns the bindless texture index
		virtual TextureHandle GetTextureHandle(GraphResourceID id) const = 0;

		virtual void* GetPhysicalResource(GraphResourceID id) const = 0;
		virtual uint64_t GetRtvHandlePtr(GraphResourceID id) const = 0;
		virtual uint64_t GetDsvHandlePtr(GraphResourceID id) const = 0;
	};

	// represents virtual resource in graph
	struct ResourceNode {
		GraphResourceID ID = INVALID_RESOURCE_ID;
		std::string_view Name;

		bool IsTexture = true;
		GraphTextureDesc TextureDesc;
		GraphBufferDesc BufferDesc;

		uint32_t ProducerPassID = 0xFFFFFFFF; // which pass creates or writes first
		ArenaVector<uint32_t> ConsumerPassIDs;

		uint32_t FirstUsePassIndex = 0xFFFFFFFF;
		uint32_t LastUsePassIndex = 0xFFFFFFFF;

		bool IsImported = false;

		ResourceNode(FrameAllocator& allocator) : ConsumerPassIDs(allocator, 4) {}
	};

	struct PassNode {
		uint32_t ID = 0;
		std::string_view Name;
		IRenderPass* PassInstance = nullptr;

		ArenaVector<GraphResourceID> TextureReads;
		ArenaVector<GraphResourceID> BufferReads;
		ArenaVector<GraphResourceID> RenderTargetWrites;
		ArenaVector<GraphResourceID> DepthStencilWrites;
		ArenaVector<GraphResourceID> ComputeWrites;

		bool IsCulled = false;

		PassNode(FrameAllocator& allocator)
			: TextureReads(allocator, 4), BufferReads(allocator, 4),
			RenderTargetWrites(allocator, 2), DepthStencilWrites(allocator, 1),
			ComputeWrites(allocator, 2) {
		}
	};

	// connects virtual ID with API specific objects/pointers (e.g. Resource -> void* = ID3D12Resource*)
	class RenderGraphResourceRegistry : public IRenderGraphResources {
	public:
		struct PhysicalResourceData {
			void* Resource = nullptr;
			uint64_t RtvHandlePtr = 0;
			uint64_t DsvHandlePtr = 0;
			TextureHandle BindlessHandle = { INVALID_RESOURCE_ID };
			uint32_t CurrentState = 0;
		};

		TextureHandle GetTextureHandle(GraphResourceID id) const override {
			if (id < m_Data.size()) return m_Data[id].BindlessHandle;
			return { INVALID_RESOURCE_ID };
		}

		void* GetPhysicalResource(GraphResourceID id) const override { return m_Data[id].Resource; }
		uint64_t GetRtvHandlePtr(GraphResourceID id) const override { return m_Data[id].RtvHandlePtr; }
		uint64_t GetDsvHandlePtr(GraphResourceID id) const override { return m_Data[id].DsvHandlePtr; }

		void Resize(size_t count) { m_Data.resize(count); }
		PhysicalResourceData& Get(GraphResourceID id) { return m_Data[id]; }

	private:
		std::vector<PhysicalResourceData> m_Data;
	};

	class IRenderGraphAllocator {
	public:
		virtual ~IRenderGraphAllocator() = default;

		virtual void AcquireTexture(const GraphTextureDesc& desc, RenderGraphResourceRegistry::PhysicalResourceData& outData) = 0;
		virtual void ReleaseTexture(const GraphTextureDesc& desc, const RenderGraphResourceRegistry::PhysicalResourceData& data) = 0;

		virtual void AcquireBuffer(const GraphBufferDesc& desc, RenderGraphResourceRegistry::PhysicalResourceData& outData) = 0;
		virtual void ReleaseBuffer(const GraphBufferDesc& desc, const RenderGraphResourceRegistry::PhysicalResourceData& data) = 0;

		virtual void UpdateFinalState(void* physicalResource, uint32_t finalState) = 0;
	};

	class RenderGraph : public IRenderGraphBuilder {
	public:
		RenderGraph(FrameAllocator& frameAllocator)
			: m_FrameAllocator(frameAllocator),
			m_Passes(frameAllocator, 32),     // maximum 32 passes TODO: make it parameterized
			m_Resources(frameAllocator, 128)  // maximum 128 resources
		{
		}

		void SaveStates(IRenderGraphAllocator* allocator);

		template<typename T, typename... Args>
		T& AddPass(std::string_view name, Args&&... args) {
			T* pass = m_FrameAllocator.AllocateObject<T>(std::forward<Args>(args)...);

			PassNode node(m_FrameAllocator);
			node.ID = static_cast<uint32_t>(m_Passes.size());
			node.Name = name;
			node.PassInstance = pass;

			m_Passes.push_back(std::move(node));

			m_CurrentPassBuilding = node.ID;

			(*pass).Setup(*this);

			return *pass;
		}

		void Compile(IRenderGraphAllocator* allocator);

		void AllocatePhysicalResources(IRenderGraphAllocator* allocator);

		void Execute(IRenderCommandList* cmdList, const SceneData& sceneData);

		GraphResourceID CreateTexture(const GraphTextureDesc& desc) override;

		GraphResourceID ReadTexture(GraphResourceID id) override;
		GraphResourceID ReadBuffer(GraphResourceID id) override;

		GraphResourceID WriteRenderTarget(GraphResourceID id) override;
		GraphResourceID WriteDepthStencil(GraphResourceID id) override;

		GraphResourceID WriteTextureCompute(GraphResourceID id) override;

		GraphResourceID ImportTexture(std::string_view name, void* physicalResource, uint64_t rtvHandle, uint64_t dsvHandle, uint32_t currentState) override;

		void UpdateImportedResource(GraphResourceID id, void* physicalResource, uint64_t rtvHandle, uint64_t dsvHandle);

		std::string_view AllocateString(const std::string& str);

	private:
		void PerformCulling();
		void CalculateLifetimes();
		void InsertBarriersForPass(IRenderCommandList* cmdList, const PassNode& pass);

	public:
		GraphResourceID CreateBuffer(const GraphBufferDesc& desc) override;

	private:
		FrameAllocator& m_FrameAllocator;

		ArenaVector<PassNode> m_Passes;
		ArenaVector<ResourceNode> m_Resources;
		uint32_t m_CurrentPassBuilding = 0xFFFFFFFF;

		RenderGraphResourceRegistry m_Registry;
	};
}
