#include "aupch.h"
#include "DirectX12PipelineState.h"

#include "Aurora/Core/Log.h"

namespace Aurora {
    DirectX12PipelineState::DirectX12PipelineState(const PipelineStateProperties& properties, const std::string& name)
        : m_Properties(properties), m_Name(name) {

    	CreateRootSignature();
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

        ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		
        psoDesc.InputLayout = { m_Properties.inputLayout.data(), (UINT) m_Properties.inputLayout.size() };
        psoDesc.pRootSignature = m_RootSignature.Get();
		psoDesc.VS = 
		{ 
			reinterpret_cast<BYTE*>(m_Properties.vertexShader->GetShaderCode()->GetBufferPointer()), 
			m_Properties.vertexShader->GetShaderCode()->GetBufferSize()
		};
		psoDesc.PS = 
		{ 
			reinterpret_cast<BYTE*>(m_Properties.pixelShader->GetShaderCode()->GetBufferPointer()),
			m_Properties.pixelShader->GetShaderCode()->GetBufferSize()
		};

        D3D12_RASTERIZER_DESC rasterizer_desc = {};
    	if(m_Properties.isWireframe) {
    		rasterizer_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
    	} else {
    		rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
    	}
    	rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
        rasterizer_desc.FrontCounterClockwise = FALSE;
        rasterizer_desc.MultisampleEnable = TRUE;

        D3D12_BLEND_DESC blend_desc = {};
        blend_desc.AlphaToCoverageEnable = FALSE;
        blend_desc.IndependentBlendEnable = FALSE;
		
        D3D12_RENDER_TARGET_BLEND_DESC rtbd = {};
        rtbd.BlendEnable = FALSE;
        rtbd.LogicOpEnable = FALSE;
        rtbd.SrcBlend = D3D12_BLEND_ONE;
        rtbd.DestBlend = D3D12_BLEND_ZERO;
        rtbd.BlendOp = D3D12_BLEND_OP_ADD;
        rtbd.SrcBlendAlpha = D3D12_BLEND_ONE;
        rtbd.DestBlendAlpha = D3D12_BLEND_ZERO;
        rtbd.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        rtbd.LogicOp = D3D12_LOGIC_OP_NOOP;
        rtbd.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        for (D3D12_RENDER_TARGET_BLEND_DESC& i : blend_desc.RenderTarget) {
	        i = rtbd;
        }

        D3D12_DEPTH_STENCIL_DESC ds = {};
        ds.DepthEnable = TRUE;
        ds.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        ds.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        ds.StencilEnable = FALSE;
        ds.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
        ds.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
        const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
        { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
        ds.FrontFace = defaultStencilOp;
        ds.BackFace = defaultStencilOp;

        psoDesc.RasterizerState = rasterizer_desc;
        psoDesc.BlendState = blend_desc;
        psoDesc.DepthStencilState = ds;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = m_Properties.backBufferFormat;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;
        psoDesc.DSVFormat = m_Properties.depthStencilFormat;
        ThrowOnFail(m_Properties.device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState)));

		D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = psoDesc;
		//opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		ThrowOnFail(m_Properties.device->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&m_PipelineState)));
        
    }

    ID3D12PipelineState* DirectX12PipelineState::GetPipelineState() const {
    	return m_PipelineState.Get();
    }

    ID3D12RootSignature* DirectX12PipelineState::GetRootSignature() const {
    	return m_RootSignature.Get();
    }

    PipelineStateProperties& DirectX12PipelineState::GetPipelineStateProperties() {
    	return m_Properties;
    }

    std::string& DirectX12PipelineState::GetName() {
        return m_Name;
    }

    void DirectX12PipelineState::CreateRootSignature() {
    	// Root parameter can be a table, root descriptor or root constants.
        D3D12_ROOT_PARAMETER slotRootParameter[3];

        // Create a single descriptor table of CBVs.
        D3D12_DESCRIPTOR_RANGE objCbvRange;
	    objCbvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	    objCbvRange.NumDescriptors = 1;
	    objCbvRange.BaseShaderRegister = 0;
	    objCbvRange.RegisterSpace = 0;
	    objCbvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_DESCRIPTOR_RANGE matCbvRange;
		matCbvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		matCbvRange.NumDescriptors = 1;
		matCbvRange.BaseShaderRegister = 1;
		matCbvRange.RegisterSpace = 0;
		matCbvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_DESCRIPTOR_RANGE passCbvRange;
        passCbvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        passCbvRange.NumDescriptors = 1;
        passCbvRange.BaseShaderRegister = 2;
        passCbvRange.RegisterSpace = 0;
        passCbvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        //slotRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        slotRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        slotRootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        slotRootParameter[0].DescriptorTable.NumDescriptorRanges = 1;
        slotRootParameter[0].DescriptorTable.pDescriptorRanges = &objCbvRange;
        //slotRootParameter[0].Descriptor.ShaderRegister = 0;
        //slotRootParameter[0].Descriptor.RegisterSpace = 0;

		slotRootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		slotRootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        slotRootParameter[1].DescriptorTable.NumDescriptorRanges = 1;
        slotRootParameter[1].DescriptorTable.pDescriptorRanges = &matCbvRange;
		//slotRootParameter[1].Descriptor.ShaderRegister = 1;
		//slotRootParameter[1].Descriptor.RegisterSpace = 0;

        slotRootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        slotRootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        slotRootParameter[2].DescriptorTable.NumDescriptorRanges = 1;
        slotRootParameter[2].DescriptorTable.pDescriptorRanges = &passCbvRange;
        //slotRootParameter[2].Descriptor.ShaderRegister = 2;
        //slotRootParameter[2].Descriptor.RegisterSpace = 0;
		
        // A root signature is an array of root parameters.
	    D3D12_ROOT_SIGNATURE_DESC rootSigDesc;
	    rootSigDesc.NumParameters = _countof(slotRootParameter);
	    rootSigDesc.pParameters = slotRootParameter;
	    rootSigDesc.NumStaticSamplers = 0;
	    rootSigDesc.pStaticSamplers = nullptr;
	    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
        MS::ComPtr<ID3DBlob> serializedRootSig = nullptr;
        MS::ComPtr<ID3DBlob> errorBlob = nullptr;

        HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
            serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

        if (errorBlob != nullptr) {
            ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        ThrowOnFail(hr);

        ThrowOnFail(m_Properties.device->CreateRootSignature(
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(&m_RootSignature)));
    }

    void DirectX12PipelineStateLibrary::AddPipeline(const std::string& name,
                                                    const std::shared_ptr<DirectX12PipelineState>& shader) {
        if(Exists(name)) {
            AU_CORE_LOG_WARN("Shader already exists!");
            return;
        }
        m_PipelineStates[name] = shader;
    }

    void DirectX12PipelineStateLibrary::AddPipeline(const std::shared_ptr<DirectX12PipelineState>& pipelineState) {
        auto& name = pipelineState->GetName();
        AddPipeline(name, pipelineState);
    }

    std::shared_ptr<DirectX12PipelineState> DirectX12PipelineStateLibrary::Get(const std::string& name) {
        if(!Exists(name)) {
            AU_CORE_LOG_WARN("Pipeline state '{0}' does not exist", name);
            return nullptr;
        }
        return m_PipelineStates[name];
    }

    bool DirectX12PipelineStateLibrary::Exists(const std::string& name) const {
        return m_PipelineStates.contains(name);
    }

    std::shared_ptr<DirectX12PipelineState> DirectX12PipelineStateLibrary::CreatePipeline(
	    const PipelineStateProperties& props, const std::string& name) {
    	if(!Exists(name)) {
    		AddPipeline(std::make_shared<DirectX12PipelineState>(props, name));
			return Get(name);
    	}
    	return nullptr;
    }

    void DirectX12PipelineStateLibrary::Clear() {
        m_PipelineStates.clear();
    }
}
