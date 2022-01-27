#ifndef Timer_h
#define Timer_h

#ifdef ESP32
#include "FreeRTOS.h"
#include "freertos/timers.h"
#else
extern "C"
{
#include "user_interface.h"
}
#endif

class Timer
{
private:
#ifdef ESP32
    TimerHandle_t _pTimer = nullptr;
    static void invokeTimerCallback(TimerHandle_t timerHandle);
#else
    os_timer_t *_pTimer = nullptr;
    static void invokeTimerCallback(void *pArg);
#endif

    using TimerCallback = void (*)();
    TimerCallback _timerCallback = nullptr;

public:
    Timer(const TimerCallback timerCallback);
    ~Timer();

    void start(uint32_t interval);
    void stop();
};

#endif