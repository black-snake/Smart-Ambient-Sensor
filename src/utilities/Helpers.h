#ifndef Helpers_h
#define Helpers_h

#include <Arduino.h>

#ifdef ESP8266
extern "C"
{
#include "user_interface.h"
}
#endif

#ifdef ESP32

#define LED_ON HIGH
#define LED_OFF LOW

#else

#define LED_ON LOW
#define LED_OFF HIGH

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
    static String getChipId();
    static unsigned long getMillis();
    static void setMillisOffset(unsigned long offset);
};

#endif