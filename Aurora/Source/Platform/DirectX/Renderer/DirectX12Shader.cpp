#include "aupch.h"
#include "Platform/DirectX/Renderer/DirectX12Shader.h"

#include "Platform/DirectX/Utils/d3dUtil.h"

namespace Aurora {

	// Vertex shader
	DirectX12VertexShader::DirectX12VertexShader(const std::string& filename, const std::string& name) : m_Name(name) {
		m_Shader = d3dUtil::utils::CompileShader(MS::stow(filename.c_str()), nullptr, "VS", "vs_5_1");
	}

	void DirectX12VertexShader::Bind() {
	}

	void DirectX12VertexShader::Unbind() {
	}

	const std::string& DirectX12VertexShader::GetName() {
		return m_Name;
	}

	ID3DBlob* DirectX12VertexShader::GetShaderCode() const {
		return m_Shader.Get();
	}

	// Pixel shader
	DirectX12PixelShader::DirectX12PixelShader(const std::string& filename, const std::string& name) : m_Name(name) {
		m_Shader = d3dUtil::utils::CompileShader(MS::stow(filename.c_str()), nullptr, "PS", "ps_5_1");
	}

	void DirectX12PixelShader::Bind() {
	}

	void DirectX12PixelShader::Unbind() {
	}

	const std::string& DirectX12PixelShader::GetName() {
		return m_Name;
	}

	ID3DBlob* DirectX12PixelShader::GetShaderCode() const {
		return m_Shader.Get();
	}
}
