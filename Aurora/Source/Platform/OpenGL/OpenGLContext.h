#pragma once
#include "Aurora/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Aurora {
	class OpenGLContext : public GraphicsContext {
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		~OpenGLContext() override = default;

		void Init() override;
		void Shutdown() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}
