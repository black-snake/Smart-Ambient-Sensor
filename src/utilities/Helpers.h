#ifndef Helpers_h
#define Helpers_h

#include <Arduino.h>

#ifdef ESP32

#define LED_ON HIGH
#define LED_OFF LOW

#else

#define LED_ON LOW
#define LED_OFF HIGH

#endif

class Helpers
{
public:
    static String getChipId();
};

#endif