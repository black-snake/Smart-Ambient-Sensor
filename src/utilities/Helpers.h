#ifndef Helpers_h
#define Helpers_h

#include <Arduino.h>

#ifdef ESP32
#include <rom/rtc.h>
#include "freertos/timers.h"
#define LED_ON HIGH
#define LED_OFF LOW
#else
extern "C"
{
#include "user_interface.h"
}
#define LED_ON LOW
#define LED_OFF HIGH
#endif

class Helpers
{
private:
#ifdef ESP32
    RTC_DATA_ATTR static unsigned long _millisOffset;
    static TimerHandle_t _pLedTimer;
#else
    static unsigned long _millisOffset;
    static os_timer_t *_pLedTimer;
#endif

    static bool _ledState;

#ifdef ESP32
    static void toggleLed(TimerHandle_t timerHandle);
#else
    static void toggleLed(void *pArg);
#endif

public:
    static String getChipId();
    static bool hasStartedFromDeepSleep();
    static unsigned long getMillis();
    static void setMillisOffset(unsigned long offset);
    static void startLedFlashing(uint32_t interval);
    static void stopLedFlashing();
};

#endif