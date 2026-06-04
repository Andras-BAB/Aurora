#pragma once

#include "Lights.h"
#include "Buffer.h"
#include "IRenderCommandList.h"

#include "Platform/DirectX/Renderer/UploadBuffer.h"

namespace Aurora {
	class Scene;

	struct FrameLightData {
		std::shared_ptr<StructuredBuffer> GPUBuffer;
		std::shared_ptr<UploadBuffer<PointLight>> UploadBuffer;
		bool IsDirty = true;
	};

	class LightManager {
	public:
		LightManager();
		virtual ~LightManager() = default;

		void AddPointLight(const PointLight& pointLight);
		void AddSpotLight(const SpotLight& spotLight);
		void AddDirectionalLight(const DirectionalLight& directionalLight);

		void GatherLights(Scene* scene);
		void UploadLightsToGPU(IRenderCommandList* cmdList);

		uint32_t GetPointLightCount() const { return m_PointLightCount; }

		std::shared_ptr<StructuredBuffer> GetCurrentPointLightBuffer() const;

		void MarkDirty();

	private:
		uint32_t m_MaxPointLights = 4096;
		uint32_t m_MaxSpotLights = 1024;
		uint32_t m_MaxDirectionalLights = 4;

		uint32_t m_PointLightCount = 0;
		uint32_t m_SpotLightCount = 0;
		uint32_t m_DirectionalLightCount = 0;

		bool m_CacheDirty = true;
		std::vector<PointLight> m_PointLightCache;
		std::vector<FrameLightData> m_FrameLights;

		//std::shared_ptr<StructuredBuffer> m_PointLightBuffer;
		//std::shared_ptr<StructuredBuffer> m_SpotLightBuffer;

		//std::unique_ptr<UploadBuffer<PointLight>> m_PointLightUpload;
		//std::unique_ptr<UploadBuffer<SpotLight>> m_SpotLightUpload;
	};
}
