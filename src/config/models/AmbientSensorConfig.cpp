#include "AmbientSensorConfig.h"

AmbientSensorConfig::AmbientSensorConfig()
{
}

AmbientSensorConfig::AmbientSensorConfig(Type type, TemperatureUnit temperatureUnit, uint16_t measurementIntervalInSeconds)
{
    this->type = type;
    this->temperatureUnit = temperatureUnit;
    this->measurementIntervalInSeconds = measurementIntervalInSeconds;
}

AmbientSensorConfig::AmbientSensorConfig(const char *type, const char *temperatureUnit, uint16_t measurementIntervalInSeconds)
{
    switch (atoi(type))
    {
    case 11:
        this->type = DHT11;
        break;
    case 12:
        this->type = DHT12;
        break;
    case 21:
        this->type = DHT21;
        break;
    case 22:
        this->type = DHT22;
        break;

    default:
        this->type = None;
        break;
    }

    switch (atoi(temperatureUnit))
    {
    case 1:
        this->temperatureUnit = TemperatureUnit::Celsius;
        break;
    case 2:
        this->temperatureUnit = TemperatureUnit::Fahrenheit;
        break;

    default:
        this->temperatureUnit = TemperatureUnit::Kelvin;
        break;
    }

    this->measurementIntervalInSeconds = measurementIntervalInSeconds;
}

const char *AmbientSensorConfig::getFileName()
{
    return "sensor.json";
}

String AmbientSensorConfig::serialize()
{
    StaticJsonDocument<48> doc;

    doc["temperatureUnit"] = temperatureUnit;
    doc["type"] = type;
    doc["measurementIntervalInSeconds"] = measurementIntervalInSeconds;

    String output;
    serializeJson(doc, output);
    return output;
}

AmbientSensorConfig AmbientSensorConfig::deserialize(const char *content)
{
    StaticJsonDocument<128> doc;

    DeserializationError error = deserializeJson(doc, content);

    if (error)
    {
        Serial.print(F("Error: deserialization of sensor config failed: "));
        Serial.println(error.f_str());
    }

    AmbientSensorConfig ambientSensorConfig(doc["type"].as<Type>(), doc["temperatureUnit"].as<TemperatureUnit>(), doc["measurementIntervalInSeconds"]);

    return ambientSensorConfig;
}