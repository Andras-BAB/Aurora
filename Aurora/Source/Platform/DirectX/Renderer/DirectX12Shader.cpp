#include "aupch.h"
#include "Platform/DirectX/Renderer/DirectX12Shader.h"

#include "Platform/DirectX/Utils/d3dUtil.h"

namespace Aurora {

	DirectX12VertexShader::DirectX12VertexShader(const std::string& filename, const std::string& name) : m_Name(name) {
		m_Shader = d3dUtil::utils::CompileShader(MS::stow(filename.c_str()), nullptr, "VS", "vs_6_6");
	}

	const std::string& DirectX12VertexShader::GetName() {
		return m_Name;
	}

	ID3DBlob* DirectX12VertexShader::GetShaderCode() const {
		return m_Shader.Get();
	}

	DirectX12PixelShader::DirectX12PixelShader(const std::string& filename, const std::string& name) : m_Name(name) {
		m_Shader = d3dUtil::utils::CompileShader(MS::stow(filename.c_str()), nullptr, "PS", "ps_6_6");
	}

	const std::string& DirectX12PixelShader::GetName() {
		return m_Name;
	}

	ID3DBlob* DirectX12PixelShader::GetShaderCode() const {
		return m_Shader.Get();
	}

	DirectX12ComputeShader::DirectX12ComputeShader(const std::string& filename, const std::string& name) {
		m_Shader = d3dUtil::utils::CompileShader(MS::stow(filename.c_str()), nullptr, "main", "cs_6_6");
	}

	const std::string& DirectX12ComputeShader::GetName() {
		return m_Name;
	}

	ID3DBlob* DirectX12ComputeShader::GetShaderCode() const {
		return m_Shader.Get();
	}
}
