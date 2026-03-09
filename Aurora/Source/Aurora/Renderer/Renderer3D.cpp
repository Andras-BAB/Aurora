#include "aupch.h"
#include "Renderer3D.h"

#include "RenderCommand.h"
#include "Shader.h"
#include "Texture.h"
#include "UniformBuffer.h"

namespace Aurora {

    struct Vertex {
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
    
    struct Renderer3DData {
        static constexpr uint32_t MaxTriangles = 20000;
        static constexpr uint32_t MaxVertices = MaxTriangles * 3;
        static constexpr uint32_t MaxIndices = MaxTriangles * 3;
        static constexpr uint32_t MaxTextureSlots = 32;

        // std::shared_ptr<VertexArray> TriangleVertexArray;
        std::shared_ptr<VertexBuffer> TriangleVertexBuffer;
        std::shared_ptr<IndexBuffer> TriangleIndexBuffer;
        std::shared_ptr<Shader> TriangleShader;
        std::shared_ptr<Texture2D> WhiteTexture;
		
        // std::shared_ptr<VertexArray> TextVertexArray;
        std::shared_ptr<VertexBuffer> TextVertexBuffer;
        std::shared_ptr<Shader> TextShader;

        uint32_t TriangleIndexCount = 0;
        Vertex* TriangleVertexBufferBase = nullptr;
        Vertex* TriangleVertexBufferPtr = nullptr;
        
        uint32_t TextIndexCount = 0;
        TextVertex* TextVertexBufferBase = nullptr;
        TextVertex* TextVertexBufferPtr = nullptr;

        float LineWidth = 2.0f;

        std::array<std::shared_ptr<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture
		
        std::shared_ptr<Texture2D> FontAtlasTexture;

        Renderer3D::Statistics Stats;

        struct CameraData {
            glm::mat4 ViewProjection;
        };
        CameraData CameraBuffer;
        std::shared_ptr<UniformBuffer> CameraUniformBuffer;
    };

    static Renderer3DData s_Data;
    
    void Renderer3D::Init() {
        // s_Data.TriangleVertexArray = VertexArray::Create();
        s_Data.TriangleVertexBuffer = VertexBuffer::Create(Renderer3DData::MaxVertices * sizeof(Vertex));

        s_Data.TriangleVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "u_Position"     },
            { ShaderDataType::Float4, "u_Color"        },
            { ShaderDataType::Float2, "u_TexCoord"     },
            { ShaderDataType::Float,  "u_TexIndex"     },
            { ShaderDataType::Float,  "u_TilingFactor" },
            { ShaderDataType::Int,    "u_EntityID"     }
        });
        // s_Data.TriangleVertexArray->AddVertexBuffer(s_Data.TriangleVertexBuffer);

        s_Data.TriangleIndexBuffer = IndexBuffer::Create(Renderer3DData::MaxIndices);
        
        s_Data.TriangleVertexBufferBase = new Vertex[Renderer3DData::MaxVertices];
    }

    void Renderer3D::Shutdown() {
        delete[] s_Data.TriangleVertexBufferBase;
    }

    // void Renderer3D::Draw(const std::shared_ptr<VertexArray>& vertexArray) {
    //     RenderCommand::DrawIndexed(vertexArray);
    // }

    void Renderer3D::BeginScene() {
        StartBatch();
    }

    void Renderer3D::EndScene() {
        Flush();
    }

    void Renderer3D::Flush() {
        // RenderCommand::DrawIndexed(s_Data.TriangleVertexArray);
    }

    void Renderer3D::ResetStats() {
    }

    Renderer3D::Statistics Renderer3D::GetStats() {
        return s_Data.Stats;
    }

    void Renderer3D::StartBatch() {
        s_Data.TriangleIndexCount = 0;
        s_Data.TriangleVertexBufferPtr = s_Data.TriangleVertexBufferBase;
        
        s_Data.TextIndexCount = 0;
        s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer3D::NextBatch() {
        Flush();
        StartBatch();
    }
}
