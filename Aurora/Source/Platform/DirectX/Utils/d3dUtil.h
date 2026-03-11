#pragma once
#include "MSUtils.h"

#include <windows.h>
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "MathHelper.h"
#include "Platform/DirectX/DirectX12HeapManager.h"

//extern const int g_NumFrameResources;

namespace d3dUtil {
	class utils {
	public:
	    static UINT CalcConstantBufferByteSize(UINT byteSize) {
	        // Constant buffers must be a multiple of the minimum hardware
	        // allocation size (usually 256 bytes).  So round up to nearest
	        // multiple of 256.  We do this by adding 255 and then masking off
	        // the lower 2 bytes which store all bits < 256.
	        // Example: Suppose byteSize = 300.
	        // (300 + 255) & ~255
	        // 555 & ~255
	        // 0x022B & ~0x00ff
	        // 0x022B & 0xff00
	        // 0x0200
	        // 512
	        return (byteSize + 255) & ~255;
	    }

	    static MS::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

	    static MS::ComPtr<ID3D12Resource> CreateDefaultBuffer(
	        ID3D12Device* device,
	        ID3D12GraphicsCommandList* cmdList,
	        const void* initData,
	        UINT64 byteSize,
	        MS::ComPtr<ID3D12Resource>& uploadBuffer);

	    static MS::ComPtr<ID3DBlob> CompileShader(
	        const std::wstring& filename,
	        const D3D_SHADER_MACRO* defines,
	        const std::string& entrypoint,
	        const std::string& target);

	};
	// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
	// geometries are stored in one vertex and index buffer.  It provides the offsets
	// and data needed to draw a subset of geometry stores in the vertex and index 
	// buffers so that we can implement the technique described by Figure 6.3.
	struct SubmeshGeometry {
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		INT BaseVertexLocation = 0;

		// Bounding box of the geometry defined by this submesh. 
		// This is used in later chapters of the book.
		DirectX::BoundingBox Bounds;
	};

	struct MeshGeometry {
		// Give it a name, so we can look it up by name.
		std::string Name;

		// System memory copies.  Use Blobs because the vertex/index format can be generic.
		// It is up to the client to cast appropriately.  
		MS::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		MS::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;
		
		MS::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
		MS::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
		
		MS::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
		MS::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

		// Data about the buffers.
		UINT VertexByteStride = 0;
		UINT VertexBufferByteSize = 0;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R32_UINT;
		UINT IndexBufferByteSize = 0;

		// A MeshGeometry may store multiple geometries in one vertex/index buffer.
		// Use this container to define the Submesh geometries, so we can draw
		// the Submeshes individually.
		std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const {
			D3D12_VERTEX_BUFFER_VIEW vbv;
			vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
			vbv.StrideInBytes = VertexByteStride;
			vbv.SizeInBytes = VertexBufferByteSize;

			return vbv;
		}

		D3D12_INDEX_BUFFER_VIEW IndexBufferView() const {
			D3D12_INDEX_BUFFER_VIEW ibv;
			ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
			ibv.Format = IndexFormat;
			ibv.SizeInBytes = IndexBufferByteSize;

			return ibv;
		}

		// We can free this memory after we finish upload to the GPU.
		void DisposeUploaders() {
			VertexBufferUploader = nullptr;
			IndexBufferUploader = nullptr;
		}
	};

	struct Material {
		// Unique material name for lookup.
		std::string Name;

		// Index into constant buffer corresponding to this material.
		int MatCBIndex = -1;	// Melyik indexen van a textúrája a heapen? (Ha van)

		// Index into SRV heap for diffuse texture.
		int DiffuseSrvHeapIndex = -1;

		// Dirty flag indicating the material has changed and we need to 
		// update the constant buffer. Because we have a material constant 
		// buffer for each FrameResource, we have to apply the update to each 
		// FrameResource. Thus, when we modify a material we should set
		// NumFramesDirty = gNumFrameResources so that each frame resource 
		// gets the update.
		int NumFramesDirty = 0;

		// Material constant buffer data used for shading.
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();

		// Itt tároljuk, hogy a shadernek mire van szüksége.
		// Nem a GPU handle-t tároljuk itt (mert az változhat a transient heap miatt),
		// hanem az SRV forrását (Persistent Handle).
		Core::DescriptorRange TextureRange;
	};

	struct Light {
		DirectX::XMFLOAT3 Strength; // Light color
		float FalloffStart;     // point/spot light only
		DirectX::XMFLOAT3 Direction;// directional/spot light only
		float FalloffEnd;      // point/spot light only
		DirectX::XMFLOAT3 Position; // point/spot light only
		float SpotPower;      // spot light only
	};

#define MaxLights 16

}
