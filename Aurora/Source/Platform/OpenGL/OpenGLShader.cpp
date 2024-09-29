#include "aupch.h"
#include "OpenGLShader.h"

namespace Aurora {

    namespace ShaderUtils {
        static std::string ReadFileAsString(const std::string& filePath) {
            std::string result;
            std::ifstream in(filePath, std::ios::in | std::ios::binary);
            if (in) {
                in.seekg(0, std::ios::end);
                result.resize((size_t) in.tellg());
                in.seekg(0, std::ios::beg);
                in.read(&result[0], result.size());
                in.close();
                return result;
            }
            AU_CORE_ASSERT(false, "Could not open file")
            return "";
        }
    }

    OpenGLShader::OpenGLShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
        m_RendererID = glCreateProgram();

        std::string vertexSrc = ShaderUtils::ReadFileAsString(vertexPath.string());
        std::string fragmentSrc = ShaderUtils::ReadFileAsString(fragmentPath.string());

        m_ShaderSrc[GL_VERTEX_SHADER] = vertexSrc;
        m_ShaderSrc[GL_FRAGMENT_SHADER] = fragmentSrc;
        
        AttachShader(ShaderType::Vertex, vertexSrc);
        AttachShader(ShaderType::Fragment, fragmentSrc);
        glLinkProgram(m_RendererID);
    }

    OpenGLShader::~OpenGLShader() {
        glDeleteProgram(m_RendererID);
    }

    void OpenGLShader::Bind() {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() {
        glUseProgram(0);
    }

    const std::string& OpenGLShader::GetName() {
        return m_Name;
    }

    void OpenGLShader::SetInt(const std::string& name, int value) {
        if(!m_UniformLocationCache.contains(name)) {
            GLint location = glGetUniformLocation(m_RendererID, name.c_str());
            m_UniformLocationCache[name] = location;
        }
        glUniform1i(m_UniformLocationCache[name], value);
    }

    void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count) {
        if(!m_UniformLocationCache.contains(name)) {
            GLint location = glGetUniformLocation(m_RendererID, name.c_str());
            m_UniformLocationCache[name] = location;
        }
        glUniform1iv(m_UniformLocationCache[name], static_cast<GLsizei>(count), values);
        
    }

    void OpenGLShader::SetFloat(const std::string& name, float value) {
        if(!m_UniformLocationCache.contains(name)) {
            GLint location = glGetUniformLocation(m_RendererID, name.c_str());
            m_UniformLocationCache[name] = location;
        }
        glUniform1f(m_UniformLocationCache[name], value);
    }

    void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value) {
        if(!m_UniformLocationCache.contains(name)) {
            GLint location = glGetUniformLocation(m_RendererID, name.c_str());
            m_UniformLocationCache[name] = location;
        }
        glUniform2f(m_UniformLocationCache[name], value.x, value.y);
    }

    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value) {
        if(!m_UniformLocationCache.contains(name)) {
            GLint location = glGetUniformLocation(m_RendererID, name.c_str());
            m_UniformLocationCache[name] = location;
        }
        glUniform3f(m_UniformLocationCache[name], value.x, value.y, value.z);
    }

    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value) {
        if(!m_UniformLocationCache.contains(name)) {
            GLint location = glGetUniformLocation(m_RendererID, name.c_str());
            m_UniformLocationCache[name] = location;
        }
        glUniform4f(m_UniformLocationCache[name], value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) {
        if(!m_UniformLocationCache.contains(name)) {
            GLint location = glGetUniformLocation(m_RendererID, name.c_str());
            m_UniformLocationCache[name] = location;
        }
        glUniformMatrix4fv(m_UniformLocationCache[name], 1, GL_FALSE, &value[0][0]);
    }

    void OpenGLShader::AttachShader(ShaderType type, const std::filesystem::path& path) {
        glAttachShader(m_RendererID, CompileShader(type, path.string()));

        m_ShaderSrc[ShaderUtils::GetGLID(type)] = ShaderUtils::ReadFileAsString(path.string());
    }

    void OpenGLShader::AttachShader(ShaderType type, const std::string& src) {
        glAttachShader(m_RendererID, CompileShader(type, src));

        m_ShaderSrc[ShaderUtils::GetGLID(type)] = src;
    }

    void OpenGLShader::DetachShader(ShaderType type) {
        glDetachShader(m_RendererID, ShaderUtils::GetGLID(type));

        m_ShaderSrc.erase(ShaderUtils::GetGLID(type));
    }

    GLuint OpenGLShader::CompileShader(ShaderType type, const std::string& src) {
        GLuint ShaderID = glCreateShader(ShaderUtils::GetGLID(type));
        const GLchar* sourceCStr = src.c_str();
        glShaderSource(ShaderID, 1, &sourceCStr, nullptr);
        glCompileShader(ShaderID);

        GLint isCompiled = 0;
        glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(ShaderID, maxLength, &maxLength, infoLog.data());

            glDeleteShader(ShaderID);

            AU_CORE_ASSERT(false, "Failed to compile shader!")
            return 0;
        }
        return ShaderID;
    }
}
