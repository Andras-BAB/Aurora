#pragma once
#include "DirectX12Shader.h"
#include "Platform/DirectX/Utils/MSUtils.h"

namespace Aurora {

    struct PipelineStateProperties {
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
        std::shared_ptr<DirectX12VertexShader> vertexShader;
        std::shared_ptr<DirectX12PixelShader> pixelShader;
        DXGI_FORMAT backBufferFormat;
        DXGI_FORMAT depthStencilFormat;
        ID3D12Device* device;
        bool isWireframe = false;
    };
    
    class DirectX12PipelineState {
    public:
        DirectX12PipelineState(const PipelineStateProperties& properties, const std::string& name);

        ID3D12PipelineState* GetPipelineState() const;
        ID3D12RootSignature* GetRootSignature() const;
        PipelineStateProperties& GetPipelineStateProperties();
        std::string& GetName();

    private:
        void CreateRootSignature();

    private:
        MS::ComPtr<ID3D12PipelineState> m_PipelineState;
        MS::ComPtr<ID3D12RootSignature> m_RootSignature;
        PipelineStateProperties m_Properties;
        std::string m_Name;
    };

    class DirectX12PipelineStateLibrary {
    public:
        void AddPipeline(const std::string& name, const std::shared_ptr<DirectX12PipelineState>& shader);
        void AddPipeline(const std::shared_ptr<DirectX12PipelineState>& pipelineState);
        std::shared_ptr<DirectX12PipelineState> Get(const std::string& name);

        bool Exists(const std::string& name) const;

        std::shared_ptr<DirectX12PipelineState> CreatePipeline(const PipelineStateProperties& props, const std::string& name);

        void Clear();
        
    private:
        std::unordered_map<std::string, std::shared_ptr<DirectX12PipelineState>> m_PipelineStates;
    };
    
}
