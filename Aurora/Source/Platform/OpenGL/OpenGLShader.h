#pragma once
#include <filesystem>

#include "Aurora/Renderer/Shader.h"
#include "glad/glad.h"

namespace Aurora {
    
    class OpenGLShader : public Shader {
    public:
        OpenGLShader(const std::string& filepath);
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader() override = default;

        void Bind() override;
        void Unbind() override;
        const std::string& GetName() override;
    
    private:
        uint32_t m_RendererID;
        std::string m_Name;
        std::string m_FilePath;

        std::unordered_map<GLuint, std::string> m_SourceCode;
    };
    
}
