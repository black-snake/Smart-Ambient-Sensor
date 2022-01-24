#include "ResetDetector.h"

ResetDetector::ResetDetector(ConfigManager *pConfigManager, unsigned long timeoutInSeconds)
{
    this->_end = millis() + timeoutInSeconds * 1000;
    this->_pConfigManager = pConfigManager;

    this->_shouldReset = _pConfigManager->exists(_resetConfig);

    if (!this->_shouldReset)
    {
        _pConfigManager->write(_resetConfig);
    }
}

ResetDetector::~ResetDetector()
{
}

bool ResetDetector::shouldReset()
{
    return _shouldReset;
}

void ResetDetector::process()
{
    if (!_enabled)
    {
        return;
    }

    if (millis() > _end)
    {
        stop();
    }
}

void ResetDetector::stop()
{
    _enabled = false;
    _shouldReset = false;
    _pConfigManager->remove(_resetConfig);
}