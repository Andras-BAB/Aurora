#include "aupch.h"
#include "OpenGLShader.h"

namespace Aurora {
    OpenGLShader::OpenGLShader(const std::string& filepath) {
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
    }

    void OpenGLShader::Bind() {
    }

    void OpenGLShader::Unbind() {
    }

    const std::string& OpenGLShader::GetName() {
        return m_Name;
    }
}
