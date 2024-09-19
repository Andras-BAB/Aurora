#pragma once
#include <filesystem>

#include "Aurora/Core/Assert.h"
#include "Aurora/Renderer/Shader.h"
#include "glad/glad.h"

namespace Aurora {

    enum class ShaderType : uint8_t {
        Vertex,
        Fragment,
        Tess_Cont,
        Tess_Eval,
        Geometry,
        Compute
    };
    namespace ShaderUtils {
        static GLuint GetGLID(ShaderType type) {
            switch (type) {
                case ShaderType::Vertex:
                    return GL_VERTEX_SHADER;
                case ShaderType::Fragment:
                    return GL_FRAGMENT_SHADER;
                case ShaderType::Tess_Cont:
                    return GL_TESS_CONTROL_SHADER;
                case ShaderType::Tess_Eval:
                    return GL_TESS_EVALUATION_SHADER;
                case ShaderType::Geometry:
                    return GL_GEOMETRY_SHADER;
                case ShaderType::Compute:
                    return GL_COMPUTE_SHADER;
            }
            AU_CORE_ASSERT(false, "Unknown shader type!")
            return 0;
        }
    }
    

    class OpenGLVertexShader : public Shader {
    public:
        OpenGLVertexShader(const std::string& filepath);
        OpenGLVertexShader(const std::string& name, const std::string& vertexSrc);
        virtual ~OpenGLVertexShader() override = default;
        void Bind() override;
        void Unbind() override;

        const std::string& GetName() override;

        uint32_t GetRendererID() const;
    private:
        uint32_t m_RendererID;
        std::string m_Name;
        std::string m_FilePath;
        std::unordered_map<std::string, int> m_UniformLocationCache;
    };

    class OpenGLFragmentShader : public Shader {
    public:
        OpenGLFragmentShader(const std::string& filepath);
        OpenGLFragmentShader(const std::string& name, const std::string& fragmentSrc);
        virtual ~OpenGLFragmentShader() override = default;

        void Bind() override;
        void Unbind() override;

        const std::string& GetName() override;
        uint32_t GetRendererID() const;
    private:
        uint32_t m_RendererID;
        std::string m_Name;
        std::string m_FilePath;
        std::unordered_map<std::string, int> m_UniformLocationCache;
    };
    
    class OpenGLShader : public Shader {
    public:
        virtual ~OpenGLShader() override = default;
        
        void Bind() override;
        void Unbind() override;
        const std::string& GetName() override;

        static std::shared_ptr<OpenGLShader> Create(
            const std::shared_ptr<OpenGLVertexShader>& vertexShader,
            const std::shared_ptr<OpenGLFragmentShader>& fragmentShader);

        void SetVertexShader(const std::shared_ptr<OpenGLVertexShader>& vertexShader);
        void SetFragmentShader(const std::shared_ptr<OpenGLFragmentShader>& fragmentShader);

    private:
        OpenGLShader() = default;
        OpenGLShader(const std::shared_ptr<OpenGLVertexShader>& vertexShader,
            const std::shared_ptr<OpenGLFragmentShader>& fragmentShader);
    private:
        uint32_t m_RendererID;
        std::string m_Name;

        std::shared_ptr<OpenGLVertexShader> m_VertexShader;
        std::shared_ptr<OpenGLFragmentShader> m_FragmentShader;
    };
    
}
