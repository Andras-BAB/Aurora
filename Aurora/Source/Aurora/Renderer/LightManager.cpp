#include "aupch.h"

#include "LightManager.h"
#include "Aurora/Scene/Scene.h"

#include "Platform/DirectX/Renderer/DirectX12RenderCommand.h"

namespace Aurora {
	LightManager::LightManager() {
		m_PointLightCache.reserve(m_MaxPointLights);

		uint32_t framesInFlight = DirectX12RenderCommand::GetContext()->GetFrameResourcesCount();
		auto device = DirectX12RenderCommand::GetContext()->GetDevice();
		m_FrameLights.resize(framesInFlight);

		for (uint32_t i = 0; i < framesInFlight; ++i) {
			m_FrameLights[i].GPUBuffer = StructuredBuffer::Create(m_MaxPointLights, sizeof(PointLight));
			m_FrameLights[i].UploadBuffer = std::make_unique<UploadBuffer<PointLight>>(device, m_MaxPointLights, false);
			m_FrameLights[i].IsDirty = true;
		}
	}

	void LightManager::AddPointLight(const PointLight& pointLight) {
	}

	void LightManager::AddSpotLight(const SpotLight& spotLight) {
	}

	void LightManager::AddDirectionalLight(const DirectionalLight& directionalLight) {
	}

	void LightManager::GatherLights(Scene* scene) {
		if (!m_CacheDirty) return;

		m_PointLightCount = 0;
		m_PointLightCache.clear();

		auto pointLightView = scene->GetAllEntitiesWith<PointLightComponent, WorldTransformComponent>();

		for (auto entity : pointLightView) {
			if (m_PointLightCount >= m_MaxPointLights) {
				AU_CORE_WARN("Maximum point light count reached!");
				break;
			}

			auto [lightComp, transform] = pointLightView.get<PointLightComponent, WorldTransformComponent>(entity);

			PointLight pl;
			pl.PositionW = transform.Transform.GetTranslation();
			pl.Radius = lightComp.Radius;
			pl.Color = lightComp.Color;
			pl.Intensity = lightComp.Intensity;

			//if (m_PointLightCache.size() <= m_PointLightCount) {
			//} else {
			//	m_PointLightCache[m_PointLightCount] = pl;
			//}

			m_PointLightCache.push_back(pl);
			m_PointLightCount++;
		}

		m_CacheDirty = false;

		// TODO: same for the spotlights
	}

	void LightManager::UploadLightsToGPU(IRenderCommandList* cmdList) {
		uint32_t frameIndex = DirectX12RenderCommand::GetContext()->GetCurrentFrameSyncIndex();
		auto& currentFrame = m_FrameLights[frameIndex];

		if (!currentFrame.IsDirty || m_PointLightCount == 0) return;

		for (uint32_t i = 0; i < m_PointLightCount; ++i) {
			currentFrame.UploadBuffer->CopyData(i, m_PointLightCache[i]);
		}

		uint32_t oldState = currentFrame.GPUBuffer->GetCurrentState();
		if (oldState != D3D12_RESOURCE_STATE_COPY_DEST) {
			cmdList->ResourceBarrierRaw(currentFrame.GPUBuffer->GetRawResource(), oldState, D3D12_RESOURCE_STATE_COPY_DEST);
		}

		uint64_t bytesToCopy = m_PointLightCount * sizeof(PointLight);
		cmdList->CopyBufferRegionRaw(
			currentFrame.GPUBuffer->GetRawResource(), 0,
			currentFrame.UploadBuffer->Resource(), 0,
			bytesToCopy
		);

		uint32_t readState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		cmdList->ResourceBarrierRaw(currentFrame.GPUBuffer->GetRawResource(), D3D12_RESOURCE_STATE_COPY_DEST, readState);
		currentFrame.GPUBuffer->SetCurrentState(readState);

		currentFrame.IsDirty = false;
	}

	std::shared_ptr<StructuredBuffer> LightManager::GetCurrentPointLightBuffer() const {
		uint32_t frameIndex = DirectX12RenderCommand::GetContext()->GetCurrentFrameSyncIndex();
		return m_FrameLights[frameIndex].GPUBuffer;
	}

	void LightManager::MarkDirty() {
		m_CacheDirty = true;
		for (auto& frameData : m_FrameLights) {
			frameData.IsDirty = true;
		}
	}
}
