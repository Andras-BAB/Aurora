#pragma once

#include "Aurora/Renderer/IRenderPass.h"

namespace Aurora {
	class MSAAResolvePass : public IRenderPass {
	public:
		MSAAResolvePass(GraphResourceID msaaSource, GraphResourceID resolveDest, DXGI_FORMAT format)
			: m_Source(msaaSource), m_Dest(resolveDest), m_Format(format) {}

		void Setup(IRenderGraphBuilder& builder) override {
			builder.RequireState(m_Source, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
			builder.RequireState(m_Dest, D3D12_RESOURCE_STATE_RESOLVE_DEST);
		}

		void Execute(IRenderCommandList* cmdList, const IRenderGraphResources& registry, const SceneData& sceneData) override {
			auto dx12CmdList = static_cast<ID3D12GraphicsCommandList*>(cmdList->GetNative());

			ID3D12Resource* pSource = static_cast<ID3D12Resource*>(registry.GetPhysicalResource(m_Source));
			ID3D12Resource* pDest = static_cast<ID3D12Resource*>(registry.GetPhysicalResource(m_Dest));

			dx12CmdList->ResolveSubresource(pDest, 0, pSource, 0, m_Format);
		}

	private:
		GraphResourceID m_Source;
		GraphResourceID m_Dest;
		DXGI_FORMAT m_Format;
	};
}
