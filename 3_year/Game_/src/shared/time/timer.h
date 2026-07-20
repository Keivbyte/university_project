#pragma once

#include <cstdint>
#include <chrono>


class Timer {
public:
    /*
     * @func  Timer
     * @desc  Конструктор. Фиксирует стартовую и предыдущую отметки времени.
     */
    Timer();

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    /*
     * @func  Tick
     * @desc  Считает время с прошлого вызова Tick. Клампит dt сверху (0.1 с),
     *        обновляет аккумулятор FPS.
     * @return Delta time (dt) в секундах.
     */
    double Tick();

    /*
     * @func  Elapsed
     * @desc  Полное время с момента создания таймера.
     * @return Секунды.
     */
    double Elapsed() const;

    /*
     * @func  FPS
     * @desc  Текущий FPS, усреднённый по окну 0.5 с.
     * @return Значение FPS.
     */
    double FPS() const;

private:
    using clock = std::chrono::steady_clock;

    clock::time_point m_startTime;
    clock::time_point m_lastTime;

    int    m_frameCount;
    double m_timeAccumulator;
    double m_currentFps;
};