#include "aupch.h"
#include "OpenGLShader.h"

namespace Aurora {
    uint32_t OpenGLVertexShader::GetRendererID() const {
        return m_RendererID;
    }

    uint32_t OpenGLFragmentShader::GetRendererID() const {
        return m_RendererID;
    }

    void OpenGLShader::Bind() {
    }

    void OpenGLShader::Unbind() {
    }

    const std::string& OpenGLShader::GetName() {
        return m_Name;
    }

    std::shared_ptr<OpenGLShader> OpenGLShader::Create(
        const std::shared_ptr<OpenGLVertexShader>& vertexShader,
        const std::shared_ptr<OpenGLFragmentShader>& fragmentShader) {

        return std::make_shared<OpenGLShader>(vertexShader, fragmentShader);
    }

    void OpenGLShader::SetVertexShader(const std::shared_ptr<OpenGLVertexShader>& vertexShader) {
        m_VertexShader = vertexShader;
    }

    void OpenGLShader::SetFragmentShader(const std::shared_ptr<OpenGLFragmentShader>& fragmentShader) {
        m_FragmentShader = fragmentShader;
    }

    OpenGLShader::OpenGLShader(const std::shared_ptr<OpenGLVertexShader>& vertexShader,
                               const std::shared_ptr<OpenGLFragmentShader>& fragmentShader)
            : m_VertexShader(vertexShader), m_FragmentShader(fragmentShader) {

        m_RendererID = glCreateProgram();
        
    }

}
