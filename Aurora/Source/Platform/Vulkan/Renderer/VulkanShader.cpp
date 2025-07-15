#include "VulkanShader.h"

namespace Aurora {
	VulkanShader::VulkanShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath,
		const std::filesystem::path& geometryPath) {
	}

	void VulkanShader::Bind() {
	}

	void VulkanShader::Unbind() {
	}

	const std::string& VulkanShader::GetName() {
		return m_Name;
	}
}
