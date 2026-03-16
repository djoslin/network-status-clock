#pragma once
#ifndef WIFIWIDGET_H
    #define WIFIWIDGET_H

    #include "ScreenManager.h"

class WifiWidget {
public:
    WifiWidget(ScreenManager &manager);
    void setup();
    void update();
    void draw();
    bool isConnected() { return m_isConnected; }

private:
    ScreenManager &m_manager;
    void connectionTimedOut();

    bool m_isConnected{false};
    bool m_connectionFailed{false};
    bool m_hardCodedWiFi{false};
    bool m_hasDisplayedError{false};
    bool m_hasDisplayedSuccess{false};
    bool m_configPortalRunning{false};

    String m_connectionString{""};
    String m_dotsString{""};
    String m_ipaddress{""};
    String m_apssid{""};
    int m_connectionTimer{0};
    const int m_connectionTimeout{10000};
};

#endif // WIFIWIDGET_H
