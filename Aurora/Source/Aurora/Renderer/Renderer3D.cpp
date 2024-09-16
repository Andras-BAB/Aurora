#include "aupch.h"
#include "Renderer3D.h"

#include "RenderCommand.h"
#include "Shader.h"
#include "UniformBuffer.h"

namespace Aurora {

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        float TilingFactor;
		
        // Editor-only
        int EntityID;
    };

    struct TextVertex {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;

        // TODO: bg color for outline/bg

        // Editor-only
        int EntityID;
    };
    
    struct Renderer3DData
    {
        static const uint32_t MaxTriangles = 20000;
        static const uint32_t MaxVertices = MaxTriangles * 3;
        static const uint32_t MaxIndices = MaxTriangles * 3;
        static const uint32_t MaxTextureSlots = 32;

        std::shared_ptr<VertexArray> TriangleVertexArray;
        std::shared_ptr<VertexBuffer> TriangleVertexBuffer;
        std::shared_ptr<Shader> TriangleShader;
        //std::shared_ptr<Texture2D> WhiteTexture;
		
        std::shared_ptr<VertexArray> TextVertexArray;
        std::shared_ptr<VertexBuffer> TextVertexBuffer;
        std::shared_ptr<Shader> TextShader;

        uint32_t TriangleIndexCount = 0;
        QuadVertex* TriangleVertexBufferBase = nullptr;
        QuadVertex* TriangleVertexBufferPtr = nullptr;
        
        uint32_t TextIndexCount = 0;
        TextVertex* TextVertexBufferBase = nullptr;
        TextVertex* TextVertexBufferPtr = nullptr;

        float LineWidth = 2.0f;

        //std::array<std::shared_ptr<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture
		
        //std::shared_ptr<Texture2D> FontAtlasTexture;

        Renderer3D::Statistics Stats;

        struct CameraData {
            glm::mat4 ViewProjection;
        };
        CameraData CameraBuffer;
        std::shared_ptr<UniformBuffer> CameraUniformBuffer;
    };

    static Renderer3DData s_Data;
    
    void Renderer3D::Init() {
    }

    void Renderer3D::Shutdown() {
    }

    void Renderer3D::Draw() {
    }

    void Renderer3D::BeginScene() {
    }

    void Renderer3D::EndScene() {
        Flush();
    }

    void Renderer3D::Flush() {
        //RenderCommand::DrawIndexed(nullptr);
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
