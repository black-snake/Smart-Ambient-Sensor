#include "LedFlasher.h"

void LedFlasher::toggleLed()
{
    bool state = digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, !state);
}

LedFlasher::LedFlasher()
{
    pinMode(LED_BUILTIN, OUTPUT);
}

LedFlasher::~LedFlasher()
{
}

void LedFlasher::start(uint32_t interval)
{
    _ticker.attach_ms(interval, toggleLed);
}

void LedFlasher::stop()
{
    _ticker.detach();

    digitalWrite(LED_BUILTIN, LED_OFF);
}