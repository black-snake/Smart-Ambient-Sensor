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
    static void toggleLed();

public:
    static Timer ledFlasher;

    static String getChipId();
    static bool hasStartedFromDeepSleep();
    static unsigned long getMillis();
    static void setMillisOffset(unsigned long offset);
};

#endif