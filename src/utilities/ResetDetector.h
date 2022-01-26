#ifndef ResetDetector_h
#define ResetDetector_h

#include <Arduino.h>
#include "../config/models/ResetConfig.h"
#include "../config/ConfigManager.h"
#include "Helpers.h"

class ResetDetector
{
private:
    unsigned long _end;
    bool _isEnabled = true;
    bool _shouldReset;
    ConfigManager *_pConfigManager = nullptr;
    ResetConfig _resetConfig;

public:
    ResetDetector(ConfigManager *pConfigManager, unsigned long timeoutInSeconds = 10);
    ~ResetDetector();

    bool isEnabled();
    bool shouldReset();
    void process();
    void disable();
};

#endif