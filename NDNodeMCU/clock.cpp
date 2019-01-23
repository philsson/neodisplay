#include "clock.h"
#include "config.h"

Clock::Clock(UDP& ntpUdp, int timezone)
: m_timeClient(ntpUdp)
, m_time()
, m_timeZone(timezone)
{
    m_timeClient.setTimeOffset(m_timeZone);
}

void Clock::begin()
{
    m_timeClient.begin();
}

void Clock::tick()
{
    m_time.second = (m_time.second + 1) % 60;
    if (m_time.second == 0)
    {
        m_time.minute = (m_time.minute + 1) % 60;

        if (m_time.minute == 0)
        {
            m_time.hour = (m_time.hour + 1) % 24;
        }
    }
}

const Clock::MyTime& Clock::getTime()
{
    return m_time;
}

void Clock::update()
{
    Serial.println("Updating time from NTP");
    for (uint8_t i = 0; i < 10; i++)
    {
        if (m_timeClient.update())
        {
            Serial.println("\nUpdate Successful");
            m_time.hour = m_timeClient.getHours();
            m_time.minute = m_timeClient.getMinutes();
            m_time.second = m_timeClient.getSeconds();
            return;
        }
        else
        {
            delay(1000);
            Serial.print(".");
        }
    }
    Serial.println("\nUpdate Failed");
}