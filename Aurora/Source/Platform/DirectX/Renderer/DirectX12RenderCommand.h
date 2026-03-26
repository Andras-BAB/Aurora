#pragma once
#include "DirectX12RendererAPI.h"
#include "Aurora/Renderer/RenderCommand.h"

namespace Aurora {
	
	class DirectX12RenderCommand {
	public:
		static DirectX12Context* GetContext() {
			static auto* cached = RenderCommand::GetContextAs<DirectX12Context>();
			return cached;
		}
	};
}
