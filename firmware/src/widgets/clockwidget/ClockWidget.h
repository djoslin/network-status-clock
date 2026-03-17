#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include "Button.h"
#include "GlobalTime.h"
#include "ScreenManager.h"

#define CLOCK_FONT DSEG7
#define CLOCK_FONT_SIZE 200
#define CLOCK_OFFSET_X_DIGITS -10
#define CLOCK_OFFSET_X_COLON 0
#define CLOCK_DIGITS_OFFSET \
    {                       \
        {0, 0}, {1, -5}, {0, 0}, {1, 0}, {0, -5}, {0, 0}, {0, 0}, {0, -5}, {0, 0}, {0, 0}}

#ifndef CLOCK_COLOR
    #define CLOCK_COLOR 0xfc80
#endif
#ifndef CLOCK_COLOR_NET_DOWN
    #define CLOCK_COLOR_NET_DOWN CLOCK_COLOR
#endif
#ifndef CLOCK_COLOR_NET_UP
    #define CLOCK_COLOR_NET_UP TFT_GREEN
#endif
#ifndef CLOCK_NET_CHECK_INTERVAL_MS
    #define CLOCK_NET_CHECK_INTERVAL_MS 30000
#endif
#ifndef CLOCK_NET_RECOVERY_MS
    #define CLOCK_NET_RECOVERY_MS 900000
#endif
#ifndef CLOCK_NET_FAIL_INTERVAL
    #define CLOCK_NET_FAIL_INTERVAL 10000
#endif
#ifndef CLOCK_NET_RECOVER_INTERVAL
    #define CLOCK_NET_RECOVER_INTERVAL 10000
#endif
#ifndef CLOCK_SHADOW_COLOR
    #define CLOCK_SHADOW_COLOR 0x20a1
#endif
#ifndef CLOCK_SHADOWING
    #define CLOCK_SHADOWING true
#endif

struct DigitOffset {
    int x;
    int y;
};

enum class NetStatus {
    NET_OK,
    NET_DOWN,
    NET_RECOVERED
};

class ClockWidget {
public:
    ClockWidget(ScreenManager &manager);
    void setup();
    void update(bool force = false);
    void draw(bool force = false);
    void buttonPressed(uint8_t buttonId, ButtonState state);

private:
    ScreenManager &m_manager;

    void change24hMode();
    void displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color, bool shadowing);
    void displayDigit(int displayIndex, const String &lastDigit, const String &digit, uint32_t color);
    void displaySeconds(int displayIndex, int seconds, int color);
    void displayAmPm(String &amPm, uint32_t color);
    void displayStatusScreen(uint32_t color, bool force);
    DigitOffset getOffsetForDigit(const String &digit);
    bool checkInternet();
    NetStatus getEffectiveNetStatus();
    void toggleStatusPreview();
    void toggleIPDisplay();
    void displayIPAddress(uint32_t color, bool force);
    void simulateOutage();
    void simulateRecovery();
    void cancelDebug();
    void clearScreen2State();
    uint32_t getClockColor();

    NetStatus m_netStatus = NetStatus::NET_OK;
    NetStatus m_lastDrawnNetStatus = NetStatus::NET_OK;
    unsigned long m_netCheckPrev = 0;
    unsigned long m_downtimeStart = 0;
    unsigned long m_lastDowntimeMs = 0;
    unsigned long m_recoveryStart = 0;
    unsigned long m_failStart = 0;
    unsigned long m_successStart = 0;
    bool m_forceStatusView{false};
    bool m_debugSimulation{false};
    bool m_showIP{false};
    String m_lastIPStr = "";
    uint32_t m_lastClockColor = CLOCK_COLOR;
    String m_lastDowntimeHStr = "";
    String m_lastDowntimeMStr = "";

    unsigned long m_secondTimer = 2000;
    unsigned long m_secondTimerPrev = 0;

    int m_minuteSingle;
    int m_hourSingle;
    int m_secondSingle;

    int m_lastMinuteSingle{-1};
    int m_lastHourSingle{-1};
    int m_lastSecondSingle{-1};

    String m_display1Digit;
    String m_lastDisplay1Digit{""};
    String m_display2Digit;
    String m_lastDisplay2Digit{""};
    String m_display4Digit;
    String m_lastDisplay4Digit{""};
    String m_display5Digit;
    String m_lastDisplay5Digit{""};

    String m_amPm;
    String m_lastAmPm{""};

    DigitOffset m_digitOffsets[10] = CLOCK_DIGITS_OFFSET;
};
#endif // CLOCKWIDGET_H
