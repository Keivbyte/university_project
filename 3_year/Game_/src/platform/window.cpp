#include "window.h"

Window::~Window() {
    DestroyWindow(m_hwnd_);
    UnregisterClassW(L"MyEngineWindowClass", GetModuleHandle(nullptr));
}

Window::Window(int width, int height, const std::wstring& title) {
    const wchar_t* className = L"MyEngineWindowClass";
    WNDCLASSEXW wc = {};

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = className;
    wc.lpfnWndProc = Window::WndProc;

    RegisterClassExW(&wc);

    RECT rect = { 0, 0, width, height };
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

    m_hwnd_ = CreateWindowExW( 0, className, title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, wc.hInstance, this );

    ShowWindow(m_hwnd_, SW_SHOW);
}

bool Window::ShouldClose() const { return m_shouldClose_; }

HWND Window::GetHandle() const { return m_hwnd_; }

void Window::PumpMessages() {
    MSG msg = {};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Window* window = nullptr;

    if (msg == WM_NCCREATE) {
        CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Window*>(create->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (window) {
        switch (msg) {
            case WM_CLOSE:
                window->m_shouldClose_ = true;
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}