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
        static GLenum GetGLID(ShaderType type) {
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
    
    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::filesystem::path& vertexPath,
            const std::filesystem::path& fragmentPath,
            const std::filesystem::path& geometryPath = "");
        virtual ~OpenGLShader() override;
        
        void Bind() override;
        void Unbind() override;
        const std::string& GetName() override;

        void SetInt(const std::string& name, int value);
        void SetIntArray(const std::string& name, int* values, uint32_t count);
        void SetFloat(const std::string& name, float value);
        void SetFloat2(const std::string& name, const glm::vec2& value);
        void SetFloat3(const std::string& name, const glm::vec3& value);
        void SetFloat4(const std::string& name, const glm::vec4& value);
        void SetMat4(const std::string& name, const glm::mat4& value);
        
        // void AttachShader(ShaderType type, const std::filesystem::path& path);
        void AttachShader(ShaderType type, const std::string& src);
        void DetachShader(ShaderType type);

    private:
        static GLuint CompileShader(ShaderType type, const std::string& src);
        
    private:
        uint32_t m_RendererID;
        std::string m_Name;
        
        std::unordered_map<GLenum, std::string> m_ShaderSrc;
        std::unordered_map<std::string, int> m_UniformLocationCache;
    };
    
}
