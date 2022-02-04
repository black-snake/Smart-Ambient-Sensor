#ifndef LedFlasher_h
#define LedFlasher_h

#include <Arduino.h>
#include <Ticker.h>

#ifdef ESP32
#define LED_ON HIGH
#define LED_OFF LOW
#else
#define LED_ON LOW
#define LED_OFF HIGH
#endif

class LedFlasher
{
private:
    Ticker _ticker;

    static void toggleLed();

public:
    LedFlasher();
    ~LedFlasher();

    void start(uint32_t interval);
    void stop();
};

#endif