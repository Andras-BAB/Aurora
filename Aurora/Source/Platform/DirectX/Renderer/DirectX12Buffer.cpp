#include "aupch.h"
#include "DirectX12Buffer.h"

#include "DirectX12Context.h"
#include "DirectX12RenderCommand.h"
#include "Aurora/Core/Application.h"
#include "Aurora/Renderer/RenderCommand.h"

namespace Aurora {
    // Vertex buffer
    DirectX12VertexBuffer::DirectX12VertexBuffer(uint32_t size) {
        //m_GraphicsContext = dynamic_cast<WindowsWindow&>(Application::Get().GetWindow()).GetGraphicsContextAs<DirectX12Context>();
        m_GraphicsContext = DirectX12RenderCommand::GetContext();

        ThrowOnFail(D3DCreateBlob(size, m_CPUBuffer.GetAddressOf()));

        D3D12_HEAP_PROPERTIES uploadProps = {};
        uploadProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

        D3D12_RESOURCE_DESC rd = {};
        rd.Width = size;
        rd.Height = 1;
        rd.Flags = D3D12_RESOURCE_FLAG_NONE;
        rd.Alignment = 0;
        rd.DepthOrArraySize = 1;
        rd.MipLevels = 1;
        rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        rd.SampleDesc.Count = 1;
        rd.SampleDesc.Quality = 0;
        rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        ThrowOnFail(m_GraphicsContext->GetDevice()->CreateCommittedResource(
            &uploadProps,
            D3D12_HEAP_FLAG_NONE,
            &rd,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            nullptr,
            IID_PPV_ARGS(m_UploadBuffer.GetAddressOf())));

        ThrowOnFail(m_GraphicsContext->GetDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &rd,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(m_GPUBuffer.GetAddressOf())));
        
    }

    DirectX12VertexBuffer::DirectX12VertexBuffer(float* vertices, uint32_t size) {
        //m_GraphicsContext = dynamic_cast<WindowsWindow&>(Application::Get().GetWindow()).GetGraphicsContextAs<DirectX12Context>();
        m_GraphicsContext = DirectX12RenderCommand::GetContext();

        ThrowOnFail(D3DCreateBlob(size, m_CPUBuffer.GetAddressOf()));
        memcpy(m_CPUBuffer->GetBufferPointer(), vertices, size);
        m_GPUBuffer = d3dUtil::utils::CreateDefaultBuffer(m_GraphicsContext->GetDevice(), m_GraphicsContext->GetCommandList(),
            vertices, size, m_UploadBuffer);

        m_BufferView.BufferLocation = m_GPUBuffer->GetGPUVirtualAddress();
        m_BufferView.SizeInBytes = size;
        m_BufferView.StrideInBytes = m_BufferLayout.GetStride();
    }

    void DirectX12VertexBuffer::SetData(const void* data, uint32_t size) {
        
        memcpy(m_CPUBuffer.Get(), data, size);

        D3D12_RESOURCE_BARRIER barrier[2];
        barrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier[0].Transition.pResource = m_UploadBuffer.Get();
        barrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
        barrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier[1].Transition.pResource = m_GPUBuffer.Get();
        barrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        barrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
        m_GraphicsContext->GetCommandList()->ResourceBarrier(2, barrier);
        
        memcpy(m_UploadBuffer.Get(), data, size);
        memcpy(m_GPUBuffer.Get(), m_UploadBuffer.Get(), size);

        barrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier[0].Transition.pResource = m_UploadBuffer.Get();
        barrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
        barrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier[1].Transition.pResource = m_GPUBuffer.Get();
        barrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        m_GraphicsContext->GetCommandList()->ResourceBarrier(2, barrier); 

        m_BufferView.BufferLocation = m_GPUBuffer->GetGPUVirtualAddress();
        m_BufferView.SizeInBytes = size;
        m_BufferView.StrideInBytes = m_BufferLayout.GetStride();
    }

    void DirectX12VertexBuffer::Bind() const {
        m_GraphicsContext->GetCommandList()->IASetVertexBuffers(0, 1, &m_BufferView);
    }

    void DirectX12VertexBuffer::Unbind() const {
        m_GraphicsContext->GetCommandList()->IASetVertexBuffers(0, 1, nullptr);
    }

    void DirectX12VertexBuffer::SetLayout(const BufferLayout& layout) {
        m_BufferLayout = layout;
    }

    const BufferLayout& DirectX12VertexBuffer::GetLayout() const {
        return m_BufferLayout;
    }

    DirectX12IndexBuffer::DirectX12IndexBuffer(uint32_t size) {
    }

    // Index buffer
    DirectX12IndexBuffer::DirectX12IndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count) {
        //m_GraphicsContext = dynamic_cast<WindowsWindow&>(Application::Get().GetWindow()).GetGraphicsContextAs<DirectX12Context>();
        m_GraphicsContext = DirectX12RenderCommand::GetContext();

        uint64_t sizeInBytes = count * sizeof(uint32_t);

        ThrowOnFail(D3DCreateBlob(sizeInBytes, m_CPUBuffer.GetAddressOf()));
        memcpy(m_CPUBuffer->GetBufferPointer(), indices, sizeInBytes);
        m_GPUBuffer = d3dUtil::utils::CreateDefaultBuffer(m_GraphicsContext->GetDevice(), m_GraphicsContext->GetCommandList(),
            indices, sizeInBytes, m_UploadBuffer);

        m_BufferView.BufferLocation = m_GPUBuffer->GetGPUVirtualAddress();
        m_BufferView.SizeInBytes = static_cast<UINT>(sizeInBytes);
        m_BufferView.Format = DXGI_FORMAT_R32_UINT;
    }

    void DirectX12IndexBuffer::Bind() const { 
        m_GraphicsContext->GetCommandList()->IASetIndexBuffer(&m_BufferView);
     }

    void DirectX12IndexBuffer::Unbind() const {
        m_GraphicsContext->GetCommandList()->IASetIndexBuffer(nullptr);
    }

    uint32_t DirectX12IndexBuffer::GetCount() const {
        return m_Count;
    }
    
}
