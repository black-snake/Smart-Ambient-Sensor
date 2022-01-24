#ifndef Measurement_h
#define Measurement_h

#include <Arduino.h>
#include <ArduinoJson.h>

template <class T>
struct Measurement
{
public:
    Measurement();

    String quantity;
    T value;
    String unit;

    JsonObject populateJsonObject(JsonObject &jsonObject);
};

template <class T>
Measurement<T>::Measurement()
{
}

template <class T>
JsonObject Measurement<T>::populateJsonObject(JsonObject &jsonObject)
{
    jsonObject["quantity"] = quantity;
    jsonObject["value"] = value;
    jsonObject["unit"] = unit;

    return jsonObject;
}

#endif