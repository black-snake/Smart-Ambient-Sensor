#include "ResetDetector.h"

ResetDetector::ResetDetector(ConfigManager *pConfigManager, unsigned long timeoutInSeconds)
{
    this->_end = Helpers::getMillis() + timeoutInSeconds * 1000;
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

bool ResetDetector::isEnabled()
{
    return _isEnabled;
}

bool ResetDetector::shouldReset()
{
    return _shouldReset;
}

void ResetDetector::process()
{
    if (!_isEnabled)
    {
        return;
    }

    if (Helpers::getMillis() > _end)
    {
        disable();
    }
}

void ResetDetector::disable()
{
    _isEnabled = false;
    _shouldReset = false;
    _pConfigManager->remove(_resetConfig);
}