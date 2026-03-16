#ifndef GLOBALTIME_H
#define GLOBALTIME_H

#include "config_helper.h"
#include <NTPClient.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

class GlobalTime {
public:
    static GlobalTime *getInstance();

    void updateTime();
    int getHour();
    int getHour24();
    int getMinute();
    int getSecond();
    bool isPM();
    bool getFormat24Hour();
    bool setFormat24Hour(bool format24hour);

private:
    GlobalTime();
    ~GlobalTime();

    static GlobalTime *m_instance;

    time_t m_unixEpoch;
    int m_hour = 0;
    int m_hour24 = 0;
    int m_minute = 0;
    int m_second = 0;
    int m_timeZoneOffset = -1;
    unsigned long m_nextTimeZoneUpdate = 0;

    WiFiUDP m_udp;
    NTPClient *m_timeClient{nullptr};

    unsigned long m_oneSecond = 1000;
    unsigned long m_updateTimer = 0;

    bool m_format24hour{FORMAT_24_HOUR};

    void getTimeZoneOffsetFromAPI();
};

#endif
