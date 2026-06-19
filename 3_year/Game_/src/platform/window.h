#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::wstring& title);
    ~Window();
    // Prohibition of copying
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool ShouldClose() const;
    HWND GetHandle() const;
    void PumpMessages();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd_ = nullptr;
    bool m_shouldClose_ = false;
};
