# Project "Game_" — Brief and Roadmap

## Goal

A fully custom multiplayer FPS game inspired by DOOM and Counter-Strike, written from scratch in C++. All key systems — rendering, physics, audio, networking, saving, and UI — will be built by hand. The main goal is to learn by building a large project. **Additionally, this project is the final thesis for the "Software Systems Development" course.**

## Architecture: Client-Server

The game is designed as a multiplayer game from the very beginning. Roles are physically split across different machines and operating systems:

* **Client** — Windows. Players' laptops. Handles everything the user sees and hears: window, framebuffer, rendering, input, audio, and UI. Players only connect using clients.
* **Server** — Linux on Raspberry Pi 5 (ARM64). Headless: no window, no graphics, no audio, and no input. It handles the authoritative world state, game ticks, and networking. It doesn't play the game — it only hosts.
* **Match Topology**: The Pi runs the server, and laptop clients connect to it over the network. The server is authoritative, and the clients are "thin" (they just show what the server tells them).

## Strict Limitations

* **Platforms**: Client — Windows; Server — Linux (Raspberry Pi 5). Shared code must compile on both.
* **Allowed APIs**:
* Client (Windows): Win32, GDI, WASAPI / WinMM, Winsock.
* Server (Linux): POSIX (including POSIX sockets `sys/socket.h`, `clock_gettime`, pthreads/`std::thread`).
* These are all default system libraries that come with the OS out of the box.


* **Forbidden**: SDL, SFML, OpenGL, DirectX, Vulkan, any external game engines, physics libraries (PhysX, Bullet), audio libraries (FMOD), networking libraries (RakNet, ENet, Asio), JSON parsers, SQLite — absolutely no third-party tools for the main logic.
* **STL**: Fully allowed (containers, threads, filesystem, chrono).
* **Standard**: C++20.
* **Build Tools**: CMake + CLion. Client — MinGW/Windows. Server — GCC/Clang for ARM64 (cross-compiling or building directly on the Pi).

## Principles

* No taking a ready engine and building on top of it. Every pixel, every collision, and every network packet must be written by your own code.
* Claude **does not write code**. Claude only provides explanations, code reviews, concepts, and architecture decisions. One exception (if agreed upon): mechanical porting of already approved code to a new platform (without adding new features).
* One step at a time. For each step, we clearly define: what we are doing, what we are NOT doing, the public interface, what to decide in advance, and the criteria for completion.

## Platform Boundary (Key Architecture Principle)

The code is divided into three zones, separated by the folder structure:

* **shared/** — Compiles for BOTH Windows and Linux/ARM. No direct includes of `<windows.h>`, GDI, Winsock, or POSIX headers. Includes math, network protocol, serialization, world state, shared game logic, and timers. Platform-specific features are only accessed through shared interfaces.
* **client/** — Windows only. Window creation, framebuffer, rendering, input, audio, UI, and client socket implementation (Winsock).
* **server/** — Linux only. The headless server loop and server socket implementation (POSIX).

**Rule**: If a file in `shared/` fails to compile on either platform, the boundary is broken. Fix it immediately.

## Subsystems Map (Overview)

1. **Platform (Client)** — Window, framebuffer, user input.
2. **Time (Shared)** — Monotonic timer, delta time, tick rate.
3. **Math (Shared)** — Vectors, matrices, quaternions, geometry shapes.
4. **Rendering (Client)** — Software rasterizer with a z-buffer.
5. **Assets** — Custom binary formats for maps, models, textures, and sounds.
6. **World (Shared)** — Scene, entities, level representation. Zero pixels — the server runs this without graphics.
7. **Physics (Shared)** — Collisions, player movement, gravity, jumping.
8. **Audio (Client)** — Playback using WASAPI.
9. **UI (Client)** — Menus, HUD, all drawn by the custom renderer.
10. **Saves & Database** — Custom binary format, local storage.
11. **Network (Shared interface + Platform implementations)** — Custom protocol over sockets, state synchronization.
12. **Game Logic (Shared)** — Players, weapons, gameplay rules.

---

## Roadmap

### Phase 1 — Foundation

1. **Win32 Window** — Opens and closes cleanly.
2. **Framebuffer** — Custom pixel array, drawing via GDI (`StretchDIBits`).
3. **Game Loop & Timing** — Delta time, FPS counter.
4. **Input System** — Keyboard and mouse, wrapper for Win32 messages.
5. **Math Library** — Vec2/3/4, Mat4, quaternions, ray, AABB. *(shared)*

### Phase 2 — Rendering (Software Rasterizer, Client)

6. **Triangle Rasterization** — Edge function, scanline fill.
7. **Z-buffer & 3D Pipeline** — Model/view/projection matrices, perspective divide, viewport transform.
8. **Texturing** — Affine first, then perspective-correct.
9. **Lighting** — Phong shading (ambient + diffuse + specular).
10. **FPS Camera** — WASD + mouse look (yaw/pitch).

### Phase 3 — World (Shared)

11. **Custom Map Format** — Simple structure (sectors / convex regions / triangle lists).
12. **Asset Loading** — Custom binary formats for textures, models, and sounds.
13. **Entity System** — The world stores entities; logic is separate from rendering (the server holds the world without graphics).

### Phase 4 — Physics & Gameplay (Shared)

14. **Collisions** — AABB vs AABB, player capsule vs world geometry.
15. **Player Movement** — Gravity, jumping, ground checks, stepping up stairs.
16. **Game Logic** — Weapons, hit-scan / projectiles, health, damage.

### Phase 5 — Audio (Client)

17. **WAV Loader** — Manual RIFF / WAV parsing.
18. **WASAPI Output** — Sending audio to the hardware.
19. **Mixer** — Playing multiple sounds at once, volume control, 3D positioning.

### Phase 6 — UI & Saves (Client)

20. **UI System** — Text (bitmap fonts), buttons, sliders, all drawn by the custom renderer.
21. **Menus** — Main menu + pause menu + settings.
22. **Save / Load** — Custom binary format with version control.
23. **Game Database** — Local storage for player profiles, match history, and settings.

### Phase 7 — Network (Major Architecture Shift)

24. **Socket Basics** — Shared interface `shared/net/socket`, with Winsock (client) and POSIX (server) implementations. TCP and UDP.
25. **Custom Protocol** — Message format, serialization (`shared/serialization`), reliable layer over UDP.
26. **Client-Server** — Authoritative server on the Pi, thin clients on laptops.
27. **State Sync** — Entity snapshots, client-side prediction, lag compensation.
28. **Lobby & Matchmaking** — Minimal implementation.

### Phase 8 — Polish

29. **Optimization** — Profiling, multi-threading (parallel rendering on the client), SIMD where helpful.
30. **Final Integration** — Full flow: menu → connect to server → match → results → menu.

---

## Workflow (How we work)

* One step at a time.
* Before each step, Claude describes the module: responsibilities, non-responsibilities, interface, key decisions, definition of done, and potential pitfalls.
* Kevin writes the code himself. Claude reviews it: what is wrong, why, and how to fix it conceptually. No code generation.
* We iterate until the code is clean, then move to the next step.

## Project Structure

```text
/src
    /client                     # Windows only
        main.cpp                # Client entry point
        /platform
            window.h/.cpp       
            framebuffer.h/.cpp  
            input.h/.cpp        
        /render
            rasterizer.h/.cpp
            camera.h/.cpp
            material.h
            light.h
        /audio
            wav_loader.h/.cpp
            mixer.h/.cpp
        /ui
            ui.h/.cpp
            menu.h/.cpp
        /persistence
            save.h/.cpp
            database.h/.cpp
        /net
            winsock_socket.cpp  # shared/net/socket implementation via Winsock

    /server                     # Linux only (Raspberry Pi 5)
        /app
            server_main.cpp     # Headless server entry point
        /net
            posix_socket.cpp    # shared/net/socket implementation via POSIX

    /shared                     # Compiles on BOTH platforms
        /time
            timer.h/.cpp        # std::chrono::steady_clock
        /math
            vec.h
            mat.h
            ray.h
        /world
            scene.h/.cpp
            entity.h/.cpp
            map.h/.cpp
        /game
            weapons.h/.cpp
            player.h/.cpp
            gameplay.h/.cpp
        /net
            socket.h            # Shared socket interface
            protocol.h/.cpp     # Message formats
        /serialization
            byte_stream.h/.cpp  # Manual byte serialization

/cmake                          # Toolchains (including ARM64 for Pi), helpers
/assets                         # Game assets

```