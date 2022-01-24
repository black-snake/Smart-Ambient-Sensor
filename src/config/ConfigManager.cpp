#include "ConfigManager.h"

ConfigManager::ConfigManager()
{
    bool result = FILESYSTEM.begin();

    if (!result)
    {
        Serial.println(F("Error: Failed to mount the file system."));
    }
}

ConfigManager::~ConfigManager()
{
    FILESYSTEM.end();
}

bool ConfigManager::hardReset()
{
    Serial.println(F("Performing hard reset."));

    bool result = FILESYSTEM.format();

    if (!result)
    {
        Serial.println(F("Error: Failed to hard reset (formatting file system failed)."));
    }

    Serial.println(F("Successfully performed hard reset."));

    return result;
}

bool ConfigManager::softReset()
{
    Serial.println(F("Performing soft reset."));

    // soft reset currently fails because LittleFS does not allow removal of non-empty directory
    // thus only hard reset clears all data
    Serial.println(F("Error: Failed to soft reset (not implemented)."));
    return false;

    // if (!FILESYSTEM.exists(CONFIG_DIR))
    // {
    //     return true;
    // }

    // bool rmdirResult = FILESYSTEM.rmdir(CONFIG_DIR);

    // if (!rmdirResult)
    // {
    //     Serial.println(F("Error: Failed to soft reset (couldn't remove config dir)."));
    //     return false;
    // }

    // bool mkdirResult = FILESYSTEM.mkdir(CONFIG_DIR);

    // if (!mkdirResult)
    // {
    //     Serial.println(F("Error: Failed to soft reset (couldn't create config dir)."));
    //     return false;
    // }

    // return rmdirResult && mkdirResult;
}

bool ConfigManager::reset(ResetMode resetMode)
{
    Serial.println(F("Reset has been requested."));

    switch (resetMode)
    {
    case HardReset:
        return hardReset();
    case SoftReset:
        return softReset();
    default:
        return false;
    }
}