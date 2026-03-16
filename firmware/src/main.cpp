#include "Button.h"
#include "GlobalTime.h"
#include "ScreenManager.h"
#include "Utils.h"
#include "clockwidget/ClockWidget.h"
#include "config_helper.h"
#include "icons.h"
#include "wifiwidget/WifiWidget.h"
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <TJpg_Decoder.h>

TFT_eSPI tft = TFT_eSPI();

Button buttonLeft(BUTTON_LEFT);
Button buttonOK(BUTTON_OK);
Button buttonRight(BUTTON_RIGHT);

GlobalTime *globalTime;
ScreenManager *sm;
WifiWidget *wifiWidget{nullptr};
ClockWidget *clockWidget{nullptr};

bool clockInitialized = false;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    if (y >= tft.height())
        return 0;
    for (int i = 0; i < w * h; i++) {
        bitmap[i] = Utils::rgb565dim(bitmap[i], sm->getBrightness(), true);
    }
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

void isrButtonChangeLeft() { buttonLeft.isrButtonChange(); }
void isrButtonChangeMiddle() { buttonOK.isrButtonChange(); }
void isrButtonChangeRight() { buttonRight.isrButtonChange(); }

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Starting up...");

    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);

    buttonLeft.begin();
    buttonOK.begin();
    buttonRight.begin();
    attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), isrButtonChangeLeft, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_OK), isrButtonChangeMiddle, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), isrButtonChangeRight, CHANGE);

    sm = new ScreenManager(tft);
    sm->fillAllScreens(TFT_BLACK);
    sm->setFontColor(TFT_WHITE);

    sm->selectScreen(0);
    sm->drawCentreString("Network", ScreenCenterX, ScreenCenterY - 20, 29);
    sm->drawCentreString("Status Clock", ScreenCenterX, ScreenCenterY + 20, 22);

    sm->selectScreen(2);
    TJpgDec.setJpgScale(1);
    TJpgDec.drawJpg(0, 0, logo_start, logo_end - logo_start);

    pinMode(BUSY_PIN, OUTPUT);
    Serial.println("Connecting to WiFi");

    wifiWidget = new WifiWidget(*sm);
    wifiWidget->setup();

    globalTime = GlobalTime::getInstance();

    clockWidget = new ClockWidget(*sm);
    clockWidget->setup();

    ArduinoOTA.setHostname("network-status-clock");
    ArduinoOTA.begin();
    Serial.println("OTA ready");
}

void updateBrightnessByTime(uint8_t hour24) {
#if defined(DIM_START_HOUR) && defined(DIM_END_HOUR) && defined(DIM_BRIGHTNESS)
    bool isInDimRange;
    if (DIM_START_HOUR < DIM_END_HOUR) {
        isInDimRange = (hour24 >= DIM_START_HOUR && hour24 < DIM_END_HOUR);
    } else {
        isInDimRange = (hour24 >= DIM_START_HOUR || hour24 < DIM_END_HOUR);
    }
    uint8_t brightness = isInDimRange ? DIM_BRIGHTNESS : TFT_BRIGHTNESS;
    if (sm->setBrightness(brightness)) {
        sm->clearAllScreens();
        clockWidget->draw(true);
    }
#endif
}

void loop() {
    ArduinoOTA.handle();
    if (!wifiWidget->isConnected()) {
        wifiWidget->update();
        wifiWidget->draw();
        clockInitialized = false;
        delay(100);
    } else {
        globalTime->updateTime();

        if (!clockInitialized) {
            sm->clearAllScreens();
            clockWidget->update(true);
            clockWidget->draw(true);
            clockInitialized = true;
        }

        ButtonState leftState = buttonLeft.getState();
        if (leftState != BTN_NOTHING) {
            clockWidget->buttonPressed(BUTTON_LEFT, leftState);
        }
        ButtonState okState = buttonOK.getState();
        if (okState != BTN_NOTHING) {
            clockWidget->buttonPressed(BUTTON_OK, okState);
        }
        ButtonState rightState = buttonRight.getState();
        if (rightState != BTN_NOTHING) {
            clockWidget->buttonPressed(BUTTON_RIGHT, rightState);
        }

        clockWidget->update();
        updateBrightnessByTime(globalTime->getHour24());
        clockWidget->draw();
    }
}
