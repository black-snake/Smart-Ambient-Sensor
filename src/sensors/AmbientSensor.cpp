#include "AmbientSensor.h"

String AmbientSensor::getTemperatureUnit(TemperatureUnit temperatureUnit)
{
    switch (temperatureUnit)
    {
    case Kelvin:
        return "K";
    case Celsius:
        return "°C";
    case Fahrenheit:
        return "°F";
    default:
        return "";
    }
}

Measurement<float> AmbientSensor::getTemperature()
{
    bool useFahrenheit = ambientSensorConfig.temperatureUnit == Fahrenheit;

    _lastTemperature.value = _dht.readTemperature(useFahrenheit);

    if (ambientSensorConfig.temperatureUnit == Kelvin)
    {
        _lastTemperature.value += 273.15;
    }

    return _lastTemperature;
}

Measurement<float> AmbientSensor::getHumidity()
{
    _lastHumidity.value = _dht.readHumidity();

    return _lastHumidity;
}

AmbientSensor::AmbientSensor(uint8_t pin, AmbientSensorConfig ambientSensorConfig) : _dht(pin, ambientSensorConfig.type), ambientSensorConfig(ambientSensorConfig)
{
    this->_millis = millis();

    this->_lastTemperature.quantity = "temperature";
    this->_lastTemperature.unit = getTemperatureUnit(ambientSensorConfig.temperatureUnit);

    this->_lastHumidity.quantity = "humidity";
    this->_lastHumidity.unit = "%";

    _dht.begin();
}

AmbientSensor::~AmbientSensor()
{
}

void AmbientSensor::measure()
{
    bool value = (millis() - _millis) > ambientSensorConfig.measurementIntervalInSeconds * 1000;

    if (value)
    {
        _measurementCallback(getTemperature(), getHumidity());
        _millis = millis();
    }
}

void AmbientSensor::setMeasurementCallback(const MeasurementCallback measurementCallback)
{
    _measurementCallback = measurementCallback;
}