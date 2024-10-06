#include "aupch.h"

#include "Framebuffer.h"

#include "Renderer.h"

namespace Aurora {
    
    std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            AU_CORE_ERROR("RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::DirectX12:
            AU_CORE_ERROR("RendererAPI::DirectX12 is currently not supported!");
            return nullptr;
        case RendererAPI::API::Vulkan:
            AU_CORE_ERROR("RendererAPI::Vulkan is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            AU_CORE_ERROR("RendererAPI::OpenGL is currently WIP!");
            return nullptr;
        case RendererAPI::API::DirectX11:
            AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
            return nullptr;
        }

        AU_CORE_ERROR("Unknown RendererAPI!");
        return nullptr;
    }
    
}

