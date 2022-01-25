#include "Helpers.h"

unsigned long Helpers::_millisOffset;

#ifdef ESP32
TimerHandle_t Helpers::_ledTimer;
#else
os_timer_t *Helpers::_pLedTimer = nullptr;
#endif

bool Helpers::_ledState = false;

String Helpers::getChipId()
{
#ifdef ESP32
    String chipId = String((uint32_t)ESP.getEfuseMac(), HEX);
#else
    String chipId = String(ESP.getChipId(), HEX);
#endif

    chipId.toUpperCase();
    return String("ESP-" + chipId);
}

bool Helpers::hasStartedFromDeepSleep()
{
#ifdef ESP32
    return rtc_get_reset_reason(0) == DEEPSLEEP_RESET;
#else
    rst_info *pRstInfo = ESP.getResetInfoPtr();

    return pRstInfo->reason == REASON_DEEP_SLEEP_AWAKE;
#endif
}

unsigned long Helpers::getMillis()
{
#ifdef ESP8266
    if (hasStartedFromDeepSleep())
    {
        ESP.rtcUserMemoryRead(0, (uint32_t *)&_millisOffset, sizeof(_millisOffset));
    }
#endif

    return millis() + _millisOffset;
}

void Helpers::setMillisOffset(unsigned long millisOffset)
{
    _millisOffset = getMillis() + millisOffset;

#ifdef ESP8266
    ESP.rtcUserMemoryWrite(0, (uint32_t *)&_millisOffset, sizeof(_millisOffset));
#endif
}

#ifdef ESP32
void Helpers::toggleLed(TimerHandle_t timerHandle)
#else
void Helpers::toggleLed(void *pArg)
#endif
{
    _ledState = !_ledState;
    digitalWrite(LED_BUILTIN, _ledState);
}

void Helpers::startLedFlashing(uint32_t interval)
{
    pinMode(LED_BUILTIN, OUTPUT);

#ifdef ESP32
    _ledTimer = xTimerCreate("toggleLed", pdMS_TO_TICKS(interval), pdTRUE, nullptr, toggleLed);
    xTimerStart(_ledTimer, 10);
#else
    if (_pLedTimer != nullptr)
    {
        delete[] _pLedTimer;
    }

    _pLedTimer = new os_timer_t();
    os_timer_setfn(_pLedTimer, toggleLed, nullptr);
    os_timer_arm(_pLedTimer, interval, true);
#endif
}

void Helpers::stopLedFlashing()
{
    // make sure the LED is off
    digitalWrite(LED_BUILTIN, LED_OFF);

#ifdef ESP32
    xTimerStop(_ledTimer, 10);
#else
    if (_pLedTimer != nullptr)
    {
        os_timer_disarm(_pLedTimer);
        delete[] _pLedTimer;
        _pLedTimer = nullptr;
    }
#endif
}