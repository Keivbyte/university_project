#pragma once

#include <windows.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::wstring& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool ShouldClose() const noexcept;
    HWND GetHandle() const noexcept;
    void PumpMessages();

    int GetWidth() const;
    int GetHeight() const;

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static constexpr const wchar_t* CLASS_NAME = L"MyEngineWindowClass";

    HWND m_hwnd = nullptr;
    bool m_shouldClose = false;
};