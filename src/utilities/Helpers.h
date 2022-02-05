#ifndef Helpers_h
#define Helpers_h

#include <Arduino.h>
#include "Timer.h"

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

    static bool _ledState;

    static bool _hasWokenUpFromDeepSleep;

    static void toggleLed();

    static bool getHasWokenUpFromDeepSleep();

public:
    static Timer ledFlasher;

    static void init();

    static String getChipId();

    static bool hasWokenUpFromDeepSleep();

    static void setMillisOffset(unsigned long offset);
    static unsigned long getMillis();
};

#endif