#include "vtb/app/clock.h"
#include <ctime>
#include <iostream>

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


Time::Time(const std::chrono::milliseconds &dur) {
    auto milliseconds = dur.count();
    _ms = milliseconds%1000;
    auto seconds = milliseconds / 1000;
    _s = seconds%60;
    auto minutes = seconds / 60;
    _min = minutes%60;
    auto hours = minutes / 60;
    _hour = hours % 24;
}

static int absMod(int x, int d){
    x = x < 0 ? (d - (-x)%d)%d : x % d;
    return x;
}

void Time::addHours(int hours)
{
    _hour = absMod(_hour+hours, 24);
}


std::string Time::toStr() const {
    char buf[16];
    sprintf(buf, "%02d:%02d:%02d.%03d", _hour, _min, _s, _ms);

    return std::string(buf);
}

std::ostream &operator<<(std::ostream &stream, const Time &t) {
    return stream << t.toStr();
}

int SystemClock::_zone_ofs = SystemClock::getZoneOffset();

int SystemClock::getZoneOffset() {
    auto now = std::chrono::system_clock::now();

    auto epoch = now.time_since_epoch();
    Time t(std::chrono::duration_cast<std::chrono::milliseconds>(epoch));

    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    // this function use static global pointer. so it is not thread safe solution
    std::tm* time_info = std::localtime(&current_time);

    return time_info->tm_hour - t.hours();
}

Time SystemClock::getTime() {
    auto now = std::chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    Time t(std::chrono::duration_cast<std::chrono::milliseconds>(epoch));
    t.addHours(_zone_ofs);

    return t;
}
