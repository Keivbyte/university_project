# Project "Game_" - Brief and Roadmap

## Objective

A fully custom FPS game in the spirit of DOOM / Counter-Strike, written from scratch in C++ for Windows. All key subsystems—rendering, physics, audio, networking, saving, UI—are implemented by hand. The primary goal is learning through the construction of a large-scale project. **Additionally, this serves as the final university project for the course "Software Systems Development."**

## Hard Constraints

* **OS:** Windows only.
* **Permitted APIs:** Win32, GDI, WASAPI / WinMM, Winsock — everything included with Windows out of the box.
* **Forbidden:** SDL, SFML, OpenGL, DirectX, Vulkan, any external engines, physics libraries (PhysX, Bullet), audio libraries (FMOD), networking libraries (RakNet, ENet), JSON parsers, SQLite — absolutely no third-party libraries for the core logic.
* **STL:** Fully permitted (containers, threads, filesystem).
* **Standard:** C++20.
* **Build System:** CMake + CLion.

## Subsystems (Overview Map)

1. **Platform** - window, framebuffer, input, timing.
2. **Mathematics** - vectors, matrices, quaternions, geometric primitives.
3. **Render** - software rasterizer with a z-buffer.
4. **Assets** - custom binary formats for maps, models, textures, and sounds.
5. **World** - scene, entities, level representation.
6. **Physics** - collisions, player movement, gravity, jumping.
7. **Audio** - sound and music playback via WASAPI.
8. **UI** - menus, HUD, everything drawn by the custom renderer.
9. **Saves & DB** - custom binary format, local storage.
10. **Network** - Winsock, custom protocol, state synchronization.
11. **Game Logic** - players, weapons, gameplay rules.

## Roadmap

### Phase 1 - Foundation

1. **Win32 Window** - opens and closes cleanly.
2. **Framebuffer** - custom pixel array, output via GDI (`StretchDIBits`).
3. **Game Loop + Timing** - `QueryPerformanceCounter`, delta time, FPS counter.
4. **Input System** - keyboard and mouse, wrapper around Win32 messages.
5. **Math Library** - Vec2/3/4, Mat4, quaternions, ray, AABB.

### Phase 2 - Rendering (Software Rasterizer)

6. **Triangle Rasterization** - edge function, scanline fill.
7. **Z-buffer + 3D Pipeline** - model/view/projection, perspective divide, viewport transform.
8. **Texturing** - affine first, then perspective-correct.
9. **Lighting** - Phong (ambient + diffuse + specular).
10. **FPS Camera** - WASD + mouse, yaw/pitch.

### Phase 3 - World

11. **Custom Map Format** - simple structure (sectors / convex regions / triangle list).
12. **Asset Loading** - custom binary formats for textures, models, sounds.
13. **Entity System** - the world stores entities; entities have a fixed set of fields or components.

### Phase 4 - Physics and Gameplay

14. **Collisions** - AABB vs AABB, player capsule vs world geometry.
15. **Player Movement** - gravity, jumping, ground check, step-up on stairs.
16. **Game Logic** - weapons, hit-scan / projectiles, health, damage.

### Phase 5 - Audio

17. **WAV Loader** - manual RIFF / WAV parsing.
18. **WASAPI Output** - sending audio to the device.
19. **Mixer** - multiple simultaneous sounds, volume, 3D positioning.

### Phase 6 - UI and Saves

20. **UI System** - text (bitmap font), buttons, sliders, all via the custom renderer.
21. **Main Menu + Pause + Settings**.
22. **Save / Load** - custom binary format with versioning.
23. **Game DB** - local storage for profiles, match history, settings. Schema and format are custom.

### Phase 7 - Network

24. **Winsock Basics** - TCP and UDP sockets.
25. **Custom Protocol** - message format, serialization, reliable layer over UDP.
26. **Client–Server** - authoritative server, thin client.
27. **State Synchronization** - entity snapshots, client-side prediction, lag compensation.
28. **Lobby and Matchmaking** - minimal implementation.

### Phase 8 - Polish

29. **Optimization** - profiling, multithreading (parallel band rasterization), SIMD where it helps.
30. **Final Integration** - full loop: menu → match → results → menu.

## Workflow

* One stage at a time.
* Before a stage, Claude describes the module: responsibility, anti-responsibility, interface, key decisions, definition of done, and pitfalls. *(Note: Since you're talking to Gemini now, I can gladly take on this role for you!)*
* Kevin writes the code himself. This is a strict rule — the AI **does not write code under any circumstances**.
* The AI reviews what was written: what is wrong, why, and how to fix it conceptually. No alternative code is provided.
* Iterate until a clean state is reached, then move to the next stage.

## Project Structure (At Start)

```text
/src
    main.cpp
    /platform
        window.h/.cpp
        framebuffer.h/.cpp
        input.h/.cpp
        timer.h/.cpp
    /math
        vec.h
        mat.h
        ray.h
    /render
        rasterizer.h/.cpp
        camera.h/.cpp
        material.h
        light.h
    /world
        scene.h/.cpp
        entity.h/.cpp
        map.h/.cpp
    /physics
        collision.h/.cpp
        player_controller.h/.cpp
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
        socket.h/.cpp
        protocol.h/.cpp
        server.h/.cpp
        client.h/.cpp
    /game
        weapons.h/.cpp
        player.h/.cpp
        gameplay.h/.cpp

```