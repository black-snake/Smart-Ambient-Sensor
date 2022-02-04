#ifndef Led_h
#define Led_h

#include <Arduino.h>

class Led
{
public:
    enum State
    {
#ifdef ESP32
        On = HIGH,
        Off = LOW
#else
        On = LOW,
        Off = HIGH
#endif
    };

    Led();
    ~Led();

    void set(State state);
    void toggle();

private:
    State _lastState = Off;
};

#endif