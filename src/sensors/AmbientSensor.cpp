#include "AmbientSensor.h"

String AmbientSensor::getTemperatureUnit(TemperatureUnit temperatureUnit)
{
    switch (temperatureUnit)
    {
    case TemperatureUnit::Kelvin:
        return "K";
    case TemperatureUnit::Celsius:
        return "°C";
    case TemperatureUnit::Fahrenheit:
        return "°F";
    default:
        return "";
    }
}

Measurement<float> AmbientSensor::getTemperature()
{
    bool useFahrenheit = ambientSensorConfig.temperatureUnit == TemperatureUnit::Fahrenheit;

    _lastTemperature.value = _dht.readTemperature(useFahrenheit);

    if (ambientSensorConfig.temperatureUnit == TemperatureUnit::Kelvin)
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
    this->_millis = Helpers::getMillis();

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
    bool result = (Helpers::getMillis() - _millis) > ambientSensorConfig.measurementIntervalInSeconds * 1000;

    if (_isInitialMeasurement || result)
    {
        _isInitialMeasurement = false;
        _measurementCallback(getTemperature(), getHumidity());
        _millis = Helpers::getMillis();
    }
}

void AmbientSensor::setMeasurementCallback(const MeasurementCallback measurementCallback)
{
    _measurementCallback = measurementCallback;
}