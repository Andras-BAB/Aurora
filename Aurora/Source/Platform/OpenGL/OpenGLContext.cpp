#include "aupch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include "Core/Log.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"

namespace Aurora {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle){  }

	void OpenGLContext::Init() {
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
		if (status == 0) {
			AU_CORE_LOG_ERROR("Failed to init Glad!");
		}
		AU_CORE_LOG_INFO("OpenGL info:");
		AU_CORE_LOG_INFO("\tVendor: {0}", glGetString(GL_VENDOR));
		AU_CORE_LOG_INFO("\tRenderer: {0}", glGetString(GL_RENDERER));
		AU_CORE_LOG_INFO("\tVersion: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::Shutdown() {
	}

	void OpenGLContext::SwapBuffers() {
		glfwSwapBuffers(m_WindowHandle);
	}
}
