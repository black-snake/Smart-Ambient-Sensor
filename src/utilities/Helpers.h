#ifndef Helpers_h
#define Helpers_h

#include <Arduino.h>

#ifdef ESP32
#include <rom/rtc.h>
#else
extern "C"
{
#include "user_interface.h"
}
#endif

class Helpers
{
private:
#ifdef ESP32
    RTC_DATA_ATTR static unsigned long _millisOffset;
#else
    static unsigned long _millisOffset;
#endif

public:
    enum LedState
    {
#ifdef ESP32
        On = HIGH,
        Off = LOW
#else
        On = LOW,
        Off = HIGH
#endif
    };

    static String getChipId();

    static bool hasStartedFromDeepSleep();

    static unsigned long getMillis();
    static void setMillisOffset(unsigned long offset);

    static void setLedState(LedState ledState);
};

#endif