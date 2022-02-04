#include "Helpers.h"

unsigned long Helpers::_millisOffset;

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

void Helpers::setLedState(LedState ledState)
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, (uint8_t)ledState);
}