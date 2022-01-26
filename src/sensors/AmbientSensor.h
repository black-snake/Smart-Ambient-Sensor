#ifndef AmbientSensor_h
#define AmbientSensor_h

#include "../config/models/AmbientSensorConfig.h"
#include "data/Measurement.h"
#include "../utilities/Helpers.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>

class AmbientSensor
{
private:
    DHT _dht;
    bool _isFirstCall = true;
    unsigned long _millis;
    Measurement<float> _lastTemperature;
    Measurement<float> _lastHumidity;

    String getTemperatureUnit(TemperatureUnit temperatureUnit);
    Measurement<float> getTemperature();
    Measurement<float> getHumidity();

    using MeasurementCallback = void (*)(Measurement<float> temperature, Measurement<float> humidity);
    MeasurementCallback _measurementCallback = nullptr;

public:
    AmbientSensor(uint8_t pin, AmbientSensorConfig ambientSensorConfig);
    ~AmbientSensor();

    const AmbientSensorConfig ambientSensorConfig;

    void setMeasurementCallback(const MeasurementCallback measurementCallback);
    void measure();
};

#endif