#pragma once

#include "Aurora/Renderer/Shader.h"
#include "Platform/DirectX/Utils/MSUtils.h"

namespace Aurora {

	enum class ShaderType : uint8_t {
		Vertex,
		Pixel,
		Domain,
		Hull,
		Geometry,
		Compute
	};
	
	class DirectX12VertexShader : public Shader {
	public:
		DirectX12VertexShader(const std::string& filename, const std::string& name);
		~DirectX12VertexShader() override = default;

		//void Bind() override;
		//void Unbind() override;

		const std::string& GetName() override;

		ID3DBlob* GetShaderCode() const;
		
	private:
		std::string m_Name;
		MS::ComPtr<ID3DBlob> m_Shader;
	};

	class DirectX12PixelShader : public Shader {
	public:
		DirectX12PixelShader(const std::string& filename, const std::string& name);
		~DirectX12PixelShader() override = default;

		//void Bind() override;
		//void Unbind() override;

		const std::string& GetName() override;

		ID3DBlob* GetShaderCode() const;
		
	private:
		std::string m_Name;
		MS::ComPtr<ID3DBlob> m_Shader;
	};    

}
