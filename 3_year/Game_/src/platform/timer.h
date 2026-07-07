#pragma once

#include <windows.h>
#include <cstdint>

class Timer {
public:
    /*
     * @func  Timer
     * @desc  Constructor. Initializes the high-resolution performance counter frequency
     *        and sets the initial time markers.
     */
    Timer();

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    /*
     * @func  Tick
     * @desc  Calculates the time elapsed since the last call to Tick. Clamps the delta time
     *        to a maximum of 0.1 seconds to prevent logic spikes. Updates FPS accumulator.
     * @return Delta time (dt) in seconds.
     */
    double Tick();

    /*
     * @func  Elapsed
     * @desc  Calculates the total time elapsed since the timer was created.
     * @return Total elapsed time in seconds.
     */
    double Elapsed() const;

    /*
     * @func  FPS
     * @desc  Returns the current frames per second, averaged over a 0.5-second window.
     * @return Current calculated FPS.
     */
    double FPS() const;

private:
    int64_t m_frequency;
    int64_t m_startTime;
    int64_t m_lastTime;

    int m_frameCount;
    double m_timeAccumulator;
    double m_currentFps;
};