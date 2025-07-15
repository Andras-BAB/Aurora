#pragma once
#include "Aurora/Renderer/Shader.h"

#include <filesystem>
#include <string>

namespace Aurora {
	class VulkanShader : public Shader {
	public:
		VulkanShader(const std::filesystem::path& vertexPath,
			const std::filesystem::path& fragmentPath,
			const std::filesystem::path& geometryPath = "");
		virtual ~VulkanShader() override = default;

		virtual void Bind();
		virtual void Unbind();

		virtual const std::string& GetName();

	private:
		std::string m_Name;
	};
}
