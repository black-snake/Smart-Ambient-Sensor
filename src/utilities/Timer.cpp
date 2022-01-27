#include "Timer.h"

#ifdef ESP32
void Timer::invokeTimerCallback(TimerHandle_t timerHandle)
{
    TimerCallback callback = (TimerCallback)pvTimerGetTimerID(timerHandle);
    callback();
}
#else
void Timer::invokeTimerCallback(void *pArgs)
{
    ((TimerCallback)pArgs)();
}
#endif

Timer::Timer(const TimerCallback timerCallback)
{
    this->_timerCallback = timerCallback;
}

Timer::~Timer()
{
    this->stop();

    if (this->_pTimer == nullptr)
    {
        return;
    }

#ifdef ESP32
    xTimerDelete(_pTimer, 0);
#else
    delete[] this->_pTimer;
#endif

    this->_pTimer = nullptr;
}

void Timer::start(uint32_t interval)
{
#ifdef ESP32
    if (_pTimer != nullptr)
    {
        xTimerStop(_pTimer, 0);
    }

    _pTimer = xTimerCreate(nullptr, pdMS_TO_TICKS(interval), pdTRUE, (void *)_timerCallback, &Timer::invokeTimerCallback);
    xTimerStart(_pTimer, 0);
#else
    if (_pTimer != nullptr)
    {
        os_timer_disarm(_pTimer);
        delete[] _pTimer;
    }

    _pTimer = new os_timer_t();
    os_timer_setfn(_pTimer, &Timer::invokeTimerCallback, (void *)_timerCallback);
    os_timer_arm(_pTimer, interval, true);
#endif
}

void Timer::stop()
{
    if (_pTimer != nullptr)
    {
#ifdef ESP32
        xTimerStop(_pTimer, 0);
#else
        os_timer_disarm(_pTimer);
#endif
    }
}