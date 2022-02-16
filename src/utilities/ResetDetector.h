#ifndef ResetDetector_h
#define ResetDetector_h

#include <Arduino.h>
#include "config/ConfigManager.h"
#include "config/models/ResetConfig.h"
#include "Helpers.h"

class ResetDetector
{
private:
    unsigned long _end = 0;
    bool _isEnabled = false;
    ResetConfig _resetConfig;
    ConfigManager _configManager;

public:
    ResetDetector(const ConfigManager &configManager);
    ~ResetDetector();

    bool isEnabled();
    bool shouldReset();
    void go(unsigned long timeout = 10 * 1000);
    void clear();
    void process();
};

#endif