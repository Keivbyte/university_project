#include "timer.h"
#include <algorithm> // для std::min

Timer::Timer() 
    : m_frameCount(0), m_timeAccumulator(0.0), m_currentFps(0.0) 
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    m_frequency = freq.QuadPart;

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    m_startTime = now.QuadPart;
    m_lastTime = now.QuadPart;
}

double Timer::Tick() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    int64_t diff = now.QuadPart - m_lastTime;
    m_lastTime = now.QuadPart;

    double dt = static_cast<double>(diff) / static_cast<double>(m_frequency);

    if (dt > 0.1) {
        dt = 0.1;
    }

    // Подсчет FPS (окно 0.5 секунды)
    m_frameCount++;
    m_timeAccumulator += dt;
    
    if (m_timeAccumulator >= 0.5) {
        m_currentFps = static_cast<double>(m_frameCount) / m_timeAccumulator;
        m_frameCount = 0;
        m_timeAccumulator = 0.0;
    }

    return dt;
}

double Timer::Elapsed() const {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    int64_t diff = now.QuadPart - m_startTime;
    return static_cast<double>(diff) / static_cast<double>(m_frequency);
}

double Timer::FPS() const {
    return m_currentFps;
}