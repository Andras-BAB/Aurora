#include "aupch.h"
#include "Shader.h"

#include "RendererAPI.h"
#include "Aurora/Core/Assert.h"
#include "Aurora/Core/Log.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/Renderer/VulkanShader.h"

namespace Aurora {

	std::shared_ptr<Shader> Shader::Create(const std::string& vertexPath, const std::string& fragmentPath) {
		switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ASSERT(false, "RendererAPI::None is not supported")
			return nullptr;
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanShader>(vertexPath, fragmentPath);
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(vertexPath, fragmentPath);
		case RendererAPI::API::DirectX12:
			AU_CORE_ASSERT(false, "RendererAPI::DirectX12 is not supported")
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ASSERT(false, "RendererAPI::DirectX11 is not supported")
			return nullptr;
		}
		AU_CORE_ASSERT(false, "Unknown RendererAPI")
		return nullptr;
	}

	std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc,
										   const std::string& fragmentSrc) {
		switch (RendererAPI::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ASSERT(false, "RendererAPI::None is not supported")
			return nullptr;
		case RendererAPI::API::Vulkan:
			// TODO: give the sources to constructor
			AU_CORE_WARN("RendererAPI::Vulkan currently only gives the shader paths");
			return std::make_shared<VulkanShader>(vertexSrc, fragmentSrc);
		case RendererAPI::API::OpenGL:
			AU_CORE_ASSERT(false, "RendererAPI::OpenGL is WIP")
			return nullptr;
		case RendererAPI::API::DirectX12:
			AU_CORE_ASSERT(false, "RendererAPI::DirectX12 is not supported")
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ASSERT(false, "RendererAPI::DirectX11 is not supported")
			return nullptr;
		}
		AU_CORE_ASSERT(false, "Unknown RendererAPI")
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader) {
		if(Exists(name)) {
			AU_CORE_WARN("Shader already exists!");
			return;
		}
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader) {
		auto& name = shader->GetName();
		Add(name, shader);
	}

	//std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath)
	//{
	//	auto shader = Shader::Create(filepath);
	//	Add(shader);
	//	return shader;
	//}

	//std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	//{
	//	auto shader = Shader::Create(filepath);
	//	Add(name, shader);
	//	return shader;
	//}

	std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name) {
		if (!Exists(name)) {
			AU_CORE_WARN("Shader do not exists!");
			return nullptr;
		}
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const {
		return m_Shaders.contains(name);
	}

}
