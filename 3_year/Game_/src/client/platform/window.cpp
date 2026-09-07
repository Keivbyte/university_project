#include "window.h"
#include "input.h"
#include <stdexcept>

Window::~Window() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
}

Window::Window(int width, int height, const std::wstring& title) {
    static bool s_classRegistered = false;

    if (!s_classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = CLASS_NAME;
        wc.lpfnWndProc = Window::WndProc;
        wc.hCursor = LoadCursorW(nullptr, (LPCWSTR)IDC_ARROW);

        if (!RegisterClassExW(&wc)) {
            throw std::runtime_error("Failed to register window class!");
        }
        s_classRegistered = true;
    }

    DWORD windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

    RECT rect = { 0, 0, width, height };
    AdjustWindowRectEx(&rect, windowStyle, FALSE, 0);

    m_hwnd = CreateWindowExW(
        0, CLASS_NAME, title.c_str(), windowStyle, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr,
        GetModuleHandleW(nullptr), this
    );

    if (!m_hwnd) {
        throw std::runtime_error("Failed to create window!");
    }

    ShowWindow(m_hwnd, SW_SHOW);
}

bool Window::ShouldClose() const noexcept { return m_shouldClose; }

void Window::SetInput(Input* input) noexcept { m_input = input; }

HWND Window::GetHandle() const noexcept { return m_hwnd; }

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
        // Feed the input accumulator first. OnMessage never consumes a message
        // and never calls DefWindowProc, so WM_SYSKEYDOWN / WM_SYSKEYUP still
        // fall through to DefWindowProcW below and Alt+F4 keeps working.
        if (window->m_input) {
            window->m_input->OnMessage(msg, wParam, lParam);
        }

        if (msg == WM_CLOSE) {
            window->m_shouldClose = true;
            return 0;
        }
        if (msg == WM_ERASEBKGND) {
            return 1;
        }
        if (msg == WM_PAINT) {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int Window::GetWidth() const {
    RECT rect{};
    GetClientRect(m_hwnd, &rect);
    return rect.right - rect.left;
}

int Window::GetHeight() const {
    RECT rect{};
    GetClientRect(m_hwnd, &rect);
    return rect.bottom - rect.top;
}