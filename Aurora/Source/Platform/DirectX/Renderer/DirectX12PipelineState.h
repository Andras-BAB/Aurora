#pragma once
#include "DirectX12Shader.h"
//#include "Aurora/Renderer/IRenderPipeline.h"
#include "Platform/DirectX/Utils/MSUtils.h"

namespace Aurora {

	enum class CullMode { None, Front, Back };
	enum class DepthMode { ReadWrite, Read, None };
	enum class BlendMode { Opaque, AlphaBlend, Additive };
	enum class TopologyType { Point, Line, Triangle };

	struct PipelineConfig {
		std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
		std::shared_ptr<DirectX12VertexShader> VertexShader;
		std::shared_ptr<DirectX12PixelShader> PixelShader;

		DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		CullMode Cull = CullMode::Back;
		DepthMode Depth = DepthMode::ReadWrite;
		BlendMode Blend = BlendMode::Opaque;
		TopologyType Topology = TopologyType::Triangle;
		bool Wireframe = false;

		bool operator==(const PipelineConfig& other) const {
			return VertexShader == other.VertexShader &&
				PixelShader == other.PixelShader &&
				BackBufferFormat == other.BackBufferFormat &&
				DepthStencilFormat == other.DepthStencilFormat &&
				Cull == other.Cull && Depth == other.Depth &&
				Blend == other.Blend && Topology == other.Topology &&
				Wireframe == other.Wireframe;
		}
	};

	struct PipelineConfigHasher {
		std::size_t operator()(const PipelineConfig& conf) const {
			std::size_t seed = 0;
			auto hash_combine = [&seed](auto v) {
				seed ^= std::hash<decltype(v)>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			};

			hash_combine(conf.VertexShader.get());
			hash_combine(conf.PixelShader.get());
			hash_combine(conf.BackBufferFormat);
			hash_combine(conf.DepthStencilFormat);
			hash_combine(static_cast<int>(conf.Cull));
			hash_combine(static_cast<int>(conf.Depth));
			hash_combine(static_cast<int>(conf.Blend));
			hash_combine(static_cast<int>(conf.Topology));
			hash_combine(conf.Wireframe);
			return seed;
		}
	};
		
	class DirectX12PipelineState {
	public:
		DirectX12PipelineState(ID3D12Device* device, ID3D12RootSignature* rootSig, const PipelineConfig& config, const std::string& name);

		ID3D12PipelineState* GetPipelineState() const;
		const PipelineConfig& GetConfig() const { return m_Config; }
		std::string& GetName();

	private:
		MS::ComPtr<ID3D12PipelineState> m_PipelineState;
		PipelineConfig m_Config;
		std::string m_Name;
	};

	class DirectX12PipelineStateLibrary {
	public:
		void Init(ID3D12Device* device);
		void Clear();

		ID3D12RootSignature* GetUberRootSignature() const { return m_UberRootSignature.Get(); }

		std::shared_ptr<DirectX12PipelineState> GetOrCreate(const PipelineConfig& config, const std::string& debugName = "Unnamed");
		std::shared_ptr<DirectX12PipelineState> Get(const std::string& name);

	private:
		void CreateUberRootSignature();

	private:
		ID3D12Device* m_Device = nullptr;
		MS::ComPtr<ID3D12RootSignature> m_UberRootSignature;

		std::unordered_map<std::size_t, std::shared_ptr<DirectX12PipelineState>> m_Cache;
		std::unordered_map<std::string, std::size_t> m_NameMap;
	};
	
}
