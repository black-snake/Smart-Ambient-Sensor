#ifndef ConfigManager_h
#define ConfigManager_h

#ifdef ESP32
#include <FS.h>
#include <SPIFFS.h>
#define FILESYSTEM SPIFFS
#else
#include <LittleFS.h>
#define FILESYSTEM LittleFS
#endif

#include "ArduinoJson.h"

#include "models/Config.h"
#include "models/AmbientSensorConfig.h"
#include "models/MqttConfig.h"
#include "models/WiFiConfig.h"

class ConfigManager
{
private:
    const String CONFIG_DIR = "/config";

    template <class T>
    String getPath(T &config);

    bool hardReset();
    bool softReset();

public:
    enum ResetMode
    {
        SoftReset,
        HardReset
    };

    ConfigManager();
    ~ConfigManager();
    bool reset(ResetMode resetMode);

    template <class T>
    bool exists(T &config);

    template <class T>
    bool read(T &config);

    template <class T>
    bool write(T &config);

    template <class T>
    bool remove(T &config);
};

template <class T>
String ConfigManager::getPath(T &config)
{
    return CONFIG_DIR + "/" + config.getFileName();
}

template <class T>
bool ConfigManager::exists(T &config)
{
    String path = getPath(config);
    return FILESYSTEM.exists(path);
}

template <class T>
bool ConfigManager::read(T &config)
{
    String path = getPath(config);

    if (!FILESYSTEM.exists(path))
    {
        Serial.print(F("Error: The config file does not exist: "));
        Serial.println(path);
        return false;
    }

    File file = FILESYSTEM.open(path, "r");

    if (file)
    {
        String content = file.readString();
        config = T::deserialize(content.c_str());
        file.close();

        Serial.print(F("Successfully read config file: "));
        Serial.println(path);
        Serial.print(F("File content read: "));
        Serial.println(content);

        return true;
    }
    else
    {
        Serial.print(F("Error: Failed to read config file: "));
        Serial.println(path);
        return false;
    }
}

template <class T>
bool ConfigManager::write(T &config)
{
    String path = getPath(config);

    File file = FILESYSTEM.open(path, "w");

    if (file)
    {
        String content = config.serialize();
        const char *str = content.c_str();
        file.write((const uint8_t *)str, strlen_P(str));
        file.close();

        Serial.print(F("Successfully written config file: "));
        Serial.println(path);
        Serial.print(F("File content written: "));
        Serial.println(content);

        return true;
    }
    else
    {
        Serial.print(F("Error: Failed to write config file: "));
        Serial.println(path);
        return false;
    }
}

template <class T>
bool ConfigManager::remove(T &config)
{
    String path = getPath(config);
    bool result = FILESYSTEM.remove(path);

    if (result)
    {
        Serial.print(F("Successfully removed config file: "));
        Serial.println(path);
    }
    else
    {
        Serial.print(F("Error: Failed to remove config file: "));
        Serial.println(path);
    }

    return result;
}

#endif