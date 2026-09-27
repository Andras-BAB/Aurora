#include "aupch.h"

#include "Renderer.h"
#include "RenderCommand.h"
#include "Renderer3D.h"

#include "Platform/DirectX/Renderer/DirectX12RenderCommand.h"
#include "Platform/DirectX/Renderer/DirectX12Context.h"

namespace Aurora {
	void Renderer::Init() {
		RenderCommand::Init();
		Renderer3D::Init();
	}

	void Renderer::Shutdown() {
		RenderCommand::Shutdown();
	}

	void Renderer::WaitForGPUIdle() {
		RenderCommand::GetContextAs<DirectX12Context>()->FlushCommandQueue();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		DirectX12RenderCommand::GetContext()->OnWindowResize();
		RenderCommand::SetViewport(0, 0, width, height);
	}
}
