#include "app/clock.h"

Clock::Clock() : m_start()
{
    restart();
}

double Clock::restart()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto before = m_start;
    m_start = now;
    auto dur = now - before;
    auto mcs = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

    return seconds(mcs);
}

double Clock::getTime() const
{
    auto now = std::chrono::high_resolution_clock::now();
    auto dur = now - m_start;
    auto mcs = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();

    return seconds(mcs);
}

double Clock::seconds(long long microseconds)
{
    return static_cast<double>(microseconds) * 1e-6;
}