#include "platform/window.h"

int main() {
    Window window(800, 600, L"My Engine Step 1");

    while (!window.ShouldClose()) {
        window.PumpMessages();
    }

    return 0;
}