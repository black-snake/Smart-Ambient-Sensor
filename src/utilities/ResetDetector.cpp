#include "ResetDetector.h"

ResetDetector::ResetDetector(ConfigManager *pConfigManager)
{
    this->_pConfigManager = pConfigManager;
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
    return _pConfigManager->exists(_resetConfig);
}

void ResetDetector::start(unsigned long timeout)
{
    if (_isEnabled)
    {
        return;
    }

    _end = Helpers::getMillis() + timeout;

    if (_pConfigManager->write(_resetConfig))
    {
        _isEnabled = true;
    }
}

void ResetDetector::stop()
{
    if (!_isEnabled)
    {
        return;
    }

    if (_pConfigManager->remove(_resetConfig))
    {
        _isEnabled = false;
    }
}

void ResetDetector::process()
{
    if (!_isEnabled)
    {
        return;
    }

    if (Helpers::getMillis() > _end)
    {
        stop();
    }
}