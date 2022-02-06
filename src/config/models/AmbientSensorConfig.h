#ifndef AmbientSensorConfig_h
#define AmbientSensorConfig_h

#include "Config.h"

enum TemperatureUnit
{
    Kelvin = 0,
    Celsius = 1,
    Fahrenheit = 2
};

struct AmbientSensorConfig : public Config
{
public:
    enum Type
    {
        None = 0,
        DHT11 = 11,
        DHT12 = 12,
        DHT21 = 21,
        DHT22 = 22,
        AM2301 = 21
    };

    AmbientSensorConfig();
    AmbientSensorConfig(Type type, TemperatureUnit temperatureUnit, uint16_t measurementIntervalInSeconds);
    AmbientSensorConfig(const char *type, const char *temperatureUnit, uint16_t measurementIntervalInSeconds);

    TemperatureUnit temperatureUnit = Kelvin;
    Type type = None;
    uint16_t measurementIntervalInSeconds = 30;

    const char *getFileName();
    String serialize();
    static AmbientSensorConfig deserialize(const char *content);
};

#endif