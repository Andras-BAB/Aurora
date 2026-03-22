#pragma once
#include "RenderProxyData.h"

namespace Aurora {
	class Entity;
	
	class Renderer3D {
	public:
		static void Init();
		static void Shutdown();

		static void SubmitEntity(Entity entity);
		static void RemoveEntity(Entity entity);
		static void SubmitProxy(const RenderProxyData& proxyData);
		
		static void BeginScene();
		static void EndScene();
		static void Flush();

		// Stats
		struct Statistics {
			uint32_t DrawCalls = 0;
		};
		static void ResetStats();
		static Statistics GetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};
	
}
