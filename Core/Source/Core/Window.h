#pragma once

#include <Events/Event.h>

namespace Aurora {

	struct WindowProps {
		std::string Title = "Aurora";
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Aurora Engine",
			uint32_t width = 1600,
			uint32_t height = 900)
			: Title(title), Width(width), Height(height) {
		}
	};

	class Window {
	public:

		using EventCallbackFunction = std::function<void(Event&)>;

		Window(const WindowProps& props);
		virtual ~Window() = default;

		virtual void OnUpdate();

		virtual uint32_t GetWidth() const;
		virtual uint32_t GetHeight() const;
		virtual std::string GetTitle() const;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFunction& callback);
		virtual void SetVSync(bool enabled);
		virtual bool IsVSync() const;

		virtual void* GetNativeWindow() const;

		static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());

	private:
		WindowProps m_Props;

	};
}