#include "stdafx.h"
#include "Utils/d3dUtil.h"

#include "Platform/DirectX/DirectX12RenderCommand.h"

namespace d3dUtil {

    // From d3dx12.h - buffer upload helpers
    //------------------------------------------------------------------------------------------------
    // Row-by-row memcpy
    void MemcpySubresource(
        _In_ const D3D12_MEMCPY_DEST* pDest,
        _In_ const D3D12_SUBRESOURCE_DATA* pSrc,
        SIZE_T RowSizeInBytes,
        UINT NumRows,
        UINT NumSlices) {
        for (UINT z = 0; z < NumSlices; ++z) {
            BYTE* pDestSlice = reinterpret_cast<BYTE*>(pDest->pData) + pDest->SlicePitch * z;
            const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(pSrc->pData) + pSrc->SlicePitch * z;
            for (UINT y = 0; y < NumRows; ++y) {
                memcpy(pDestSlice + pDest->RowPitch * y,
                    pSrcSlice + pSrc->RowPitch * y,
                    RowSizeInBytes);
            }
        }
    }

    //------------------------------------------------------------------------------------------------
    // All arrays must be populated (e.g. by calling GetCopyableFootprints)
    UINT64 UpdateSubresources(
        _In_ ID3D12GraphicsCommandList* pCmdList,
        _In_ ID3D12Resource* pDestinationResource,
        _In_ ID3D12Resource* pIntermediate,
        _In_range_(0, D3D12_REQ_SUBRESOURCES) UINT FirstSubresource,
        _In_range_(0, D3D12_REQ_SUBRESOURCES - FirstSubresource) UINT NumSubresources,
        UINT64 RequiredSize,
        _In_reads_(NumSubresources) const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
        _In_reads_(NumSubresources) const UINT* pNumRows,
        _In_reads_(NumSubresources) const UINT64* pRowSizesInBytes,
        _In_reads_(NumSubresources) const D3D12_SUBRESOURCE_DATA* pSrcData) {
        // Minor validation
        D3D12_RESOURCE_DESC IntermediateDesc = pIntermediate->GetDesc();
        D3D12_RESOURCE_DESC DestinationDesc = pDestinationResource->GetDesc();
        if (IntermediateDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
            IntermediateDesc.Width < RequiredSize + pLayouts[0].Offset ||
            RequiredSize >(SIZE_T) - 1 ||
            (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
                (FirstSubresource != 0 || NumSubresources != 1))) {
            return 0;
        }

        BYTE* pData;
        HRESULT hr = pIntermediate->Map(0, NULL, reinterpret_cast<void**>(&pData));
        if (FAILED(hr)) {
            return 0;
        }

        for (UINT i = 0; i < NumSubresources; ++i) {
            if (pRowSizesInBytes[i] > (SIZE_T)-1) return 0;
            D3D12_MEMCPY_DEST DestData = { pData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, pLayouts[i].Footprint.RowPitch * pNumRows[i] };
            MemcpySubresource(&DestData, &pSrcData[i], (SIZE_T)pRowSizesInBytes[i], pNumRows[i], pLayouts[i].Footprint.Depth);
        }
        pIntermediate->Unmap(0, NULL);

        if (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {
            pCmdList->CopyBufferRegion(
                pDestinationResource, 0, pIntermediate, pLayouts[0].Offset, pLayouts[0].Footprint.Width);
        } else {
            for (UINT i = 0; i < NumSubresources; ++i) {
                D3D12_TEXTURE_COPY_LOCATION dst = {};
                dst.pResource = pDestinationResource;
                dst.SubresourceIndex = i + FirstSubresource;

                D3D12_TEXTURE_COPY_LOCATION src = {};
                src.pResource = pIntermediate;
                src.PlacedFootprint = pLayouts[i];
                pCmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
            }
        }
        return RequiredSize;
    }

    //------------------------------------------------------------------------------------------------
    // Heap-allocating UpdateSubresources implementation
    UINT64 UpdateSubresources(
        _In_ ID3D12GraphicsCommandList* pCmdList,
        _In_ ID3D12Resource* pDestinationResource,
        _In_ ID3D12Resource* pIntermediate,
        UINT64 IntermediateOffset,
        _In_range_(0, D3D12_REQ_SUBRESOURCES) UINT FirstSubresource,
        _In_range_(0, D3D12_REQ_SUBRESOURCES - FirstSubresource) UINT NumSubresources,
        _In_reads_(NumSubresources) D3D12_SUBRESOURCE_DATA* pSrcData) {
        UINT64 RequiredSize = 0;
        UINT64 MemToAlloc = static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * NumSubresources;
        if (MemToAlloc > SIZE_MAX) {
            return 0;
        }
        void* pMem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(MemToAlloc));
        if (pMem == NULL) {
            return 0;
        }
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pMem);
        UINT64* pRowSizesInBytes = reinterpret_cast<UINT64*>(pLayouts + NumSubresources);
        UINT* pNumRows = reinterpret_cast<UINT*>(pRowSizesInBytes + NumSubresources);

        D3D12_RESOURCE_DESC Desc = pDestinationResource->GetDesc();
        ID3D12Device* pDevice;
        pDestinationResource->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
        pDevice->GetCopyableFootprints(&Desc, FirstSubresource, NumSubresources, IntermediateOffset, pLayouts, pNumRows, pRowSizesInBytes, &RequiredSize);
        pDevice->Release();

        UINT64 Result = UpdateSubresources(pCmdList, pDestinationResource, pIntermediate, FirstSubresource, NumSubresources, RequiredSize, pLayouts, pNumRows, pRowSizesInBytes, pSrcData);
        HeapFree(GetProcessHeap(), 0, pMem);
        return Result;
    }

    //------------------------------------------------------------------------------------------------
    // Stack-allocating UpdateSubresources implementation
    template <UINT MaxSubresources>
    UINT64 UpdateSubresources(
        _In_ ID3D12GraphicsCommandList* pCmdList,
        _In_ ID3D12Resource* pDestinationResource,
        _In_ ID3D12Resource* pIntermediate,
        UINT64 IntermediateOffset,
        _In_range_(0, MaxSubresources) UINT FirstSubresource,
        _In_range_(1, MaxSubresources - FirstSubresource) UINT NumSubresources,
        _In_reads_(NumSubresources) D3D12_SUBRESOURCE_DATA* pSrcData) {
        UINT64 RequiredSize = 0;
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT Layouts[MaxSubresources];
        UINT NumRows[MaxSubresources];
        UINT64 RowSizesInBytes[MaxSubresources];

        D3D12_RESOURCE_DESC Desc = pDestinationResource->GetDesc();
        ID3D12Device* pDevice;
        pDestinationResource->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
        pDevice->GetCopyableFootprints(&Desc, FirstSubresource, NumSubresources, IntermediateOffset, Layouts, NumRows, RowSizesInBytes, &RequiredSize);
        pDevice->Release();

        return UpdateSubresources(pCmdList, pDestinationResource, pIntermediate, FirstSubresource, NumSubresources, RequiredSize, Layouts, NumRows, RowSizesInBytes, pSrcData);
    }

    /** End of d3dx12.h section ---------------------------------- **/


    MS::ComPtr<ID3DBlob> utils::LoadBinary(const std::wstring& filename) {
        std::ifstream fin(filename, std::ios::binary);

        fin.seekg(0, std::ios_base::end);
        std::ifstream::pos_type size = static_cast<int>(fin.tellg());
        fin.seekg(0, std::ios_base::beg);

        MS::ComPtr<ID3DBlob> blob;
        ThrowOnFail(D3DCreateBlob(size, blob.GetAddressOf()));

        fin.read(static_cast<char*>(blob->GetBufferPointer()), size);
        fin.close();

        return blob;
    }

    MS::ComPtr<ID3D12Resource> utils::CreateDefaultBuffer(
	        ID3D12Device* device,
	        ID3D12GraphicsCommandList* cmdList,
	        const void* initData,
	        UINT64 byteSize,
	        MS::ComPtr<ID3D12Resource>& uploadBuffer) {

        ID3D12Device* _device = Core::DirectX12RenderCommand::GetContext()->GetDevice();
		ID3D12GraphicsCommandList* _cmdList = Core::DirectX12RenderCommand::GetContext()->GetCommandList();

        MS::ComPtr<ID3D12Resource> defaultBuffer;

        // Create the actual default buffer resource.
        D3D12_HEAP_PROPERTIES hProps = {};
        hProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        D3D12_RESOURCE_DESC rd = {};
        rd.Width = byteSize;
        rd.Height = 1;
        rd.Flags = D3D12_RESOURCE_FLAG_NONE;
        rd.Alignment = 0;
        rd.DepthOrArraySize = 1;
        rd.MipLevels = 1;
        rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        rd.SampleDesc.Count = 1;
        rd.SampleDesc.Quality = 0;
        rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        ThrowOnFail(_device->CreateCommittedResource(
            &hProps,
            D3D12_HEAP_FLAG_NONE,
            &rd,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

        // In order to copy CPU memory data into our default buffer, we need to create
        // an intermediate upload heap.
        D3D12_HEAP_PROPERTIES hp = {};
        hp.Type = D3D12_HEAP_TYPE_UPLOAD;

        ThrowOnFail(_device->CreateCommittedResource(
            &hp,
            D3D12_HEAP_FLAG_NONE,
            &rd,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


        // Describe the data we want to copy into the default buffer.
        D3D12_SUBRESOURCE_DATA subResourceData = {};
        subResourceData.pData = initData;
        subResourceData.RowPitch = static_cast<long long>(byteSize);
        subResourceData.SlicePitch = subResourceData.RowPitch;

        // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
        // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
        // the intermediate upload heap data will be copied to mBuffer.
        D3D12_RESOURCE_BARRIER rb = {};
        rb.Transition.pResource = defaultBuffer.Get();
        rb.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
        rb.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
        _cmdList->ResourceBarrier(1, &rb);

        UpdateSubresources<1>(_cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

        D3D12_RESOURCE_BARRIER rbr = {};
        rbr.Transition.pResource = defaultBuffer.Get();
        rbr.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        rbr.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
        _cmdList->ResourceBarrier(1, &rbr);

        // Note: uploadBuffer has to be kept alive after the above function calls because
        // the command list has not been executed yet that performs the actual copy.
        // The caller can Release the uploadBuffer after it knows the copy has been executed.

        return defaultBuffer;
    }

    MS::ComPtr<ID3DBlob> utils::CompileShader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target) {
        UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        HRESULT hr = S_OK;

        MS::ComPtr<ID3DBlob> byteCode = nullptr;
        MS::ComPtr<ID3DBlob> errors;
        hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

        if (errors != nullptr)
            OutputDebugStringA(static_cast<char*>(errors->GetBufferPointer()));

        ThrowOnFail(hr);

        return byteCode;
    }
}
