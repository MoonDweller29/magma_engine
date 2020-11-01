#pragma once
#include <chrono>
#include <string>

class Clock
{
    std::chrono::high_resolution_clock::time_point m_start;
public:
    Clock();

    double restart();
    double getTime() const;

    static double seconds(long long microseconds);
};

class Time {
public:
    Time(const std::chrono::milliseconds &dur);
    Time(int hour=0, int min=0, int s=0, int ms=0):
        _hour(hour), _min(min), _s(s), _ms(ms) {}

    int hours()         const  { return _hour; }
    int minutes()       const  { return _min;  }
    int seconds()       const  { return _s;    }
    int milliseconds()  const  { return _ms;   }

    void addHours(int hours);

    std::string toStr() const;
    friend std::ostream &operator<<(std::ostream &stream, const Time &t);
private:
    int _hour;
    int _min;
    int _s;
    int _ms;
};


class SystemClock {
public:
    static Time getTime();

private:
    static int _zone_ofs;

    static int getZoneOffset();
};