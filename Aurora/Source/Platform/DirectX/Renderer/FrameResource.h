#pragma once

#include "UploadBuffer.h"
#include "Platform/DirectX/Utils/MathHelper.h"
#include "Platform/DirectX/Utils/MSUtils.h"

#include "Aurora/Math/Math.h"
#include "Aurora/Renderer/Lights.h"

namespace Aurora {

	struct ObjectConstants {
		math::Mat4 World = math::Mat4::Identity();
	};

	struct PassConstants {
		math::Mat4 View = math::Mat4::Identity();
		math::Mat4 InvView = math::Mat4::Identity();
		math::Mat4 Proj = math::Mat4::Identity();
		math::Mat4 InvProj = math::Mat4::Identity();
		math::Mat4 ViewProj = math::Mat4::Identity();
		math::Mat4 InvViewProj = math::Mat4::Identity();
		math::Vec3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float cbPerObjectPad1 = 0.0f;
		math::Vec2 RenderTargetSize = { 0.0f, 0.0f };
		math::Vec2 InvRenderTargetSize = { 0.0f, 0.0f };
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;

		math::Vec4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

		//d3dUtil::Light Lights[MaxLights];
		uint32_t PointLightCount = 0;
		uint32_t PointLightBufferSRV;

		uint32_t SpotLightCount = 0;
		uint32_t SpotLightBufferSRV;

		DirectionalLight DirectionalLights[4];
		uint32_t DirectionalLightCount = 0;
	};

	struct MaterialConstants {
		math::Vec4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		math::Vec3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		// Used in the chapter on texture mapping.
		//math::Mat4 MatTransform = math::Mat4::Identity();
		math::Vec4 UVTransform = { 1.0f, 1.0f, 0.0f, 0.0f };

		uint32_t DiffuseMapIndex = 0xFFFFFFFF;
		math::Vec3 Padding;
	};

	struct Vertex {
		math::Vec3 Position;
		math::Vec3 Normal;
		math::Vec3 Tangent;
		math::Vec2 UV;
	};

	// Stores the resources needed for the CPU to build the command lists
	// for a frame.  
	struct FrameResource {

		FrameResource(ID3D12Device* device);
		FrameResource(const FrameResource& rhs) = delete;
		FrameResource& operator=(const FrameResource& rhs) = delete;
		~FrameResource() = default;

		void Init(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
		
		// We cannot reset the allocator until the GPU is done processing the commands.
		// So each frame needs their own allocator.
		MS::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

		// We cannot update a cbuffer until the GPU is done processing the commands
		// that reference it.  So each frame needs their own cbuffers.
		std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
		std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
		std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

		// We cannot update a dynamic vertex buffer until the GPU is done processing
		// the commands that reference it.  So each frame needs their own.
		//std::unique_ptr<UploadBuffer<Vertex>> DynamicVB = nullptr;

		// Fence value to mark commands up to this fence point.  This lets us
		// check if these frame resources are still in use by the GPU.
		UINT64 Fence = 0;
	};

	struct FrameSync {
		FrameSync(ID3D12Device* device);
		FrameSync(const FrameSync&) = delete;
		FrameSync& operator=(const FrameSync&) = delete;
		~FrameSync() = default;

		MS::ComPtr<ID3D12CommandAllocator> CommandAllocator;
		UINT64 FenceValue = 0;
	};

	struct FrameRenderData {
		FrameRenderData() = default;
		FrameRenderData(const FrameRenderData&) = delete;
		FrameRenderData& operator=(const FrameRenderData&) = delete;
		~FrameRenderData() = default;

		void Init(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
		void Reset();

		void ResizeCBs(ID3D12Device* device, uint32_t objCount, uint32_t matCount);

		// We cannot update a cbuffer until the GPU is done processing the commands
		// that reference it.  So each frame needs their own cbuffers.
		std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
		std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
		std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

		std::vector<MS::ComPtr<ID3D12Resource>> StagingBuffers;

		// We cannot update a dynamic vertex buffer until the GPU is done processing
		// the commands that reference it.  So each frame needs their own.
		//std::unique_ptr<UploadBuffer<Vertex>> DynamicVB = nullptr;
	};

}
