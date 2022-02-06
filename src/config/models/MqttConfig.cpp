#include "MqttConfig.h"

MqttConfig::MqttConfig()
{
}

const char *MqttConfig::getFileName()
{
    return "mqtt.json";
}

String MqttConfig::serialize()
{
    StaticJsonDocument<512> doc;

    doc["host"] = host;
    doc["port"] = port;
    doc["clientId"] = clientId;
    doc["baseTopic"] = baseTopic;
    doc["username"] = username;
    doc["password"] = password;

    String output;
    serializeJson(doc, output);
    return output;
}

MqttConfig MqttConfig::deserialize(const char *content)
{
    StaticJsonDocument<512> doc;

    DeserializationError error = deserializeJson(doc, content);

    if (error)
    {
        Serial.print(F("Error: deserialization of MQTT config failed: "));
        Serial.println(error.f_str());
    }

    MqttConfig mqttConfig;
    mqttConfig.host = doc["host"].as<String>();
    mqttConfig.port = doc["port"];
    mqttConfig.clientId = doc["clientId"].as<String>();
    mqttConfig.baseTopic = doc["baseTopic"].as<String>();
    mqttConfig.username = doc["username"].as<String>();
    mqttConfig.password = doc["password"].as<String>();

    return mqttConfig;
}