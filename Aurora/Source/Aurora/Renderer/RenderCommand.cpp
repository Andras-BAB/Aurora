#include "aupch.h"

#include "Renderer/RenderCommand.h"

namespace Aurora {
	std::unique_ptr<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}