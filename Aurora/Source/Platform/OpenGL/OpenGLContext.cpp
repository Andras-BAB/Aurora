#include "aupch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include "Aurora/Core/Log.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"

namespace Aurora {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle){  }

	void OpenGLContext::Init() {
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
		if (status == 0) {
			AU_CORE_ERROR("Failed to init Glad!");
		}
		AU_CORE_INFO("OpenGL info:");
		AU_CORE_INFO("\tVendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		AU_CORE_INFO("\tRenderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		AU_CORE_INFO("\tVersion: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	}

	void OpenGLContext::Shutdown() {
	}

	void OpenGLContext::SwapBuffers() {
		glfwSwapBuffers(m_WindowHandle);
	}
}
