#include "aupch.h"
#include "DirectX12PipelineState.h"

#include "Aurora/Core/Log.h"

namespace Aurora {

	DirectX12PipelineState::DirectX12PipelineState(ID3D12Device* device, ID3D12RootSignature* rootSig, 
		const PipelineConfig& config, const std::string& name) : m_Config(config), m_Name(name) {

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		psoDesc.InputLayout = { m_Config.InputLayout.data(), (UINT)m_Config.InputLayout.size() };
		psoDesc.pRootSignature = rootSig;

		if (m_Config.VertexShader) {
			psoDesc.VS = { reinterpret_cast<BYTE*>(m_Config.VertexShader->GetShaderCode()->GetBufferPointer()), m_Config.VertexShader->GetShaderCode()->GetBufferSize() };
		}
		if (m_Config.PixelShader) {
			psoDesc.PS = { reinterpret_cast<BYTE*>(m_Config.PixelShader->GetShaderCode()->GetBufferPointer()), m_Config.PixelShader->GetShaderCode()->GetBufferSize() };
		}

		D3D12_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = m_Config.Wireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;

		if (m_Config.Cull == CullMode::None) rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		else if (m_Config.Cull == CullMode::Front) rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
		else rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;

		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.MultisampleEnable = TRUE;
		psoDesc.RasterizerState = rasterizerDesc;

		D3D12_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;

		D3D12_RENDER_TARGET_BLEND_DESC rtbd = {};
		rtbd.LogicOpEnable = FALSE;
		rtbd.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		if (m_Config.Blend == BlendMode::Opaque) {
			rtbd.BlendEnable = FALSE;
		} else if (m_Config.Blend == BlendMode::AlphaBlend) {
			rtbd.BlendEnable = TRUE;
			rtbd.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			rtbd.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			rtbd.BlendOp = D3D12_BLEND_OP_ADD;
			rtbd.SrcBlendAlpha = D3D12_BLEND_ONE;
			rtbd.DestBlendAlpha = D3D12_BLEND_ZERO;
			rtbd.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		} else if (m_Config.Blend == BlendMode::Additive) {
			rtbd.BlendEnable = TRUE;
			rtbd.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			rtbd.DestBlend = D3D12_BLEND_ONE;
			rtbd.BlendOp = D3D12_BLEND_OP_ADD;
			rtbd.SrcBlendAlpha = D3D12_BLEND_ONE;
			rtbd.DestBlendAlpha = D3D12_BLEND_ONE;
			rtbd.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		}

		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
			blendDesc.RenderTarget[i] = rtbd;
		}
		psoDesc.BlendState = blendDesc;

		D3D12_DEPTH_STENCIL_DESC dsDesc = {};
		if (m_Config.Depth == DepthMode::None) {
			dsDesc.DepthEnable = FALSE;
			dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		} else if (m_Config.Depth == DepthMode::Read) {
			dsDesc.DepthEnable = TRUE;
			dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			//dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
			dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		} else {
			dsDesc.DepthEnable = TRUE;
			dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			//dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		}
		dsDesc.StencilEnable = FALSE;
		psoDesc.DepthStencilState = dsDesc;

		if (m_Config.Topology == TopologyType::Line) psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		else if (m_Config.Topology == TopologyType::Point) psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		else psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		psoDesc.SampleMask = UINT_MAX;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = m_Config.BackBufferFormat;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		psoDesc.DSVFormat = m_Config.DepthStencilFormat;

		ThrowOnFail(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));
	}

	ID3D12PipelineState* DirectX12PipelineState::GetPipelineState() const {
		return m_PipelineState.Get();
	}

	std::string& DirectX12PipelineState::GetName() {
		return m_Name;
	}

	std::shared_ptr<DirectX12PipelineState> DirectX12PipelineStateLibrary::GetOrCreate(const PipelineConfig& config, const std::string& debugName) {
		std::size_t hash = PipelineConfigHasher()(config);

		auto it = m_Cache.find(hash);
		if (it != m_Cache.end()) {
			return it->second;
		}

		auto newPso = std::make_shared<DirectX12PipelineState>(m_Device, m_UberRootSignature.Get(), config, debugName);
		m_Cache[hash] = newPso;

		AU_CORE_INFO("Compiled new PSO. Cache size: {0}", m_Cache.size());
		return newPso;
	}

	std::shared_ptr<DirectX12PipelineState> DirectX12PipelineStateLibrary::Get(const std::string& name) {
		return m_Cache[m_NameMap[name]];
	}

	void DirectX12PipelineStateLibrary::CreateUberRootSignature() {
		// Root parameter can be a table, root descriptor or root constants.
		D3D12_ROOT_PARAMETER slotRootParameter[4];

		// Slot 0: Object Constant Buffer
		slotRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		slotRootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		slotRootParameter[0].Descriptor.ShaderRegister = 0;
		slotRootParameter[0].Descriptor.RegisterSpace = 0;

		// Slot 1: Material Constant Buffer
		slotRootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		slotRootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		slotRootParameter[1].Descriptor.ShaderRegister = 1;
		slotRootParameter[1].Descriptor.RegisterSpace = 0;

		// Slot 2: Pass Constant Buffer
		slotRootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		slotRootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		slotRootParameter[2].Descriptor.ShaderRegister = 2;
		slotRootParameter[2].Descriptor.RegisterSpace = 0;

		D3D12_DESCRIPTOR_RANGE srvRange;
		srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRange.NumDescriptors = -1;
		srvRange.BaseShaderRegister = 0;
		srvRange.RegisterSpace = 0;
		srvRange.OffsetInDescriptorsFromTableStart = 0;

		slotRootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		slotRootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		slotRootParameter[3].DescriptorTable.NumDescriptorRanges = 1;
		slotRootParameter[3].DescriptorTable.pDescriptorRanges = &srvRange;

		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 1;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_ROOT_SIGNATURE_DESC rootSigDesc;
		rootSigDesc.NumParameters = _countof(slotRootParameter);
		rootSigDesc.pParameters = slotRootParameter;
		rootSigDesc.NumStaticSamplers = 1;
		rootSigDesc.pStaticSamplers = &sampler;
		rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		MS::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		MS::ComPtr<ID3DBlob> errorBlob = nullptr;

		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr) {
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowOnFail(hr);

		ThrowOnFail(m_Device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&m_UberRootSignature)));
	}

	void DirectX12PipelineStateLibrary::Init(ID3D12Device* device) {
		m_Device = device;
		CreateUberRootSignature();
	}

	void DirectX12PipelineStateLibrary::Clear() {
		m_Cache.clear();
		m_UberRootSignature.Reset();
	}
}
