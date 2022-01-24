#include "Helpers.h"

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

unsigned long Helpers::_millisOffset;

unsigned long Helpers::getMillis()
{
#ifdef ESP8266
    rst_info *pRstInfo = ESP.getResetInfoPtr();

    if ((pRstInfo)->reason == REASON_DEEP_SLEEP_AWAKE)
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
    bool result = ESP.rtcUserMemoryWrite(0, (uint32_t *)&_millisOffset, sizeof(_millisOffset));
#endif
}