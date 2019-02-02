#pragma once

#include <NTPClient.h>

class Clock
{
public:
    Clock(UDP& ntpUdp, int timezone);

    typedef struct MyTime {
        uint8_t hour, minute, second;

        MyTime()
        : hour(0)
        , minute(0)
        , second(0)
        {}

        MyTime(uint8_t hour_, uint8_t minute_, uint8_t second_)
        : hour(hour_)
        , minute(minute_)
        , second(second_)
        {}
    };

    void begin();

    void tick();

    const MyTime& getTime() const;

    bool update();

private:
    NTPClient m_timeClient;

    MyTime m_time;

    int m_timeZone;
};