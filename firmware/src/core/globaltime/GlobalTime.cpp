#include "GlobalTime.h"

#include "config_helper.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <TimeLib.h>

GlobalTime *GlobalTime::m_instance = nullptr;

GlobalTime::GlobalTime() {
    m_timeClient = new NTPClient(m_udp);
    m_timeClient->begin();
    m_timeClient->setPoolServerName(NTP_SERVER);
}

GlobalTime::~GlobalTime() {
    delete m_timeClient;
}

GlobalTime *GlobalTime::getInstance() {
    if (m_instance == nullptr) {
        m_instance = new GlobalTime();
    }
    return m_instance;
}

void GlobalTime::updateTime() {
    if (millis() - m_updateTimer > m_oneSecond) {
        if (m_timeZoneOffset == -1 || (m_nextTimeZoneUpdate > 0 && m_unixEpoch > m_nextTimeZoneUpdate)) {
            getTimeZoneOffsetFromAPI();
        }
        m_timeClient->update();
        m_unixEpoch = m_timeClient->getEpochTime();
        m_updateTimer = millis();
        m_minute = minute(m_unixEpoch);
        if (m_format24hour) {
            m_hour = hour(m_unixEpoch);
        } else {
            m_hour = hourFormat12(m_unixEpoch);
        }
        m_hour24 = hour(m_unixEpoch);
        m_second = second(m_unixEpoch);
    }
}

int GlobalTime::getHour() { return m_hour; }
int GlobalTime::getHour24() { return m_hour24; }
int GlobalTime::getMinute() { return m_minute; }
int GlobalTime::getSecond() { return m_second; }

bool GlobalTime::isPM() {
    return hour(m_unixEpoch) >= 12;
}

void GlobalTime::getTimeZoneOffsetFromAPI() {
    HTTPClient http;
    http.begin(String(TIMEZONE_API_URL) + "?key=" + TIMEZONE_API_KEY + "&format=json&fields=gmtOffset,zoneEnd&by=zone&zone=" + String(TIMEZONE_API_LOCATION));
    int httpCode = http.GET();

    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getString());
        if (!error) {
            m_timeZoneOffset = doc["gmtOffset"].as<int>();
            if (doc["zoneEnd"].isNull()) {
                // Timezone does not use DST, no futher updates necessary
                m_nextTimeZoneUpdate = 0;
            } else {
                // Timezone uses DST, update when necessary
                m_nextTimeZoneUpdate = doc["zoneEnd"].as<unsigned long>() + random(5 * 60); // Randomize update by 5 minutes to avoid flooding the API
            }
            Serial.print("Timezone Offset from API: ");
            Serial.println(m_timeZoneOffset);
            Serial.print("Next timezone update: ");
            Serial.println(m_nextTimeZoneUpdate);
            m_timeClient->setTimeOffset(m_timeZoneOffset);
        } else {
            Serial.println("Deserialization error on timezone offset API response");
        }
    } else {
        Serial.println("Failed to get timezone offset from API");
    }
}

bool GlobalTime::getFormat24Hour() {
    return m_format24hour;
}

bool GlobalTime::setFormat24Hour(bool format24hour) {
    m_format24hour = format24hour;
    return m_format24hour;
}
