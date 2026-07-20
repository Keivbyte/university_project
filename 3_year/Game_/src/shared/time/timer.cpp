#include "timer.h"

Timer::Timer()
    : m_frameCount(0), m_timeAccumulator(0.0), m_currentFps(0.0)
{
    m_startTime = clock::now();
    m_lastTime  = m_startTime;
}

double Timer::Tick() {
    const clock::time_point now = clock::now();

    const std::chrono::duration<double> delta = now - m_lastTime;
    m_lastTime = now;

    double dt = delta.count();

    if (dt > 0.1) {
        dt = 0.1;
    }

    // FPS count
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
    const std::chrono::duration<double> elapsed = clock::now() - m_startTime;
    return elapsed.count();
}

double Timer::FPS() const {
    return m_currentFps;
}