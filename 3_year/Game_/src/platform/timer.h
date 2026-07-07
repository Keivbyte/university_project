#pragma once

#include <windows.h>
#include <cstdint>

class Timer {
public:
    Timer();

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    double Tick();

    double Elapsed() const;

    double FPS() const;

private:
    int64_t m_frequency;
    int64_t m_startTime;
    int64_t m_lastTime;

    int m_frameCount;
    double m_timeAccumulator;
    double m_currentFps;
};