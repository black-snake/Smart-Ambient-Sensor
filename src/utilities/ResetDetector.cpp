#include "ResetDetector.h"

ResetDetector::ResetDetector(const ConfigManager &configManager) : _configManager(configManager)
{
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
    return _configManager.exists(_resetConfig);
}

void ResetDetector::go(unsigned long timeout)
{
    _end = Helpers::getMillis() + timeout;

    bool result = _configManager.write(_resetConfig);

    if (result)
    {
        _isEnabled = true;
    }
}

void ResetDetector::clear()
{
    bool result = true;

    if (_configManager.exists(_resetConfig))
    {
        result = _configManager.remove(_resetConfig);
    }

    if (result)
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
        clear();
    }
}