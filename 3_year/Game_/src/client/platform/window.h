#pragma once

#include <windows.h>
#include <string>

class Input;

class Window {
public:
    /*
     * @func  Window
     * @desc  Constructor. Registers the window class (if needed) and creates the OS window.
     * @param width: client area width in pixels
     * @param height: client area height in pixels
     * @param title: string to display on the window title bar
     */
    Window(int width, int height, const std::wstring& title);

    /*
     * @func  ~Window
     * @desc  Destructor. Destroys the OS window handle.
     */
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    /*
     * @func  ShouldClose
     * @desc  Checks if a close request (WM_CLOSE) has been sent to the window.
     * @return True if the window should be closed, false otherwise.
     */
    bool ShouldClose() const noexcept;

    /*
     * @func  GetHandle
     * @desc  Retrieves the native Win32 window handle (HWND).
     * @return The HWND associated with this window instance.
     */
    HWND GetHandle() const noexcept;

    /*
     * @func  PumpMessages
     * @desc  Processes all pending OS messages in the thread's message queue.
     *        Should be called once per frame in the main loop.
     */
    void PumpMessages();

    /*
     * @func  SetInput
     * @desc  Attaches an Input instance. While set, WndProc forwards every
     *        message to Input::OnMessage before doing its own handling. Pass
     *        nullptr to detach. The Window does not own the Input.
     * @param input: pointer to the Input state accumulator, or nullptr
     */
    void SetInput(Input* input) noexcept;

    /*
     * @func  GetWidth
     * @desc  Retrieves the current width of the window's client area.
     * @return Width in pixels.
     */
    int GetWidth() const;

    /*
     * @func  GetHeight
     * @desc  Retrieves the current height of the window's client area.
     * @return Height in pixels.
     */
    int GetHeight() const;

private:
    /*
     * @func  WndProc
     * @desc  Static message procedure callback used by the Win32 API to route events.
     *        Forwards messages to the corresponding Window class instance.
     * @param hwnd: handle to the window receiving the message
     * @param msg: the message identifier
     * @param wParam: additional message information
     * @param lParam: additional message information
     * @return The result of the message processing.
     */
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static constexpr const wchar_t* CLASS_NAME = L"MyEngineWindowClass";

    HWND   m_hwnd = nullptr;
    bool   m_shouldClose = false;
    Input* m_input = nullptr;   // not owned; forwarded to in WndProc
};