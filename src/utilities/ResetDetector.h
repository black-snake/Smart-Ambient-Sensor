#ifndef ResetDetector_h
#define ResetDetector_h

#include <Arduino.h>
#include "Helpers.h"

class ResetDetector
{
private:
    static unsigned long _end;
    static bool _isEnabled;

#ifdef ESP32
    RTC_DATA_ATTR static unsigned long _value;
#endif

public:
    static bool isEnabled();
    static bool shouldReset();
    static void go(unsigned long timeout = 10 * 1000);
    static void clear();
    static void process();
};

#endif