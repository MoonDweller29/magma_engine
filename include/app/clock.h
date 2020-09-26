#pragma once
#include <chrono>

class Clock
{
    std::chrono::high_resolution_clock::time_point m_start;
public:
    Clock();

    double restart();
    double getTime() const;

    static double seconds(long long microseconds);
};