#include "platform.h"
#include "setup.h"

#if BACKEND(IMGUI_WIN32)

#include "renderer.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <string>

#include <imgui.h>
#include "imgui_impl_win32.h"

#if defined(_UNICODE)
std::wstring Utf8ToNative(const std::string &str)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), (wchar_t *)result.data(), size);
    return result;
}
#else
std::string Utf8ToNative(const std::string &str)
{
    return str;
}
#endif

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WinProc_g(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct PlatformWin32 final
    : Platform
{
    static PlatformWin32 *s_Instance;

    PlatformWin32(void);

    bool ApplicationStart(void) override;
    void ApplicationStop() override;
    bool OpenMainWindow(const char *title,
                        int is_child_wnd,
                        void *parent_handle,
                        int width,
                        int height) override;
    bool CloseMainWindow() override;
    void *GetMainWindowHandle() const override;
    void SetMainWindowTitle(const char *title) override;
    void ShowMainWindow() override;
    bool ProcessMainWindowEvents() override;
    bool IsMainWindowVisible() const override;
    void SetRenderer(Renderer *renderer) override;
    void NewFrame() override;
    void FinishFrame() override;
    void Quit() override;

    void SetDpiScale(float dpiScale);

    void set_window_size(int32_t x, int32_t y, int32_t width, int32_t height) override;

    LRESULT WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Application &m_Application;
    WNDCLASSEXA m_WindowClass = {};
    HWND m_MainWindowHandle = nullptr;
    bool m_IsMinimized = false;
    bool m_WasMinimized = false;
    bool m_CanCloseResult = false;
    Renderer *m_Renderer = nullptr;
};

std::unique_ptr<Platform> CreatePlatform(void)
{
    return std::make_unique<PlatformWin32>();
}

PlatformWin32 *PlatformWin32::s_Instance = nullptr;

PlatformWin32::PlatformWin32(void)
{
}

bool PlatformWin32::ApplicationStart(void)
{
    if (s_Instance)
        return false;

    s_Instance = this;

    WNDPROC temp_win_proc = &WinProc_g;
    m_WindowClass =
        {
            sizeof(WNDCLASSEXA),
            CS_CLASSDC,
            &WinProc_g,
            0L,
            0L,
            GetModuleHandle(nullptr),
            LoadIcon(GetModuleHandle(nullptr),
                     IDI_APPLICATION),
            LoadCursor(nullptr, IDC_ARROW),
            nullptr,
            nullptr,
            "imgui-node-editor-application",
            LoadIcon(GetModuleHandle(nullptr),
                     IDI_APPLICATION)};

    if (!RegisterClassExA(&m_WindowClass))
    {
        s_Instance = nullptr;
        return false;
    }

    ImGui_ImplWin32_EnableDpiAwareness();

    return true;
}

void PlatformWin32::ApplicationStop()
{
    if (!s_Instance)
        return;

    UnregisterClassA(m_WindowClass.lpszClassName, m_WindowClass.hInstance);

    s_Instance = nullptr;
}

bool PlatformWin32::OpenMainWindow(const char *title,
                                   int is_child_wnd,
                                   void *parent_handle,
                                   int width,
                                   int height)
{
    if (m_MainWindowHandle)
        return false;

    HWND parent_wnd = (HWND)parent_handle;

    int32_t style = WS_OVERLAPPEDWINDOW;
    if (is_child_wnd == true)
    {
        style = WS_CHILD;
    }
    else
    {
        style = WS_OVERLAPPEDWINDOW;
    }

    m_MainWindowHandle = CreateWindowA(
        m_WindowClass.lpszClassName,
        title,
        //Utf8ToNative(title).c_str(),
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width < 0 ? CW_USEDEFAULT : width,
        height < 0 ? CW_USEDEFAULT : height,
        parent_wnd, nullptr, m_WindowClass.hInstance, nullptr);

    if (!m_MainWindowHandle)
        return false;

    if (!ImGui_ImplWin32_Init(m_MainWindowHandle))
    {
        DestroyWindow(m_MainWindowHandle);
        m_MainWindowHandle = nullptr;
        return false;
    }

    //* Set focus to the control
    SetFocus(m_MainWindowHandle);

    SetDpiScale(ImGui_ImplWin32_GetDpiScaleForHwnd(m_MainWindowHandle));

    return true;
}

bool PlatformWin32::CloseMainWindow()
{
    if (m_MainWindowHandle == nullptr)
        return true;

    SendMessage(m_MainWindowHandle, WM_CLOSE, 0, 0);

    return m_CanCloseResult;
}

void *PlatformWin32::GetMainWindowHandle() const
{
    return m_MainWindowHandle;
}

void PlatformWin32::SetMainWindowTitle(const char *title)
{
    SetWindowText(m_MainWindowHandle, Utf8ToNative(title).c_str());
}

void PlatformWin32::set_window_size(int32_t x, int32_t y, int32_t width, int32_t height)
{
    SetWindowPos(m_MainWindowHandle,
                 nullptr,
                 x,
                 y,
                 width,
                 height,
                 SWP_SHOWWINDOW);
}

void PlatformWin32::ShowMainWindow()
{
    if (m_MainWindowHandle == nullptr)
        return;

    //ShowWindow(m_MainWindowHandle, SW_SHOWMAXIMIZED);
    ShowWindow(m_MainWindowHandle, SW_SHOW);
    UpdateWindow(m_MainWindowHandle);
}

bool PlatformWin32::ProcessMainWindowEvents()
{
    if (m_MainWindowHandle == nullptr)
        return false;

    auto fetchMessage = [this](MSG *msg) -> bool {
        if (!m_IsMinimized)
            return PeekMessage(msg, nullptr, 0U, 0U, PM_REMOVE) != 0;
        else
            return GetMessage(msg, nullptr, 0U, 0U) != 0;
    };

    MSG msg = {};
    while (fetchMessage(&msg))
    {
        if (msg.message == WM_KEYDOWN && (msg.wParam == VK_ESCAPE))
            PostQuitMessage(0);

        if (msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

bool PlatformWin32::IsMainWindowVisible() const
{
    if (m_MainWindowHandle == nullptr)
        return false;

    if (m_IsMinimized)
        return false;

    return true;
}

void PlatformWin32::SetRenderer(Renderer *renderer)
{
    m_Renderer = renderer;
}

void PlatformWin32::NewFrame()
{
    ImGui_ImplWin32_NewFrame();

    if (m_WasMinimized)
    {
        ImGui::GetIO().DeltaTime = 0.1e-6f;
        m_WasMinimized = false;
    }
}

void PlatformWin32::FinishFrame()
{
    if (m_Renderer)
        m_Renderer->Present();
}

void PlatformWin32::Quit()
{
    PostQuitMessage(0);
}

void PlatformWin32::SetDpiScale(float dpiScale)
{
    SetWindowScale(dpiScale);
    SetFramebufferScale(dpiScale);
}

LRESULT PlatformWin32::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return 1;

    switch (msg)
    {
        case WM_CLOSE:
            m_CanCloseResult = true; //m_Application.CanClose();
            if (m_CanCloseResult)
            {
                ImGui_ImplWin32_Shutdown();
                DestroyWindow(hWnd);
            }
            return 0;

        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
            {
                m_IsMinimized = true;
                m_WasMinimized = true;
            }
            else if (wParam == SIZE_RESTORED && m_IsMinimized)
            {
                m_IsMinimized = false;
            }

            if (m_Renderer != nullptr && wParam != SIZE_MINIMIZED)
                m_Renderer->Resize(static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam)));
            return 0;

        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;

        case WM_DPICHANGED:
            SetDpiScale(ImGui_ImplWin32_GetDpiScaleForHwnd(hWnd));
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT __stdcall WinProc_g(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return PlatformWin32::s_Instance->WinProc(hWnd, msg, wParam, lParam);
}

#endif // BACKEND(IMGUI_WIN32)