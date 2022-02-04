#ifndef ResetDetector_h
#define ResetDetector_h

#include <Arduino.h>
#include "../config/models/ResetConfig.h"
#include "../config/ConfigManager.h"
#include "Helpers.h"

class ResetDetector
{
private:
    unsigned long _end = 0;
    bool _isEnabled = false;
    ConfigManager *_pConfigManager = nullptr;
    ResetConfig _resetConfig;

public:
    ResetDetector(ConfigManager *pConfigManager);
    ~ResetDetector();

    bool isEnabled();
    bool shouldReset();
    void start(unsigned long timeout = 10 * 1000);
    void stop();
    void process();
};

#endif