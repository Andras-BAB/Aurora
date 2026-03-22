#include "aupch.h"
#include "Platform/DirectX/Renderer/DirectX12RendererAPI.h"

#include "DirectX12RenderCommand.h"
#include "Aurora/Core/Application.h"
#include "Aurora/Scene/Entity.h"
#include "Aurora/Utils/PlatformUtils.h"

#ifdef max
#undef max
#endif

namespace Aurora {
	DirectX12RendererAPI::DirectX12RendererAPI() {
		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = 0;
		m_Viewport.Height = 0;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
		m_ScissorRect = {
			.left = 0, 
			.top = 0, 
			.right = 0, 
			.bottom = 0
		};
	}

	void DirectX12RendererAPI::Init() {
		m_Context = DirectX12RenderCommand::GetContext();
		m_GlobalMeshBuffer = std::make_unique<GlobalMeshBuffer>(m_Context->GetDevice(), 536'870'912, 268'435'456); // 512MB VB, 256MB IB
		m_FrameData.resize(m_Context->GetFrameResourcesCount());

		if (!m_Context) {
			AU_CORE_ERROR("DirectX12RendererAPI::Init: Failed to acquire DirectX12Context");
		}
		//XMStoreFloat4(&m_ClearColor, DirectX::Colors::LightSteelBlue);

		// create pipeline
		PipelineStateProperties props = {};
		props.backBufferFormat = m_Context->m_SwapChain.GetBackBufferFormat();
		props.depthStencilFormat = m_Context->m_SwapChain.GetDepthStencilFormat();
		props.device = m_Context->GetDevice();
		props.inputLayout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		props.vertexShader = std::make_shared<DirectX12VertexShader>("shaders/color.hlsl", "baseVert");
		props.pixelShader = std::make_shared<DirectX12PixelShader>("shaders/color.hlsl", "basePixel");
		props.isWireframe = false;
		m_PipelineLib.CreatePipeline(props, "basePipeline");

		m_PassCBVRange = m_Context->m_HeapManager->AllocateCBV_SRV_UAV_Persistent(m_Context->m_NumFrameResources);

		UINT objCBByteSize = d3dUtil::utils::CalcConstantBufferByteSize(sizeof(ObjectConstants));
		UINT matCBByteSize = d3dUtil::utils::CalcConstantBufferByteSize(sizeof(MaterialConstants));
		UINT passCBByteSize = d3dUtil::utils::CalcConstantBufferByteSize(sizeof(PassConstants));
		UINT descriptorSize = m_Context->GetHeapManager()->GetCbvSrvUavIncrementSize();

		for (uint32_t i = 0; i < m_Context->GetFrameResourcesCount(); i++) {
			m_FrameData[i] = std::make_unique<FrameRenderData>();
			// TODO: make dynamic pass count
			m_FrameData[i]->Init(m_Context->GetDevice(), 1, m_ObjectCBCapacity, m_MaterialCBCapacity);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = m_FrameData[i]->PassCB->Resource()->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = passCBByteSize;

			D3D12_CPU_DESCRIPTOR_HANDLE handle = m_PassCBVRange.cpuBase.handle;
			handle.ptr += static_cast<SIZE_T>(i) * descriptorSize;

			m_Context->GetDevice()->CreateConstantBufferView(&cbvDesc, handle);
		}

		m_CurrentFrameData = m_FrameData[0].get();

		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = static_cast<float>(m_Context->m_WindowHandle->GetWidth());
		m_Viewport.Height = static_cast<float>(m_Context->m_WindowHandle->GetHeight());
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		m_ScissorRect = {
			.left = static_cast<LONG>(0),
			.top = static_cast<LONG>(0),
			.right = static_cast<LONG>(m_Context->m_WindowHandle->GetWidth()),
			.bottom = static_cast<LONG>(m_Context->m_WindowHandle->GetHeight())
		};

		m_TextureManager = std::make_unique<DirectX12TextureManager>(m_Context);
	}

	void DirectX12RendererAPI::Shutdown() {
		if (m_Context) {
			m_Context->FlushCommandQueue();
		}
		
		m_ProxyCache.clear();
		m_ActiveDrawList.clear();
		m_PendingMeshes.clear();

		m_FrameData.clear();

		m_PipelineLib.Clear(); 

		m_GlobalMeshBuffer.reset();
	}

	void DirectX12RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		m_Viewport.TopLeftX = static_cast<float>(x);
		m_Viewport.TopLeftY = static_cast<float>(y);
		m_Viewport.Width = static_cast<float>(width);
		m_Viewport.Height = static_cast<float>(height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
		//SetViewport();
	}

	void DirectX12RendererAPI::SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		m_ScissorRect = {
			static_cast<LONG>(x),
			static_cast<LONG>(y),
			static_cast<LONG>(width),
			static_cast<LONG>(height)
		};
		//SetScissors();
	}

	void DirectX12RendererAPI::ResizeSwapchainImage(uint32_t width, uint32_t height) {
		m_Context->OnWindowResize();
	}

	void DirectX12RendererAPI::SetClearColor(const math::Vec4& color) {
	}

	void DirectX12RendererAPI::Clear() {
		m_Context->GetCommandList()->ClearRenderTargetView(m_Context->CurrentBackBufferView(),
			XMLoadFloat4(&m_ClearColor).m128_f32, 0, nullptr);
		m_Context->GetCommandList()->ClearDepthStencilView(m_Context->DepthStencilView(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	}

	void DirectX12RendererAPI::DrawIndexed(const std::shared_ptr<d3dUtil::MeshGeometry>& meshGeo) {
		m_Context->GetCommandList()->SetGraphicsRootSignature(m_PipelineLib.Get("basePipeline")->GetRootSignature());
	}

	MeshAllocation DirectX12RendererAPI::CreateMesh(const MeshData& meshData) {
		MeshAllocation alloc = m_GlobalMeshBuffer->AllocateMeshCPU(meshData);

		PendingMesh pending;
		pending.allocation = alloc;
		pending.VertexStride = meshData.VertexStride;

		pending.VertexData.assign(static_cast<uint8_t*>(meshData.VertexData), static_cast<uint8_t*>(meshData.VertexData) + meshData.VertexSize);
		pending.IndexData.assign(static_cast<uint8_t*>(meshData.IndexData), static_cast<uint8_t*>(meshData.IndexData) + meshData.IndexSize);

		m_PendingMeshes.push_back(std::move(pending));
		return alloc;
	}

	void DirectX12RendererAPI::SetLineWidth(float width) {
	}

	void DirectX12RendererAPI::SubmitEntity(Entity entity) {
		uint32_t entityID = (uint32_t)entity;
		MeshComponent& meshComp = entity.GetComponent<MeshComponent>();
		TransformComponent& transformComp = entity.GetComponent<TransformComponent>();

		WorldTransformComponent& worldComp = entity.GetComponent<WorldTransformComponent>();

		if (!meshComp.Mesh) return;

		const std::vector<SubmeshInstance>& instances = meshComp.Mesh->GetMesh()->GetSubmeshInstances();
		const std::vector<SubmeshGeometry>& submeshes = meshComp.Mesh->GetMesh()->GetSubmeshes();

		for (const auto& instance : instances) {
			uint64_t proxyKey = (static_cast<uint64_t>(entityID) << 32) | static_cast<uint64_t>(instance.SubmeshIndex);

			uint32_t gpuMatIndex = m_MaterialManager.GetGPUIndex(meshComp.Mesh->GetMaterial(instance.MaterialIndex), m_Context->GetFrameResourcesCount());

			if (gpuMatIndex >= m_MaterialCBCapacity || m_ProxyCache.size() >= m_ObjectCBCapacity) {
				EnsureCapacity(static_cast<uint32_t>(m_ProxyCache.size()) + 128, gpuMatIndex + 32);
			}

			if (!m_ProxyCache.contains(proxyKey)) {
				DirectX12RenderProxy proxy;
				proxy.ObjCBIndex = m_Context->GetHeapManager()->AllocateObjCBIndex();
				proxy.ObjCBRange = m_Context->GetHeapManager()->AllocateCBV_SRV_UAV_Persistent(m_Context->GetFrameResourcesCount());
				proxy.MatCBRange = m_Context->GetHeapManager()->AllocateCBV_SRV_UAV_Persistent(m_Context->GetFrameResourcesCount());

				proxy.MatCBIndex = gpuMatIndex;

				const auto& submeshGeo = submeshes[instance.SubmeshIndex];
				const auto& alloc = meshComp.Mesh->GetMesh()->GetAllocation();

				uint32_t globalVOffset = alloc.VertexOffsetBytes / alloc.VertexStride;
				uint32_t globalIOffset = alloc.IndexOffsetBytes / sizeof(uint32_t);

				proxy.IndexCount = submeshGeo.IndexCount;
				proxy.StartIndexLocation = globalIOffset + submeshGeo.StartIndexLocation;
				proxy.BaseVertexLocation = static_cast<int32_t>(globalVOffset + submeshGeo.BaseVertexLocation);

				proxy.NumFramesDirty = m_Context->GetFrameResourcesCount();

				CreateProxyCBVs(proxy, proxy.ObjCBIndex, proxy.MatCBIndex);
				m_ProxyCache[proxyKey] = proxy;
			} else if (m_ProxyCache[proxyKey].MatCBIndex != gpuMatIndex) {
				// in case of getting a new material index
				auto& proxy = m_ProxyCache[proxyKey];
				proxy.MatCBIndex = gpuMatIndex;
				CreateProxyCBVs(proxy, proxy.ObjCBIndex, proxy.MatCBIndex);
			}

			auto& proxy = m_ProxyCache[proxyKey];

			DirectX::XMFLOAT4X4 entityWorldF = worldComp.Transform;
			DirectX::XMMATRIX entityWorld = DirectX::XMLoadFloat4x4(&entityWorldF);
			DirectX::XMMATRIX submeshLocal = instance.LocalTransform;
			DirectX::XMMATRIX finalWorld = DirectX::XMMatrixMultiply(submeshLocal, entityWorld);

			DirectX::XMMATRIX oldWorld = DirectX::XMLoadFloat4x4(&proxy.World);

			if (!MathHelper::IsEqual(oldWorld, finalWorld)) {
				DirectX::XMStoreFloat4x4(&proxy.World, finalWorld);
				proxy.NumFramesDirty = m_Context->GetFrameResourcesCount();
			}

			m_ActiveDrawList.push_back(&proxy);
		}
	}

	void DirectX12RendererAPI::SubmitProxy(const RenderProxyData& proxyData) {
		if (!proxyData.Mesh) return;

		const auto& meshAsset = proxyData.Mesh->GetMesh();
		const auto& instance = meshAsset->GetSubmeshInstances()[proxyData.SubmeshIndex];
		const auto& submeshes = meshAsset->GetSubmeshes();
		const auto& submeshGeo = submeshes[instance.SubmeshIndex];

		uint64_t proxyKey = (static_cast<uint64_t>(proxyData.ObjectID) << 32) | static_cast<uint64_t>(instance.SubmeshIndex);
		uint32_t gpuMatIndex = m_MaterialManager.GetGPUIndex(proxyData.Mesh->GetMaterial(instance.MaterialIndex), m_Context->GetFrameResourcesCount());

		if (!m_ProxyCache.contains(proxyKey)) {
			DirectX12RenderProxy proxy;
			proxy.ObjCBIndex = m_Context->GetHeapManager()->AllocateObjCBIndex();

			if (gpuMatIndex >= m_MaterialCBCapacity || proxy.ObjCBIndex >= m_ObjectCBCapacity) {
				EnsureCapacity(std::max(proxy.ObjCBIndex + 128, m_ObjectCBCapacity * 2), gpuMatIndex + 32);
			}

			proxy.ObjCBRange = m_Context->GetHeapManager()->AllocateCBV_SRV_UAV_Persistent(m_Context->GetFrameResourcesCount());
			proxy.MatCBRange = m_Context->GetHeapManager()->AllocateCBV_SRV_UAV_Persistent(m_Context->GetFrameResourcesCount());
			proxy.MatCBIndex = gpuMatIndex;

			const auto& alloc = proxyData.Mesh->GetMesh()->GetAllocation();
			uint32_t globalVOffset = alloc.VertexOffsetBytes / alloc.VertexStride;
			uint32_t globalIOffset = alloc.IndexOffsetBytes / sizeof(uint32_t);

			proxy.IndexCount = submeshGeo.IndexCount;
			proxy.StartIndexLocation = globalIOffset + submeshGeo.StartIndexLocation;
			proxy.BaseVertexLocation = static_cast<int32_t>(globalVOffset + submeshGeo.BaseVertexLocation);
			proxy.NumFramesDirty = m_Context->GetFrameResourcesCount();

			CreateProxyCBVs(proxy, proxy.ObjCBIndex, proxy.MatCBIndex);
			m_ProxyCache[proxyKey] = proxy;
		} else if (m_ProxyCache[proxyKey].MatCBIndex != gpuMatIndex) {
			if (gpuMatIndex >= m_MaterialCBCapacity) {
				EnsureCapacity(m_ObjectCBCapacity, gpuMatIndex + 32);
			}
			auto& proxy = m_ProxyCache[proxyKey];
			proxy.MatCBIndex = gpuMatIndex;
			CreateProxyCBVs(proxy, proxy.ObjCBIndex, proxy.MatCBIndex);
		}

		auto& proxy = m_ProxyCache[proxyKey];

		DirectX::XMMATRIX finalWorld = proxyData.Transform;
		DirectX::XMMATRIX oldWorld = DirectX::XMLoadFloat4x4(&proxy.World);

		if (!MathHelper::IsEqual(oldWorld, finalWorld)) {
			DirectX::XMStoreFloat4x4(&proxy.World, finalWorld);
			proxy.NumFramesDirty = m_Context->GetFrameResourcesCount();
		}

		m_ActiveDrawList.push_back(&proxy);
	}

	void DirectX12RendererAPI::DeleteRenderProxy(uint32_t entityID, uint32_t submeshCount) {
		UINT64 currentFence = m_Context->m_FrameSyncs[m_Context->m_CurrentFrameSyncIndex]->FenceValue;
		DeferTicket ticket{ currentFence };

		for (uint32_t i = 0; i < submeshCount; ++i) {
			uint64_t proxyKey = (static_cast<uint64_t>(entityID) << 32) | static_cast<uint64_t>(i);

			if (m_ProxyCache.contains(proxyKey)) {
				auto& proxy = m_ProxyCache[proxyKey];

				m_Context->GetHeapManager()->FreeObjCBIndex(proxy.ObjCBIndex);
				m_Context->GetHeapManager()->FreeCBV_SRV_UAV_Persistent(proxy.ObjCBRange, ticket);
				m_Context->GetHeapManager()->FreeCBV_SRV_UAV_Persistent(proxy.MatCBRange, ticket);

				m_ProxyCache.erase(proxyKey);
			}
		}
	}

	void DirectX12RendererAPI::RenderActiveList(ID3D12GraphicsCommandList* cmdList) {
		UpdateConstantBuffers();

		UINT frameIndex = m_Context->GetCurrentFrameSyncIndex();
		UINT descriptorSize = m_Context->GetHeapManager()->GetCbvSrvUavIncrementSize();
		auto device = m_Context->GetDevice();

		// setting descriptor heap
		ID3D12DescriptorHeap* heaps[] = { m_Context->GetHeapManager()->GetCurrentFrameSrvUavCbvHeap() };
		cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

		// setting root signature
		auto basePipeline = m_PipelineLib.Get("basePipeline");
		cmdList->SetGraphicsRootSignature(basePipeline->GetRootSignature());
		cmdList->SetPipelineState(basePipeline->GetPipelineState());

		// pass data
		DescriptorRange transientPassRange = m_Context->GetHeapManager()->AllocateCBV_SRV_UAV_Transient(1);
		D3D12_CPU_DESCRIPTOR_HANDLE srcPass = m_PassCBVRange.cpuBase.handle;
		srcPass.ptr += static_cast<SIZE_T>(frameIndex) * descriptorSize;

		device->CopyDescriptorsSimple(1, transientPassRange.cpuBase.handle, srcPass, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		cmdList->SetGraphicsRootDescriptorTable(2, transientPassRange.gpuBase.handle);

		// allocating transient heap for this frame
		UINT objCount = static_cast<UINT>(m_ActiveDrawList.size());
		DescriptorRange transientRange = m_Context->GetHeapManager()->AllocateCBV_SRV_UAV_Transient(objCount * 2);

		D3D12_VERTEX_BUFFER_VIEW vbv = m_GlobalMeshBuffer->GetVertexBufferView();
		D3D12_INDEX_BUFFER_VIEW ibv = m_GlobalMeshBuffer->GetIndexBufferView();
		cmdList->IASetVertexBuffers(0, 1, &vbv);
		cmdList->IASetIndexBuffer(&ibv);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// copy from persistent to transient
		for (UINT i = 0; i < objCount; ++i) {
			auto* proxy = m_ActiveDrawList[i];

			D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = proxy->ObjCBRange.cpuBase.handle;
			srcHandle.ptr += static_cast<SIZE_T>(frameIndex) * descriptorSize;

			D3D12_CPU_DESCRIPTOR_HANDLE srcMat = proxy->MatCBRange.cpuBase.handle;
			srcMat.ptr += static_cast<SIZE_T>(frameIndex) * descriptorSize;

			D3D12_CPU_DESCRIPTOR_HANDLE dstHandle = transientRange.cpuBase.handle;
			dstHandle.ptr += static_cast<SIZE_T>(i * 2) * descriptorSize;

			D3D12_CPU_DESCRIPTOR_HANDLE dstMat = transientRange.cpuBase.handle;
			dstMat.ptr += static_cast<SIZE_T>(i * 2 + 1) * descriptorSize;

			// TODO: use CopyDescriptors instead of copyDescriptorsSimple
			device->CopyDescriptorsSimple(1, dstHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			device->CopyDescriptorsSimple(1, dstMat, srcMat, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			// Binding
			D3D12_GPU_DESCRIPTOR_HANDLE gpuObj = transientRange.gpuBase.handle;
			gpuObj.ptr += static_cast<UINT64>(i * 2) * descriptorSize;

			D3D12_GPU_DESCRIPTOR_HANDLE gpuMat = transientRange.gpuBase.handle;
			gpuMat.ptr += static_cast<UINT64>(i * 2 + 1) * descriptorSize;

			cmdList->SetGraphicsRootDescriptorTable(0, gpuObj); // Slot 0: Object
			cmdList->SetGraphicsRootDescriptorTable(1, gpuMat); // Slot 1: Material

			cmdList->DrawIndexedInstanced(
				proxy->IndexCount,
				1,
				proxy->StartIndexLocation,
				proxy->BaseVertexLocation,
				0
			);
		}
	}

	void DirectX12RendererAPI::BeginFrame(const SceneData& sceneData) {
		m_ActiveDrawList.clear();

		m_CurrentFrameData = m_FrameData[m_Context->GetCurrentFrameSyncIndex()].get();
		m_CurrentFrameData->Reset();

		EnsureCapacity(static_cast<uint32_t>(m_ProxyCache.size()) + 128, m_MaterialManager.GetAllocatedCount() + 32);

		PassConstants passConstants;
		using namespace DirectX;
		XMMATRIX view = sceneData.ViewMatrix.m;
		XMMATRIX proj = sceneData.ProjectionMatrix.m;

		auto t1 = XMMatrixDeterminant(view);
		auto t2 = XMMatrixDeterminant(proj);
		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invView = XMMatrixInverse(&t1, view);
		XMMATRIX invProj = XMMatrixInverse(&t2, proj);
		auto t3 = XMMatrixDeterminant(viewProj);
		XMMATRIX invViewProj = XMMatrixInverse(&t3, viewProj);

		XMStoreFloat4x4(&passConstants.View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&passConstants.InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&passConstants.Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&passConstants.InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&passConstants.ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&passConstants.InvViewProj, XMMatrixTranspose(invViewProj));

		XMStoreFloat3(&passConstants.EyePosW, sceneData.EyePosition.v);
		passConstants.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };

		XMVECTOR lightDir = -MathHelper::SphericalToCartesian(1.0f, m_SunTheta, m_SunPhi);
		XMStoreFloat3(&passConstants.Lights[0].Direction, lightDir);

		passConstants.Lights[0].Strength = { 1.0f, 1.0f, 0.9f };
		for (int i = 1; i < 16; ++i) passConstants.Lights[i].Strength = { 0.0f, 0.0f, 0.0f };

		passConstants.TotalTime = static_cast<float>(Time::GetTime());
		passConstants.RenderTargetSize = { m_Viewport.Width, m_Viewport.Height };

		m_CurrentFrameData->PassCB->CopyData(0, passConstants);

		if (!m_PendingMeshes.empty()) {
			CommitMeshes(m_Context->GetCommandList());
		}

		SetViewport();
		SetScissors();
	}

	void DirectX12RendererAPI::EndFrame() {
		RenderActiveList(m_Context->GetCommandList());
	}

	void DirectX12RendererAPI::SetContext(IGraphicsContext* context) {
		m_Context = dynamic_cast<DirectX12Context*>(context);
	}

	IGraphicsContext* DirectX12RendererAPI::GetContext() const {
		assert(m_Context && "Graphics context is null!");
		return m_Context;
	}

	void DirectX12RendererAPI::SetViewport() const {
		m_Context->GetCommandList()->RSSetViewports(1, &m_Viewport);
	}

	void DirectX12RendererAPI::SetScissors() const {
		m_Context->GetCommandList()->RSSetScissorRects(1, &m_ScissorRect);
	}

	void DirectX12RendererAPI::UpdateConstantBuffers() {
		UploadBuffer<ObjectConstants>* currentObjectCB = m_CurrentFrameData->ObjectCB.get();
		UploadBuffer<MaterialConstants>* currentMatCB = m_CurrentFrameData->MaterialCB.get();

		for (auto* proxy : m_ActiveDrawList) {
			if (proxy->NumFramesDirty > 0) {
				ObjectConstants objConstants;
				DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&proxy->World);
				DirectX::XMStoreFloat4x4(&objConstants.World, DirectX::XMMatrixTranspose(world));
				currentObjectCB->CopyData(proxy->ObjCBIndex, objConstants);

				//MaterialConstants matConstants;
				//matConstants.DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
				//matConstants.FresnelR0 = { 0.01f, 0.01f, 0.01f };
				//matConstants.Roughness = 0.25f;

				//if (proxy->Material) {
				//	matConstants.DiffuseAlbedo = proxy->Material->DiffuseAlbedo;
				//	matConstants.FresnelR0 = proxy->Material->FresnelR0;
				//	matConstants.Roughness = proxy->Material->Roughness;
				//	DirectX::XMStoreFloat4x4(&matConstants.MatTransform, DirectX::XMMatrixTranspose(
				//		DirectX::XMLoadFloat4x4(&proxy->Material->MatTransform)));
				//}
				//currentMatCB->CopyData(proxy->MatCBIndex, matConstants);

				proxy->NumFramesDirty--;
			}
		}

		m_MaterialManager.UpdateAllDirtyMaterials(currentMatCB);
	}

	void DirectX12RendererAPI::UpdatePassCBV(uint32_t frameIndex) {
		auto device = m_Context->GetDevice();
		UINT descriptorSize = m_Context->GetHeapManager()->GetCbvSrvUavIncrementSize();
		UINT passCBByteSize = d3dUtil::utils::CalcConstantBufferByteSize(sizeof(PassConstants));

		D3D12_GPU_VIRTUAL_ADDRESS passAddress = m_FrameData[frameIndex]->PassCB->Resource()->GetGPUVirtualAddress();

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = passAddress;
		cbvDesc.SizeInBytes = passCBByteSize;

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_PassCBVRange.cpuBase.handle;
		handle.ptr += static_cast<SIZE_T>(frameIndex) * descriptorSize;

		device->CreateConstantBufferView(&cbvDesc, handle);
	}

	void DirectX12RendererAPI::CreateProxyCBVs(DirectX12RenderProxy& proxy, uint32_t objIndex, uint32_t matIndex) {
		auto device = m_Context->GetDevice();
		UINT descriptorSize = m_Context->GetHeapManager()->GetCbvSrvUavIncrementSize();

		UINT objCBByteSize = d3dUtil::utils::CalcConstantBufferByteSize(sizeof(ObjectConstants));
		UINT matCBByteSize = d3dUtil::utils::CalcConstantBufferByteSize(sizeof(MaterialConstants));

		for (uint32_t i = 0; i < m_Context->GetFrameResourcesCount(); i++) {
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_FrameData[i]->ObjectCB->Resource()->GetGPUVirtualAddress();
			cbAddress += static_cast<UINT64>(objIndex) * objCBByteSize;

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objCBByteSize;

			D3D12_CPU_DESCRIPTOR_HANDLE handle = proxy.ObjCBRange.cpuBase.handle;
			handle.ptr += static_cast<SIZE_T>(i) * descriptorSize;

			device->CreateConstantBufferView(&cbvDesc, handle);

			D3D12_GPU_VIRTUAL_ADDRESS matAddress = m_FrameData[i]->MaterialCB->Resource()->GetGPUVirtualAddress();
			matAddress += static_cast<UINT64>(matIndex) * matCBByteSize;

			D3D12_CONSTANT_BUFFER_VIEW_DESC matCbvDesc = {};
			matCbvDesc.BufferLocation = matAddress;
			matCbvDesc.SizeInBytes = matCBByteSize;

			D3D12_CPU_DESCRIPTOR_HANDLE matHandle = proxy.MatCBRange.cpuBase.handle;
			matHandle.ptr += static_cast<SIZE_T>(i) * descriptorSize;

			device->CreateConstantBufferView(&matCbvDesc, matHandle);
		}
	}

	void DirectX12RendererAPI::CommitMeshes(ID3D12GraphicsCommandList* cmdList) {
		if (m_PendingMeshes.empty()) return;

		D3D12_RESOURCE_BARRIER preBarriers[2] = {};

		// Vertex Buffer transition
		preBarriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		preBarriers[0].Transition.pResource = m_GlobalMeshBuffer->GetVertexResource();
		preBarriers[0].Transition.StateBefore = m_VbState;
		preBarriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		preBarriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		// Index Buffer transition
		preBarriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		preBarriers[1].Transition.pResource = m_GlobalMeshBuffer->GetIndexResource();
		preBarriers[1].Transition.StateBefore = m_IbState;
		preBarriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		preBarriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		cmdList->ResourceBarrier(2, preBarriers);

		for (const PendingMesh& mesh : m_PendingMeshes) {
			MeshData uploadData;
			uploadData.VertexData = (void*)mesh.VertexData.data();
			uploadData.VertexSize = (uint32_t)mesh.VertexData.size();
			uploadData.VertexStride = mesh.VertexStride;
			uploadData.IndexData = (void*)mesh.IndexData.data();
			uploadData.IndexSize = (uint32_t)mesh.IndexData.size();

			m_GlobalMeshBuffer->UploadMeshToGPU(cmdList, m_CurrentFrameData->StagingBuffers, uploadData, mesh.allocation);
		}

		D3D12_RESOURCE_BARRIER postBarriers[2] = {};

		postBarriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		postBarriers[0].Transition.pResource = m_GlobalMeshBuffer->GetVertexResource();
		postBarriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		postBarriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		postBarriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		postBarriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		postBarriers[1].Transition.pResource = m_GlobalMeshBuffer->GetIndexResource();
		postBarriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		postBarriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
		postBarriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		cmdList->ResourceBarrier(2, postBarriers);

		m_VbState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		m_IbState = D3D12_RESOURCE_STATE_INDEX_BUFFER;

		m_PendingMeshes.clear();
	}

	void DirectX12RendererAPI::EnsureCapacity(uint32_t neededObjCount, uint32_t neededMatCount) {
		bool resizeNeeded = false;
		uint32_t newObjCapacity = m_ObjectCBCapacity;
		uint32_t newMatCapacity = m_MaterialCBCapacity;

		if (neededObjCount > m_ObjectCBCapacity) {
			newObjCapacity = std::max(neededObjCount, m_ObjectCBCapacity * 2);
			resizeNeeded = true;
		}

		if (neededMatCount > m_MaterialCBCapacity) {
			newMatCapacity = std::max(neededMatCount, m_MaterialCBCapacity * 2);
			resizeNeeded = true;
		}

		if (resizeNeeded) {
			m_Context->FlushCommandQueue();

			m_ObjectCBCapacity = newObjCapacity;
			m_MaterialCBCapacity = newMatCapacity;

			for (uint32_t i = 0; i < m_Context->GetFrameResourcesCount(); i++) {
				m_FrameData[i]->ResizeCBs(m_Context->GetDevice(), m_ObjectCBCapacity, m_MaterialCBCapacity);
				UpdatePassCBV(i);
			}

			for (auto& proxy : m_ProxyCache | std::views::values) {
				CreateProxyCBVs(proxy, proxy.ObjCBIndex, proxy.MatCBIndex);
				proxy.NumFramesDirty = m_Context->GetFrameResourcesCount();
			}

			m_MaterialManager.ForceRefreshAll(m_Context->GetFrameResourcesCount());

			AU_CORE_INFO("CB Capacity resized: Obj: {0}, Mat: {1}", m_ObjectCBCapacity, m_MaterialCBCapacity);
		}
	}
}
