#include "Helpers.h"

#define RTC_MEMORY_OFFSET_MILLIS 0

unsigned long Helpers::_millisOffset;

bool Helpers::_hasWokenUpFromDeepSleep = Helpers::getHasWokenUpFromDeepSleep();

Led Helpers::led;

bool Helpers::getHasWokenUpFromDeepSleep()
{
#ifdef ESP32
    return rtc_get_reset_reason(0) == DEEPSLEEP_RESET;
#else
    rst_info *pRstInfo = ESP.getResetInfoPtr();

    return pRstInfo->reason == REASON_DEEP_SLEEP_AWAKE;
#endif
}

void Helpers::init()
{
#ifdef ESP32

#else
    if (hasWokenUpFromDeepSleep())
    {
        ESP.rtcUserMemoryRead(RTC_MEMORY_OFFSET_MILLIS, (uint32_t *)&_millisOffset, sizeof(_millisOffset));
    }
#endif
}

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

bool Helpers::hasWokenUpFromDeepSleep()
{
    return _hasWokenUpFromDeepSleep;
}

void Helpers::setMillisOffset(unsigned long millisOffset)
{
    _millisOffset = getMillis() + millisOffset;

#ifdef ESP8266
    ESP.rtcUserMemoryWrite(RTC_MEMORY_OFFSET_MILLIS, (uint32_t *)&_millisOffset, sizeof(_millisOffset));
#endif
}

unsigned long Helpers::getMillis()
{
    return millis() + _millisOffset;
}