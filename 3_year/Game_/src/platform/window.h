#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::wstring& title);
    ~Window();

    // Запрет копирования
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