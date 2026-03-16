#include "ClockWidget.h"

#include "config_helper.h"
#include <WiFi.h>
#include <WiFiClient.h>

ClockWidget::ClockWidget(ScreenManager &manager) : m_manager(manager) {
}

void ClockWidget::setup() {
    m_lastDisplay1Digit = "";
    m_lastDisplay2Digit = "";
    m_lastDisplay4Digit = "";
    m_lastDisplay5Digit = "";
}

void ClockWidget::draw(bool force) {
    m_manager.setFont(CLOCK_FONT);
    GlobalTime *time = GlobalTime::getInstance();

    uint32_t clockColor = getClockColor();
    if (clockColor != m_lastClockColor) {
        m_lastClockColor = clockColor;
        force = true;
    }

    if (m_lastDisplay1Digit != m_display1Digit || force) {
        displayDigit(0, m_lastDisplay1Digit, m_display1Digit, clockColor);
        m_lastDisplay1Digit = m_display1Digit;
    }
    if (m_lastDisplay2Digit != m_display2Digit || force) {
        displayDigit(1, m_lastDisplay2Digit, m_display2Digit, clockColor);
        m_lastDisplay2Digit = m_display2Digit;
    }
    if (m_lastDisplay4Digit != m_display4Digit || force) {
        displayDigit(3, m_lastDisplay4Digit, m_display4Digit, clockColor);
        m_lastDisplay4Digit = m_display4Digit;
    }
    if (m_lastDisplay5Digit != m_display5Digit || force) {
        displayDigit(4, m_lastDisplay5Digit, m_display5Digit, clockColor);
        m_lastDisplay5Digit = m_display5Digit;
    }

    if (m_secondSingle != m_lastSecondSingle || force) {
        NetStatus effectiveStatus = getEffectiveNetStatus();
        bool isStatusMode = !m_showIP && (effectiveStatus != NetStatus::NET_OK);

        // Clear screen 2 on net status mode transitions
        if (!m_showIP && effectiveStatus != m_lastDrawnNetStatus) {
            m_manager.selectScreen(2);
            m_manager.clearScreen();
            m_lastDrawnNetStatus = effectiveStatus;
            m_lastDowntimeHStr = "";
            m_lastDowntimeMStr = "";
            m_lastAmPm = "";
            m_lastIPStr = "";
        }

        // Flashing colon only in normal mode (no IP, no status)
        if (!isStatusMode && !m_showIP) {
            if (m_secondSingle % 2 == 0) {
                displayDigit(2, "", ":", clockColor, false);
            } else {
                displayDigit(2, "", ":", CLOCK_SHADOW_COLOR, false);
            }
        }

#if SHOW_SECOND_TICKS == true
        displaySeconds(2, m_lastSecondSingle, TFT_BLACK);
        displaySeconds(2, m_secondSingle, clockColor);
#endif
        m_lastSecondSingle = m_secondSingle;

        if (m_showIP) {
            displayIPAddress(clockColor, force);
        } else if (isStatusMode) {
            displayStatusScreen(clockColor, force);
        } else if (!FORMAT_24_HOUR && SHOW_AM_PM_INDICATOR) {
            if (m_amPm != m_lastAmPm) {
                displayAmPm(m_lastAmPm, TFT_BLACK);
                m_lastAmPm = m_amPm;
            }
            displayAmPm(m_amPm, clockColor);
        }
    }
}

void ClockWidget::displayAmPm(String &amPm, uint32_t color) {
    m_manager.selectScreen(2);
    m_manager.setFontColor(color, TFT_BLACK);
    m_manager.setFont(DEFAULT_FONT);
    m_manager.drawString(amPm, SCREEN_SIZE / 5 * 4, SCREEN_SIZE / 2, 36, Align::MiddleCenter);
}

void ClockWidget::displayStatusScreen(uint32_t color, bool force) {
    m_manager.selectScreen(2);
    m_manager.setFont(DEFAULT_FONT);

    NetStatus effective = getEffectiveNetStatus();
    unsigned long downtimeMs = (effective == NetStatus::NET_DOWN)
        ? millis() - m_downtimeStart
        : m_lastDowntimeMs;
    unsigned long totalSec = downtimeMs / 1000;
    String hStr = String(totalSec / 3600) + "h";
    String mStr = String((totalSec % 3600) / 60) + "m";

    bool hChanged = (hStr != m_lastDowntimeHStr) || force;
    bool mChanged = (mStr != m_lastDowntimeMStr) || force;

    // Erase old text
    m_manager.setFontColor(TFT_BLACK, TFT_BLACK);
    if (hChanged && m_lastDowntimeHStr.length() > 0) {
        m_manager.drawString(m_lastDowntimeHStr, SCREEN_SIZE / 2, 90, 60, Align::MiddleCenter);
    }
    if (mChanged && m_lastDowntimeMStr.length() > 0) {
        m_manager.drawString(m_lastDowntimeMStr, SCREEN_SIZE / 2, 155, 60, Align::MiddleCenter);
    }

    // Draw new text
    m_manager.setFontColor(color, TFT_BLACK);
    if (hChanged) {
        m_manager.drawString(hStr, SCREEN_SIZE / 2, 90, 60, Align::MiddleCenter);
        m_lastDowntimeHStr = hStr;
    }
    if (mChanged) {
        m_manager.drawString(mStr, SCREEN_SIZE / 2, 155, 60, Align::MiddleCenter);
        m_lastDowntimeMStr = mStr;
    }
}

bool ClockWidget::checkInternet() {
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }
    WiFiClient client;
    bool connected = client.connect(IPAddress(8, 8, 8, 8), 53, 2000);
    if (connected) {
        client.stop();
    }
    return connected;
}

bool ClockWidget::checkRouter() {
    IPAddress gateway = WiFi.gatewayIP();
    if (gateway == IPAddress(0, 0, 0, 0)) {
        return false;
    }
    WiFiClient client;
    bool connected = client.connect(gateway, 53, 2000);
    if (connected) {
        client.stop();
    }
    return connected;
}

NetStatus ClockWidget::getEffectiveNetStatus() {
    if (m_forceStatusView && m_netStatus == NetStatus::NET_OK) {
        return NetStatus::NET_RECOVERED;
    }
    return m_netStatus;
}

void ClockWidget::clearScreen2State() {
    m_manager.selectScreen(2);
    m_manager.clearScreen();
    m_lastDrawnNetStatus = getEffectiveNetStatus();
    m_lastDowntimeHStr = "";
    m_lastDowntimeMStr = "";
    m_lastAmPm = "";
    m_lastIPStr = "";
}

void ClockWidget::toggleStatusPreview() {
    m_forceStatusView = !m_forceStatusView;
    m_showIP = false;
    clearScreen2State();
    draw(true);
}

void ClockWidget::toggleIPDisplay() {
    m_showIP = !m_showIP;
    m_forceStatusView = false;
    clearScreen2State();
    draw(true);
}

void ClockWidget::simulateOutage() {
    m_debugSimulation = true;
    m_forceStatusView = false;
    m_showIP = false;
    m_netStatus = NetStatus::NET_DOWN;
    m_downtimeStart = millis();
    clearScreen2State();
    draw(true);
}

void ClockWidget::simulateRecovery() {
    m_debugSimulation = true;
    m_forceStatusView = false;
    m_showIP = false;
    if (m_netStatus == NetStatus::NET_DOWN) {
        m_lastDowntimeMs = millis() - m_downtimeStart;
    }
    m_netStatus = NetStatus::NET_RECOVERED;
    m_recoveryStart = millis();
    clearScreen2State();
    draw(true);
}

void ClockWidget::cancelDebug() {
    m_debugSimulation = false;
    m_forceStatusView = false;
    m_showIP = false;
    m_netStatus = NetStatus::NET_OK;
    m_consecutiveFails = 0;
    m_consecutiveSuccesses = 0;
    m_netCheckPrev = 0; // Force immediate re-check
    clearScreen2State();
    draw(true);
}

void ClockWidget::displayIPAddress(uint32_t color, bool force) {
    m_manager.selectScreen(2);
    m_manager.setFont(DEFAULT_FONT);
    String ipStr = WiFi.localIP().toString();
    if (ipStr != m_lastIPStr || force) {
        if (m_lastIPStr.length() > 0) {
            m_manager.setFontColor(TFT_BLACK, TFT_BLACK);
            m_manager.drawString(m_lastIPStr, SCREEN_SIZE / 2, SCREEN_SIZE / 2, 22, Align::MiddleCenter);
        }
        m_manager.setFontColor(color, TFT_BLACK);
        m_manager.drawString(ipStr, SCREEN_SIZE / 2, SCREEN_SIZE / 2, 22, Align::MiddleCenter);
        m_lastIPStr = ipStr;
    }
}

uint32_t ClockWidget::getClockColor() {
    switch (getEffectiveNetStatus()) {
    case NetStatus::NET_DOWN:
        return CLOCK_COLOR_NET_DOWN;
    case NetStatus::NET_RECOVERED:
        return CLOCK_COLOR_NET_UP;
    default:
        return CLOCK_COLOR;
    }
}



void ClockWidget::update(bool force) {
    if (!m_debugSimulation && (millis() - m_netCheckPrev >= CLOCK_NET_CHECK_INTERVAL_MS || m_netCheckPrev == 0)) {
        m_netCheckPrev = millis();
        bool internetUp = checkInternet();

        if (internetUp) {
            m_consecutiveFails = 0;
            m_consecutiveSuccesses++;
        } else {
            // Only count as internet failure if the router is still reachable
            bool routerUp = checkRouter();
            if (routerUp) {
                if (m_consecutiveFails == 0) {
                    m_firstFailTime = millis();
                }
                m_consecutiveFails++;
            }
            // If router is also down, it's a WiFi issue — don't count
            m_consecutiveSuccesses = 0;
        }

        switch (m_netStatus) {
        case NetStatus::NET_OK:
            if (m_consecutiveFails >= CLOCK_NET_FAIL_THRESHOLD) {
                m_netStatus = NetStatus::NET_DOWN;
                m_downtimeStart = m_firstFailTime;
                m_consecutiveFails = 0;
            }
            break;
        case NetStatus::NET_DOWN:
            if (m_consecutiveSuccesses >= CLOCK_NET_RECOVER_THRESHOLD) {
                m_netStatus = NetStatus::NET_RECOVERED;
                m_lastDowntimeMs = millis() - m_downtimeStart;
                m_recoveryStart = millis();
                m_consecutiveSuccesses = 0;
            }
            break;
        case NetStatus::NET_RECOVERED:
            if (m_consecutiveFails >= CLOCK_NET_FAIL_THRESHOLD) {
                m_netStatus = NetStatus::NET_DOWN;
                m_downtimeStart = m_firstFailTime;
                m_consecutiveFails = 0;
            } else if (millis() - m_recoveryStart >= CLOCK_NET_RECOVERY_MS) {
                m_netStatus = NetStatus::NET_OK;
            }
            break;
        }
    }

    if (millis() - m_secondTimerPrev < m_secondTimer && !force) {
        return;
    }

    GlobalTime *time = GlobalTime::getInstance();

    m_hourSingle = time->getHour();
    m_minuteSingle = time->getMinute();
    m_secondSingle = time->getSecond();
    m_amPm = time->isPM() ? "PM" : "AM";

    if (m_lastHourSingle != m_hourSingle || force) {
        if (m_hourSingle < 10) {
            if (FORMAT_24_HOUR) {
                m_display1Digit = "0";
            } else {
                m_display1Digit = " ";
            }
        } else {
            m_display1Digit = int(m_hourSingle / 10);
        }
        m_display2Digit = m_hourSingle % 10;

        m_lastHourSingle = m_hourSingle;
    }

    if (m_lastMinuteSingle != m_minuteSingle || force) {
        String currentMinutePadded = String(m_minuteSingle).length() == 1 ? "0" + String(m_minuteSingle) : String(m_minuteSingle);

        m_display4Digit = currentMinutePadded.substring(0, 1);
        m_display5Digit = currentMinutePadded.substring(1, 2);

        m_lastMinuteSingle = m_minuteSingle;
    }
}

void ClockWidget::change24hMode() {
    GlobalTime *time = GlobalTime::getInstance();
    time->setFormat24Hour(!time->getFormat24Hour());
    draw(true);
}

void ClockWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_LEFT && state == BTN_SHORT) {
        change24hMode();
    } else if (buttonId == BUTTON_OK && state == BTN_SHORT) {
        toggleIPDisplay();
    } else if (buttonId == BUTTON_RIGHT) {
        if (state == BTN_SHORT) {
            if (m_debugSimulation || m_forceStatusView) {
                cancelDebug();
            } else {
                toggleStatusPreview();
            }
        } else if (state == BTN_MEDIUM) {
            simulateOutage();
        } else if (state == BTN_LONG) {
            simulateRecovery();
        }
    }
}

DigitOffset ClockWidget::getOffsetForDigit(const String &digit) {
    if (digit.length() > 0) {
        char c = digit.charAt(0);
        if (c >= '0' && c <= '9') {
            // get digit offsets
            return m_digitOffsets[c - '0'];
        }
    }
    // not a valid digit
    return {0, 0};
}

void ClockWidget::displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color, bool shadowing) {
    int fontSize = CLOCK_FONT_SIZE;
    char c = digit.charAt(0);
    bool isDigit = c >= '0' && c <= '9' || c == ' ';
    int defaultX = SCREEN_SIZE / 2 + (isDigit ? CLOCK_OFFSET_X_DIGITS : CLOCK_OFFSET_X_COLON);
    int defaultY = SCREEN_SIZE / 2;
    DigitOffset digitOffset = getOffsetForDigit(digit);
    DigitOffset lastDigitOffset = getOffsetForDigit(lastDigit);
    m_manager.selectScreen(displayIndex);
    if (shadowing) {
        m_manager.setFontColor(CLOCK_SHADOW_COLOR, TFT_BLACK);
        // DSEG7 uses 8 to fill all segments
        m_manager.drawString("8", defaultX, defaultY, fontSize, Align::MiddleCenter);
    } else {
        m_manager.setFontColor(TFT_BLACK, TFT_BLACK);
        m_manager.drawString(lastDigit, defaultX + lastDigitOffset.x, defaultY + lastDigitOffset.y, fontSize, Align::MiddleCenter);
    }
    m_manager.setFontColor(color, TFT_BLACK);
    m_manager.drawString(digit, defaultX + digitOffset.x, defaultY + digitOffset.y, fontSize, Align::MiddleCenter);
}

void ClockWidget::displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color) {
    displayDigit(displayIndex, lastDigit, digit, color, CLOCK_SHADOWING);
}

void ClockWidget::displaySeconds(int displayIndex, int seconds, int color) {
    m_manager.selectScreen(displayIndex);
    if (seconds < 30) {
        m_manager.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 110, 6 * seconds + 180, 6 * seconds + 180 + 6, color, TFT_BLACK);
    } else {
        m_manager.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 110, 6 * seconds - 180, 6 * seconds - 180 + 6, color, TFT_BLACK);
    }
}

