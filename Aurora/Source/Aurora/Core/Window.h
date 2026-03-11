#pragma once

#include "Aurora/Events/Event.h"
#include "Aurora/Renderer/GraphicsContext.h"

#include <memory>

namespace Aurora {

	struct WindowProps {
		uint32_t Width;
		uint32_t Height;
		std::string Title;

		WindowProps(const std::string& title = "Aurora", uint32_t width = 1600, uint32_t height = 900)
		: Width(width), Height(height), Title(title)
		{			
		}
	};

	class Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;
		virtual void BeginFrame() = 0;
		virtual void SubmitFrame() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual GraphicsContext* GetGraphicsContext() const = 0;

		static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());

	};

}
