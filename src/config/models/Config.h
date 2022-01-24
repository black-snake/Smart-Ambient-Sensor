#ifndef Config_h
#define Config_h

#include <Arduino.h>
#include <ArduinoJson.h>

struct Config
{
public:
    const char *getFileName();
    virtual String serialize();
    template <class T>
    static T deserialize(const char *content);
};

template <class T>
T Config::deserialize(const char *content)
{
    return T::deserialize(content);
}

#endif