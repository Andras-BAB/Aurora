#pragma once

namespace Aurora {
	
	class Renderer3D {
	public:
		static void Init();
		static void Shutdown();

		// static void Draw(const std::shared_ptr<VertexArray>& vertexArray);
		
		static void BeginScene();
		static void EndScene();
		static void Flush();

		// Stats
		struct Statistics {
			uint32_t DrawCalls = 0;
			//uint32_t QuadCount = 0;

			//uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			//uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};
		static void ResetStats();
		static Statistics GetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};
	
}
