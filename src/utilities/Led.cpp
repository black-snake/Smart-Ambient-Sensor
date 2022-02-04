#include "Led.h"

Led::Led()
{
    pinMode(LED_BUILTIN, OUTPUT);
}

Led::~Led()
{
    set(Led::State::Off);
}

void Led::set(State state)
{
    digitalWrite(LED_BUILTIN, (uint8_t)state);
}

void Led::toggle()
{
    _lastState = _lastState == Led::State::Off ? Led::State::On : Led::State::Off;
    set(_lastState);
}