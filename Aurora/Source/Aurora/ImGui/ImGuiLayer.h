#pragma once
#include "Aurora/Core/Layer.h"

#include "Aurora/Events/ApplicationEvent.h"
#include "Aurora/Events/KeyEvent.h"
#include "Aurora/Events/MouseEvent.h"

#include "Platform/DirectX/Utils/MSUtils.h"
#include <d3d12.h>

namespace Aurora {
	class DirectX12Context;
	
	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();
		uint32_t GetActiveWidgetID() const;

	private:
		bool m_BlockEvents = true;

		// TODO: Remove
		// Caching to faster access
		DirectX12Context* m_Context = nullptr;
		MS::ComPtr<ID3D12DescriptorHeap> m_ImGuiSrvHeap;
		UINT m_SrvHeapSize = 1024;
		UINT m_SrvAllocatedCount = 0;
		std::vector<UINT> m_FreeSrvIndices;
	};
	
}
