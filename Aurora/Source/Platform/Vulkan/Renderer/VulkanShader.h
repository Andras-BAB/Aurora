#pragma once
#include "Aurora/Renderer/Shader.h"

#include <filesystem>
#include <string>
#include <vulkan/vulkan.h>

namespace Aurora {
	class VulkanShader : public Shader {
	public:
		VulkanShader(const std::filesystem::path& vertexPath,
			const std::filesystem::path& fragmentPath,
			const std::filesystem::path& geometryPath = "");

		~VulkanShader() override;
		
		const std::string& GetName() override;

		VkShaderModule GetVertexHandle() const { return m_VertexShader; }
		VkShaderModule GetFragmentHandle() const { return m_FragmentShader; }

	private:
		static VkShaderModule CreateShaderModule(const std::vector<char>& code);
		
	private:
		std::string m_Name;

		VkShaderModule m_VertexShader;
		VkShaderModule m_FragmentShader;
	};
}
