#include "aupch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Aurora/Core/Application.h"
#include "Aurora/Core/Log.h"
#include "Aurora/Utils/Hash.h"

#include "Platform/DirectX/Renderer/DirectX12Texture.h"

namespace Aurora {

	std::shared_ptr<ITexture2D> ITexture2D::Create(const TextureSpecification& specification) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			AU_CORE_ERROR("RendererAPI::Vulkan is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			return std::make_shared<DirectX12Texture2D>(specification);
		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		}

		AU_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<ITexture2D> ITexture2D::Create(const std::string& path) {
		uint64_t stableHash = Utils::HashString(path);
		Aurora::UUID uuid(stableHash);

		auto existingTexture = Application::Get().GetAssetRegistry().GetTexture(uuid);
		if (existingTexture) {
			return existingTexture;
		}

		std::shared_ptr<ITexture2D> texture = nullptr;
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			AU_CORE_ERROR("RendererAPI::Vulkan is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			texture = std::make_shared<DirectX12Texture2D>(path, uuid);
			break;
		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		default:
			AU_CORE_ERROR("Unknown RendererAPI!");
			return nullptr;
		}

		if (texture) {
			Application::Get().GetAssetRegistry().AddTexture(uuid, texture);
			Application::Get().GetAssetRegistry().RegisterAssetPath(uuid, AssetType::Texture, path);
		}

		return texture;
	}
	
}
