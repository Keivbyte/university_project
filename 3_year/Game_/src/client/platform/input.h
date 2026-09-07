#pragma once

#include <windows.h>
#include <array>
#include <cstdint>

/*
 * @enum  KeyCode
 * @desc  Platform-independent key identifiers used by the public Input API.
 *        Raw Win32 virtual-key codes (VK_*) never leak past this enum; OnMessage
 *        translates them through an internal mapping table.
 *
 *        Ranges are laid out contiguously (A..Z, Num0..Num9, F1..F12) so the
 *        translator can use simple offset arithmetic. Left/right variants of the
 *        modifier keys are distinguished.
 *
 *        Unknown must stay first (value 0) and Count must stay last: Count is the
 *        size of every state array, Unknown is the "not mapped" sink slot.
 */
enum class KeyCode : uint16_t {
    Unknown = 0,

    // Letters
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // Digit row
    Num0, Num1, Num2, Num3, Num4,
    Num5, Num6, Num7, Num8, Num9,

    // Arrows
    Left, Right, Up, Down,

    // Editing / navigation
    Space, Enter, Escape, Tab,

    // Modifiers (left / right are separate physical keys)
    LShift, RShift,
    LCtrl, RCtrl,
    LAlt, RAlt,

    // Function row
    F1, F2, F3, F4, F5, F6,
    F7, F8, F9, F10, F11, F12,

    // Mouse buttons (folded into KeyCode so IsDown/WasPressed work uniformly)
    MouseLeft, MouseRight, MouseMiddle,

    Count
};

/*
 * @const KeyCount
 * @desc  Number of addressable slots in the key-state arrays.
 */
inline constexpr std::size_t KeyCount = static_cast<std::size_t>(KeyCode::Count);

/*
 * @class Input
 * @desc  Polling-style input state accumulator for keyboard and mouse.
 *
 *        The engine does NOT subscribe to callbacks. Instead the Window forwards
 *        the relevant Win32 messages into OnMessage(), which keeps a live
 *        snapshot of every tracked button. Game code reads that snapshot during
 *        its own Update(dt) via IsDown / WasPressed / WasReleased.
 *
 *        Edge detection is done with two snapshots:
 *            IsDown(k)      == current[k]
 *            WasPressed(k)  == current[k] && !previous[k]
 *            WasReleased(k) == !current[k] && previous[k]
 *
 *        Required per-frame lifecycle (order is mandatory):
 *            input.BeginFrame();     // current -> previous, clear frame accumulators
 *            window.PumpMessages();  // drains the queue, calls OnMessage()
 *            game.Update(dt);        // polls IsDown / WasPressed / ...
 *
 *        Out of scope by design: action bindings, cursor clipping / hiding,
 *        text input (WM_CHAR / IME), Raw Input, network input buffering, XInput.
 */
class Input {
public:
    Input();

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    /*
     * @func  BeginFrame
     * @desc  Promotes the current key snapshot to previous and clears the
     *        per-frame accumulators (mouse delta, wheel). MUST be called once per
     *        frame BEFORE the message pump, otherwise edge detection is off by a
     *        frame and the mouse delta never resets.
     */
    void BeginFrame();

    /*
     * @func  OnMessage
     * @desc  Feeds one Win32 message into the state machine. Called by
     *        Window::WndProc for every message; unrelated messages are ignored.
     *        Never consumes a message and never calls DefWindowProc - the caller
     *        stays responsible for default processing (e.g. Alt+F4).
     * @param msg: Win32 message id (WM_KEYDOWN, WM_MOUSEMOVE, WM_KILLFOCUS, ...)
     * @param wParam: message-specific payload
     * @param lParam: message-specific payload
     */
    void OnMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    /*
     * @func  IsDown
     * @desc  True while the key/button is held this frame.
     */
    bool IsDown(KeyCode key) const noexcept;

    /*
     * @func  WasPressed
     * @desc  True only on the frame the key transitioned up -> down.
     *        Key auto-repeat (WM_KEYDOWN storms) does NOT re-trigger this.
     */
    bool WasPressed(KeyCode key) const noexcept;

    /*
     * @func  WasReleased
     * @desc  True only on the frame the key transitioned down -> up.
     */
    bool WasReleased(KeyCode key) const noexcept;

    /*
     * @func  MouseX / MouseY
     * @desc  Cursor position in client-area pixels, as of the last WM_MOUSEMOVE.
     */
    int MouseX() const noexcept;
    int MouseY() const noexcept;

    /*
     * @func  MouseDeltaX / MouseDeltaY
     * @desc  Accumulated cursor movement since the last BeginFrame(), in pixels.
     *        Integer for now; Raw Input can later feed the same accumulators
     *        without changing this interface.
     */
    int MouseDeltaX() const noexcept;
    int MouseDeltaY() const noexcept;

    /*
     * @func  MouseWheel
     * @desc  Accumulated vertical wheel scroll since the last BeginFrame(),
     *        normalized so one detent == 1.0f (positive = away from the user).
     */
    float MouseWheel() const noexcept;

private:
    /*
     * @func  SetKey
     * @desc  Writes one entry of the current snapshot, ignoring Unknown / OOB.
     */
    void SetKey(KeyCode key, bool down) noexcept;

    /*
     * @func  ResetAll
     * @desc  Clears both snapshots and the frame accumulators. Used on focus loss
     *        so no key stays logically stuck while the window is in the
     *        background.
     */
    void ResetAll() noexcept;

    /*
     * @func  TranslateKey
     * @desc  Maps a Win32 virtual-key + message lParam to a KeyCode. Resolves the
     *        generic VK_SHIFT / VK_CONTROL / VK_MENU into their left/right variant
     *        using the scan code and the extended-key flag (lParam bit 24).
     * @return KeyCode::Unknown for keys outside the tracked shooter set.
     */
    static KeyCode TranslateKey(WPARAM vk, LPARAM lParam) noexcept;

    std::array<bool, KeyCount> m_current{};
    std::array<bool, KeyCount> m_previous{};

    int   m_mouseX = 0;
    int   m_mouseY = 0;
    int   m_mouseDeltaX = 0;
    int   m_mouseDeltaY = 0;
    float m_mouseWheel = 0.0f;

    bool  m_hasMousePos = false;   // guards the first WM_MOUSEMOVE delta
    int   m_lastMouseX = 0;
    int   m_lastMouseY = 0;
};
