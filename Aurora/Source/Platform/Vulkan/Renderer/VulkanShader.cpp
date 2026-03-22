#include "VulkanShader.h"

#include "VulkanRenderCommand.h"
#include "Aurora/Utils/ReadFile.h"
#include "Platform/Vulkan/Utils/VulkanHelper.h"

namespace Aurora {
	VulkanShader::VulkanShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath,
		const std::filesystem::path& geometryPath) {

		auto vertexSrc = Utils::ReadBinaryFile(vertexPath.string());
		auto fragmentSrc = Utils::ReadBinaryFile(fragmentPath.string());

		m_VertexShader = CreateShaderModule(vertexSrc);
		m_FragmentShader = CreateShaderModule(fragmentSrc);
		m_Name = vertexPath.stem().string();
	}

	VulkanShader::~VulkanShader() {
		vkDestroyShaderModule(VulkanRenderCommand::GetDevice(), m_VertexShader, nullptr);
		vkDestroyShaderModule(VulkanRenderCommand::GetDevice(), m_FragmentShader, nullptr);
	}

	const std::string& VulkanShader::GetName() {
		return m_Name;
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::vector<char>& code) {
		VkDevice device = VulkanRenderCommand::GetDevice();
		
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));

		return shaderModule;
	}
}
