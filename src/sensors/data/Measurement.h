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

    String serialize();
};

template <class T>
Measurement<T>::Measurement()
{
}

template <class T>
String Measurement<T>::serialize()
{
    StaticJsonDocument<96> doc;

    doc["quantity"] = quantity;
    doc["value"] = value;
    doc["unit"] = unit;

    String output;
    serializeJson(doc, output);
    return output;
}

#endif