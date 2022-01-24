#ifndef ResetDetector_h
#define ResetDetector_h

#include <Arduino.h>
#include "../config/models/ResetConfig.h"
#include "../config/ConfigManager.h"

class ResetDetector
{
private:
    unsigned long _end;
    bool _enabled = true;
    bool _shouldReset;
    ConfigManager *_pConfigManager = nullptr;
    ResetConfig _resetConfig;

public:
    ResetDetector(ConfigManager *pConfigManager, unsigned long timeoutInSeconds = 10);
    ~ResetDetector();

    bool shouldReset();
    void process();
    void stop();
};

#endif