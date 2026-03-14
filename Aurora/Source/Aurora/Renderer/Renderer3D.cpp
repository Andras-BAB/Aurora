#include "aupch.h"
#include "Renderer3D.h"

#include "RenderCommand.h"
#include "Shader.h"
#include "Texture.h"
#include "IUniformBuffer.h"

namespace Aurora {

	struct Vertex {
		math::Vec3 Position;
		math::Vec4 Color;
		math::Vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
		
		// Editor-only
		int EntityID;
	};

	struct TextVertex {
		math::Vec3 Position;
		math::Vec4 Color;
		math::Vec2 TexCoord;

		// TODO: bg color for outline/bg

		// Editor-only
		int EntityID;
	};
	
	struct Renderer3DData {
		static constexpr uint32_t MaxTriangles = 20000;
		static constexpr uint32_t MaxVertices = MaxTriangles * 3;
		static constexpr uint32_t MaxIndices = MaxTriangles * 3;
		static constexpr uint32_t MaxTextureSlots = 32;
		
		std::shared_ptr<VertexBuffer> TextVertexBuffer;
		std::shared_ptr<Shader> TextShader;

		uint32_t TriangleIndexCount = 0;
		Vertex* TriangleVertexBufferBase = nullptr;
		Vertex* TriangleVertexBufferPtr = nullptr;
		
		uint32_t TextIndexCount = 0;
		TextVertex* TextVertexBufferBase = nullptr;
		TextVertex* TextVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;

		std::array<std::shared_ptr<ITexture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture
		
		std::shared_ptr<ITexture2D> FontAtlasTexture;

		Renderer3D::Statistics Stats;

		struct CameraData {
			math::Mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		std::shared_ptr<IUniformBuffer> CameraUniformBuffer;
	};

	static Renderer3DData s_Data;
	
	void Renderer3D::Init() {
	}

	void Renderer3D::Shutdown() {
	}

	void Renderer3D::SubmitEntity(Entity entity) {
		RenderCommand::SubmitForDraw(entity);
	}

	void Renderer3D::RemoveEntity(Entity entity) {
		//RenderCommand::DeleteRenderProxy(entity);
	}

	void Renderer3D::SubmitProxy(const RenderProxyData& proxyData) {
		RenderCommand::SubmitProxy(proxyData);
	}

	void Renderer3D::BeginScene() {
	}

	void Renderer3D::EndScene() {
		Flush();
	}

	void Renderer3D::Flush() {
	}

	void Renderer3D::ResetStats() {
	}

	Renderer3D::Statistics Renderer3D::GetStats() {
		return s_Data.Stats;
	}

	void Renderer3D::StartBatch() {
	}

	void Renderer3D::NextBatch() {
		Flush();
		StartBatch();
	}
}
