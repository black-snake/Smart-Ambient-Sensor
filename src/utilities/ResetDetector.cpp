#include "ResetDetector.h"

#define RESET_MAGIC_NUMBER 1234567890

unsigned long ResetDetector::_end = 0;

bool ResetDetector::_isEnabled = false;

#ifdef ESP32
unsigned long ResetDetector::_value = 0;
#else
#define RTC_MEMORY_OFFSET_RESET_DETECTOR 1
#endif

bool ResetDetector::isEnabled()
{
    return _isEnabled;
}

bool ResetDetector::shouldReset()
{
#ifdef ESP32
    return _value == RESET_MAGIC_NUMBER;
#else
    unsigned long value = 0;

    bool result = ESP.rtcUserMemoryRead(RTC_MEMORY_OFFSET_RESET_DETECTOR, (uint32_t *)&value, sizeof(value));

    return result && value == RESET_MAGIC_NUMBER;
#endif
}

void ResetDetector::go(unsigned long timeout)
{
    _end = Helpers::getMillis() + timeout;

#ifdef ESP32
    _value = RESET_MAGIC_NUMBER;
    _isEnabled = true;
#else
    unsigned long value = RESET_MAGIC_NUMBER;
    bool result = ESP.rtcUserMemoryWrite(RTC_MEMORY_OFFSET_RESET_DETECTOR, (uint32_t *)&value, sizeof(value));

    if (result)
    {
        _isEnabled = true;
    }
#endif
}

void ResetDetector::clear()
{
#ifdef ESP32
    _value = 0;
    _isEnabled = false;
#else
    unsigned long value = 0;
    bool result = ESP.rtcUserMemoryWrite(RTC_MEMORY_OFFSET_RESET_DETECTOR, (uint32_t *)&value, sizeof(value));

    if (result)
    {
        _isEnabled = false;
    }
#endif
}

void ResetDetector::process()
{
    if (!_isEnabled)
    {
        return;
    }

    if (Helpers::getMillis() > _end)
    {
        clear();
    }
}