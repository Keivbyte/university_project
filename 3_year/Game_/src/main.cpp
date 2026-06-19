#include "platform/window.h"
#include <iostream>

int main() {
    try {
        Window window(800, 600, L"My Engine Step 1");

        while (!window.ShouldClose()) {
            window.PumpMessages();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << '\n';
        return -1;
    }

    return 0;
}