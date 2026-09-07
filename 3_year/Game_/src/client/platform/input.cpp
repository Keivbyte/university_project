#include "input.h"

#include <windowsx.h>   // GET_X_LPARAM / GET_Y_LPARAM - multi-monitor safe

namespace {

// lParam bit layout shared by WM_KEY* / WM_SYSKEY* messages.
constexpr LPARAM kScanCodeMask   = 0x00FF0000;   // bits 16..23
constexpr int    kScanCodeShift  = 16;
constexpr LPARAM kExtendedKeyBit = 0x01000000;   // bit 24

// Offset helper: turn a contiguous VK range into the matching KeyCode range.
constexpr KeyCode Offset(KeyCode base, int delta) noexcept {
    return static_cast<KeyCode>(static_cast<int>(base) + delta);
}

} // namespace

Input::Input() {
    m_current.fill(false);
    m_previous.fill(false);
}

void Input::BeginFrame() {
    // Snapshot for this frame's edge detection...
    m_previous = m_current;

    // ...and reset everything that only makes sense "per frame".
    m_mouseDeltaX = 0;
    m_mouseDeltaY = 0;
    m_mouseWheel  = 0.0f;
}

void Input::OnMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        // --- Keyboard -------------------------------------------------------
        // WM_SYSKEYDOWN/UP carry Alt and F10. We only record state here; the
        // caller still forwards them to DefWindowProc so Alt+F4 keeps working.
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            // Bit 30 == 1 means this is an auto-repeat event. current/previous
            // logic is level-based, so repeats are harmless and simply ignored.
            const KeyCode key = TranslateKey(wParam, lParam);
            SetKey(key, true);
            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            const KeyCode key = TranslateKey(wParam, lParam);
            SetKey(key, false);
            break;
        }

        // --- Mouse buttons ------------------------------------------------------
        case WM_LBUTTONDOWN: SetKey(KeyCode::MouseLeft,   true);  break;
        case WM_LBUTTONUP:   SetKey(KeyCode::MouseLeft,   false); break;
        case WM_RBUTTONDOWN: SetKey(KeyCode::MouseRight,  true);  break;
        case WM_RBUTTONUP:   SetKey(KeyCode::MouseRight,  false); break;
        case WM_MBUTTONDOWN: SetKey(KeyCode::MouseMiddle, true);  break;
        case WM_MBUTTONUP:   SetKey(KeyCode::MouseMiddle, false); break;

        // --- Mouse move ------------------------------------------------------
        // lParam here is already in client coordinates. GET_*_LPARAM sign-extend
        // correctly; LOWORD/HIWORD would corrupt negative values on multi-monitor
        // setups.
        case WM_MOUSEMOVE: {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            if (m_hasMousePos) {
                m_mouseDeltaX += x - m_lastMouseX;
                m_mouseDeltaY += y - m_lastMouseY;
            } else {
                m_hasMousePos = true;   // first sample seeds position, no delta
            }

            m_mouseX = x;
            m_mouseY = y;
            m_lastMouseX = x;
            m_lastMouseY = y;
            break;
        }

        // --- Mouse wheel ---------------------------------------------------------
        // Value is a multiple of WHEEL_DELTA (120). Coordinates in this message
        // are SCREEN space, so we deliberately do not touch m_mouseX/Y here.
        case WM_MOUSEWHEEL: {
            const int raw = GET_WHEEL_DELTA_WPARAM(wParam);
            m_mouseWheel += static_cast<float>(raw) / static_cast<float>(WHEEL_DELTA);
            break;
        }

        // --- Focus loss ----------------------------------------------------------
        // Without this, a key held while the window loses focus never gets its
        // WM_KEYUP and stays logically "down" forever.
        case WM_KILLFOCUS: {
            ResetAll();
            break;
        }

        default:
            break;
    }
}

bool Input::IsDown(KeyCode key) const noexcept {
    const auto i = static_cast<std::size_t>(key);
    return i < KeyCount && m_current[i];
}

bool Input::WasPressed(KeyCode key) const noexcept {
    const auto i = static_cast<std::size_t>(key);
    return i < KeyCount && m_current[i] && !m_previous[i];
}

bool Input::WasReleased(KeyCode key) const noexcept {
    const auto i = static_cast<std::size_t>(key);
    return i < KeyCount && !m_current[i] && m_previous[i];
}

int   Input::MouseX() const noexcept { return m_mouseX; }
int   Input::MouseY() const noexcept { return m_mouseY; }
int   Input::MouseDeltaX() const noexcept { return m_mouseDeltaX; }
int   Input::MouseDeltaY() const noexcept { return m_mouseDeltaY; }
float Input::MouseWheel() const noexcept { return m_mouseWheel; }

void Input::SetKey(KeyCode key, bool down) noexcept {
    const auto i = static_cast<std::size_t>(key);
    if (key == KeyCode::Unknown || i >= KeyCount) {
        return;
    }
    m_current[i] = down;
}

void Input::ResetAll() noexcept {
    m_current.fill(false);
    m_previous.fill(false);
    m_mouseDeltaX = 0;
    m_mouseDeltaY = 0;
    m_mouseWheel  = 0.0f;
    // Keep m_mouseX/Y as the last known position; drop the delta anchor so the
    // next move after refocus does not report a huge jump.
    m_hasMousePos = false;
}

KeyCode Input::TranslateKey(WPARAM vk, LPARAM lParam) noexcept {
    // Resolve the generic modifier VKs into a specific physical key.
    if (vk == VK_SHIFT) {
        const UINT scanCode = static_cast<UINT>((lParam & kScanCodeMask) >> kScanCodeShift);
        const UINT resolved = MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX);
        vk = (resolved != 0) ? resolved : static_cast<WPARAM>(VK_LSHIFT);
    } else if (vk == VK_CONTROL) {
        vk = (lParam & kExtendedKeyBit) ? static_cast<WPARAM>(VK_RCONTROL)
                                        : static_cast<WPARAM>(VK_LCONTROL);
    } else if (vk == VK_MENU) {
        vk = (lParam & kExtendedKeyBit) ? static_cast<WPARAM>(VK_RMENU)
                                        : static_cast<WPARAM>(VK_LMENU);
    }

    // Contiguous ranges via offset arithmetic.
    if (vk >= 'A' && vk <= 'Z') {
        return Offset(KeyCode::A, static_cast<int>(vk) - 'A');
    }
    if (vk >= '0' && vk <= '9') {
        return Offset(KeyCode::Num0, static_cast<int>(vk) - '0');
    }
    if (vk >= VK_F1 && vk <= VK_F12) {
        return Offset(KeyCode::F1, static_cast<int>(vk) - VK_F1);
    }

    switch (vk) {
        case VK_LEFT:     return KeyCode::Left;
        case VK_RIGHT:    return KeyCode::Right;
        case VK_UP:       return KeyCode::Up;
        case VK_DOWN:     return KeyCode::Down;

        case VK_SPACE:    return KeyCode::Space;
        case VK_RETURN:   return KeyCode::Enter;
        case VK_ESCAPE:   return KeyCode::Escape;
        case VK_TAB:      return KeyCode::Tab;

        case VK_LSHIFT:   return KeyCode::LShift;
        case VK_RSHIFT:   return KeyCode::RShift;
        case VK_LCONTROL: return KeyCode::LCtrl;
        case VK_RCONTROL: return KeyCode::RCtrl;
        case VK_LMENU:    return KeyCode::LAlt;
        case VK_RMENU:    return KeyCode::RAlt;

        default:          return KeyCode::Unknown;
    }
}
