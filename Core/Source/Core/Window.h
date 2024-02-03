#pragma once

#include <Events/Event.h>
#include <optional>

namespace Aurora {

	//class Graphics;

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

		void SetTitle(const std::string& title);

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFunction& callback);
		virtual void SetVSync(bool enabled);
		virtual bool IsVSync() const;

		virtual void* GetNativeWindow() const;

		//Graphics& GetGraphics();

		static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
		static std::optional<int> ProcessMessages();

	public:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void EnableCursor();
		void DisableCursor();
		bool IsCursorEnabled() const;

	private:

		void HideCursor();
		void ShowCursor();
		void EnableImGuiMouse();
		void DisableImGuiMouse();
		void ConfineCursor();
		void FreeCursor();

		static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:

		WindowProps m_Props;

		bool m_CursorEnabled = true;
		int m_Width;
		int m_Height;
		HWND m_Hwnd;
		//std::unique_ptr<Graphics> m_Graphics;
		std::vector<BYTE> m_RawBuffer;


	private:

		class WindowClass {
		public:

			static const LPCWSTR GetName();
			static HINSTANCE GetInstance();

		private:

			WindowClass();
			~WindowClass();

			WindowClass(const WindowClass&) = delete;
			WindowClass operator=(const WindowClass&) = delete;
			static constexpr const LPCWSTR m_WndClassName = L"Aurora";
			static WindowClass m_WndClass;
			HINSTANCE m_HInstance;

		};

	};
}