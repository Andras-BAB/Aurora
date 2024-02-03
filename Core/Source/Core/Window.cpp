#include <aupch.h>
#include <Core/Window.h>

#include <string>
#include <sstream>

#include <codecvt>
#include <locale>
#include <string>

namespace Aurora {

    Window::WindowClass Window::WindowClass::m_WndClass;

    std::unique_ptr<Window> Window::Create(const WindowProps& props) {
        return std::make_unique<Window>(props);
    }

    std::optional<int> Window::ProcessMessages() {
        MSG msg;

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return msg.wParam;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // return empty optional when not quitting app
        return {};
    }

    void Window::EnableCursor() {
    }

    void Window::DisableCursor() {
    }

    bool Window::IsCursorEnabled() const {
        return true;
    }

    void Window::HideCursor() {
    }

    void Window::ShowCursor() {
    }

    void Window::EnableImGuiMouse() {
    }

    void Window::DisableImGuiMouse() {
    }

    void Window::ConfineCursor() {
    }

    void Window::FreeCursor() {
    }

    Window::Window(const WindowProps& props) {
        m_Props = { 
            props.Title, 
            props.Width, 
            props.Height 
        };

        //IMGUI_CHECKVERSION();
        //ImGui::CreateContext();

        //ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        RECT wr{};
        wr.left = 100;
        wr.right = wr.left + m_Props.Width;
        wr.top = 100;
        wr.bottom = wr.top + m_Props.Height;
        if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0) {
            //throw WND_LAST_EXCEPT();
            AuCoreLogCritical("Error occured at AdjustWindowRect! File: {}, line: {}", __FILE__, __LINE__);
        }

        std::wstring nameLPCWSTR(m_Props.Title.c_str(), m_Props.Title.c_str() + strlen(m_Props.Title.c_str()));
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wideName = converter.from_bytes(m_Props.Title.c_str());

        //m_Hwnd = CreateWindow(WindowClass::GetName(), nameLPCWSTR.c_str(),
        m_Hwnd = CreateWindow(WindowClass::GetName(), wideName.c_str(),
            WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
            wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, WindowClass::GetInstance(), this);
        if (m_Hwnd == nullptr) {
            throw "Window cannot be created!";
        }

        ShowWindow(m_Hwnd, SW_SHOWDEFAULT);

        //ImGui::StyleColorsDark();
        //ImGui_ImplWin32_Init(m_Hwnd);

        //m_Graphics = std::make_unique<Graphics>(m_Hwnd, m_Props.Width, m_Props.Height);

        // register mouse raw input device
        RAWINPUTDEVICE rid{};
        rid.usUsagePage = 0x01; // mouse page
        rid.usUsage = 0x02; // mouse usage
        rid.dwFlags = 0;
        rid.hwndTarget = nullptr;
        if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
            //throw WND_LAST_EXCEPT();
        }

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

    void Window::SetTitle(const std::string& title) {

        const int titleLength = MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, nullptr, 0);
        if (titleLength == 0) {
            throw std::runtime_error("Error getting required buffer size");
        }

        std::vector<wchar_t> wideTitle(titleLength);
        MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, wideTitle.data(), titleLength);

        if (SetWindowTextW(m_Hwnd, wideTitle.data()) == 0) {
            throw "Error occured in SetTitle()";
        }

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

    // WINDOW CLASS RELATED

    Window::WindowClass::WindowClass() {
        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(wc);
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = HandleMsgSetup;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetInstance();
        //wc.hIcon = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
        wc.hCursor = nullptr;
        wc.hbrBackground = nullptr;
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = GetName();
        //wc.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));

        if (!RegisterClassEx(&wc)) {
            AuCoreLogCritical("Error occured when creating window!");
            return;
        }
    }

    Window::WindowClass::~WindowClass() {
        UnregisterClass(m_WndClassName, GetInstance());
    }

    const LPCWSTR Window::WindowClass::GetName() {
        return m_WndClassName;
    }

    HINSTANCE Window::WindowClass::GetInstance() {
        return m_WndClass.m_HInstance;
    }

    LRESULT Window::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_NCCREATE) {
            const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
            return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT Window::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
    }

    LRESULT Window::HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        /*if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
            return true;
        }*/

        //const auto& imIO = ImGui::GetIO();

        switch (msg) {
        case WM_CLOSE:
            
            PostQuitMessage(0);
            return 0;
        case WM_KILLFOCUS:
            //keyboard.ClearState();
            break;

        case WM_ACTIVATE:
            // confine/free cursor on window to foreground/background if cursor disabled
            if (!m_CursorEnabled) {
                if (wParam & WA_ACTIVE) {
                    ConfineCursor();
                    HideCursor();
                } else {
                    FreeCursor();
                    ShowCursor();
                }
            }
            break;
            /************ KEYBOARD MESSAGES ************/

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            /*if (imIO.WantCaptureKeyboard) {
                break;
            }
            if (!(lParam & 0x40000000) || keyboard.AutoRepeatIsEnabled()) {
                keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
            }*/
            break;

        case WM_KEYUP:
        case WM_SYSKEYUP:
            /*if (imIO.WantCaptureKeyboard) {
                break;
            }
            keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));*/
            break;

        case WM_CHAR:
            /*if (imIO.WantCaptureKeyboard) {
                break;
            }
            keyboard.OnChar(static_cast<unsigned char>(wParam));*/
            break;

            /************ END KEYBOARD MESSAGES ************/

            /************ MOUSE MESSAGES ************/

        case WM_MOUSEMOVE:
        {
            const POINTS pt = MAKEPOINTS(lParam);

            // cursorless exclusive gets first dibs
            if (!m_CursorEnabled) {
                /*if (!mouse.IsInWindow()) {
                    SetCapture(hwnd);
                    mouse.OnMouseEnter();
                    HideCursor();
                }*/
                break;
            }

            /*if (imIO.WantCaptureMouse) {
                break;
            }*/

            // in client region -> log move, and log enter + capture mouse (if not previously in window)
            if (pt.x >= 0 && pt.x < m_Width && pt.y >= 0 && pt.y < m_Height) {
                /*mouse.OnMouseMove(pt.x, pt.y);
                if (!mouse.IsInWindow()) {
                    SetCapture(hwnd);
                    mouse.OnMouseEnter();
                }*/
            }
            // not in client -> log move / maintain capture if button down
            else {
                if (wParam & (MK_LBUTTON | MK_RBUTTON)) {
                    //mouse.OnMouseMove(pt.x, pt.y);
                }
                // button up -> release capture / log event for leaving
                else {
                    ReleaseCapture();
                    //mouse.OnMouseLeave();
                }
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            if (!m_CursorEnabled) {
                ConfineCursor();
                HideCursor();
            }
            /*if (imIO.WantCaptureMouse) {
                break;
            }*/
            const POINTS points = MAKEPOINTS(lParam);
            //mouse.OnLeftPressed(points.x, points.y);
            SetForegroundWindow(hwnd);
            break;
        }
        case WM_LBUTTONUP:
        {
            /*if (imIO.WantCaptureMouse) {
                break;
            }*/
            const POINTS points = MAKEPOINTS(lParam);
            //mouse.OnLeftReleased(points.x, points.y);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            /*if (imIO.WantCaptureMouse) {
                break;
            }*/
            const POINTS points = MAKEPOINTS(lParam);
            //mouse.OnRightPressed(points.x, points.y);
            break;
        }
        case WM_RBUTTONUP:
        {
            /*if (imIO.WantCaptureMouse) {
                break;
            }*/
            const POINTS points = MAKEPOINTS(lParam);
            //mouse.OnRightReleased(points.x, points.y);
            break;
        }
        case WM_MBUTTONDOWN:
        {
            /*if (imIO.WantCaptureMouse) {
                break;
            }*/
            const POINTS points = MAKEPOINTS(lParam);
            //mouse.OnMiddlePressed(points.x, points.y);
            break;
        }
        case WM_MBUTTONUP:
        {
            /*if (imIO.WantCaptureMouse) {
                break;
            }*/
            const POINTS points = MAKEPOINTS(lParam);
            //mouse.OnMiddleReleased(points.x, points.y);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            /*if (imIO.WantCaptureMouse) {
                break;
            }*/
            const POINTS points = MAKEPOINTS(lParam);
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            //mouse.OnWheelDelta(points.x, points.y, delta);
            break;
        }

        /************ END MOUSE MESSAGES ************/

        /************** RAW MOUSE MESSAGES **************/
        case WM_INPUT:
        {
            /*if (!mouse.RawEnabled()) {
                break;
            }*/
            UINT size = 0;
            // first get the size of the input data
            if (GetRawInputData(
                reinterpret_cast<HRAWINPUT>(lParam),
                RID_INPUT,
                nullptr,
                &size,
                sizeof(RAWINPUTHEADER)) == -1) {
                // bail msg processing if error
                break;
            }
            //rawBuffer.resize(size);
            // read in the input data
            /*if (GetRawInputData(
                reinterpret_cast<HRAWINPUT>(lParam),
                RID_INPUT,
                rawBuffer.data(),
                &size,
                sizeof(RAWINPUTHEADER)) != size) {
                // bail msg processing if error
                break;
            }
            // process the raw input data
            auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
            if (ri.header.dwType == RIM_TYPEMOUSE &&
                (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0)) {
                mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
            }*/
            break;
        }
        /************** END RAW MOUSE MESSAGES **************/

        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

}
