#pragma once
#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Aurora {
	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		virtual ~OpenGLContext() override = default;

		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}
