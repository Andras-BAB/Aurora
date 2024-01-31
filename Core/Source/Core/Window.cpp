#include <aupch.h>
#include <Core/Window.h>

namespace Aurora {

    std::unique_ptr<Window> Aurora::Window::Create(const WindowProps& props) {
        return std::make_unique<Window>(props);
    }

    Window::Window(const WindowProps& props) {
        m_Props = { 
            props.Title, 
            props.Width, 
            props.Height 
        };
    }

    void Window::OnUpdate() {
    }

    uint32_t Window::GetWidth() const {
        return m_Props.Width;
    }

    uint32_t Window::GetHeight() const {
        return m_Props.Width;
    }

    std::string Window::GetTitle() const {
        return m_Props.Title;
    }

    void Window::SetEventCallback(const EventCallbackFunction& callback) {
    }

    void Window::SetVSync(bool enabled) {
    }

    bool Window::IsVSync() const {
        return false;
    }

    void* Window::GetNativeWindow() const {
        return nullptr;
    }

}
