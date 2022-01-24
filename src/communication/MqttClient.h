#ifndef MqttClient_h
#define MqttClient_h

#include "config/models/MqttConfig.h"

#ifdef ESP32
#include <WiFiGeneric.h>
#include <WiFiClient.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <PubSubClient.h>

class MqttClient
{
private:
    WiFiClient _wiFiClient;
    PubSubClient _pubSubClient;
    IPAddress _ipAddress;

public:
    MqttClient(MqttConfig mqttConfig);
    ~MqttClient();

    const MqttConfig mqttConfig;

    bool connect(uint8_t maxNoOfTries = 15);
    bool disconnect();
    bool isConnected();
    bool publish(const char *message);
};

#endif