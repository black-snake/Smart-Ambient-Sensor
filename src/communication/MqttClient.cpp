#include "MqttClient.h"

MqttClient::MqttClient(MqttConfig mqttConfig) : _pubSubClient(_wiFiClient), mqttConfig(mqttConfig)
{
}

MqttClient::~MqttClient()
{
}

bool MqttClient::connect(uint8_t maxNoOfTries)
{
    if (_pubSubClient.connected())
    {
        Serial.println(F("MQTT client is already connected."));
        return true;
    }

#ifdef ESP32
    if (!_ipAddress.fromString(mqttConfig.host) && WiFiGenericClass::hostByName(mqttConfig.host.c_str(), _ipAddress) == 0)
#else
    if (!_ipAddress.fromString(mqttConfig.host) && WiFi.hostByName(mqttConfig.host.c_str(), _ipAddress) == 0)
#endif
    {
        Serial.print(F("MQTT host neither seems to be a FQDN, nor an IP address: "));
        Serial.println(mqttConfig.host);
        return false;
    }

    Serial.print(F("MQTT client retrieved the following IP address for the MQTT host: "));
    Serial.println(_ipAddress.toString());
    _pubSubClient.setServer(_ipAddress, mqttConfig.port);

    uint8_t noOfTries = 0;
    do
    {
        ++noOfTries;
        Serial.print(F("MQTT client tries to connect, #"));
        Serial.print(noOfTries);
        Serial.print(F(" of "));
        Serial.println(maxNoOfTries);

        if (_pubSubClient.connect(mqttConfig.clientId.c_str(), mqttConfig.username.c_str(), mqttConfig.password.c_str()))
        {
            Serial.print(F("MQTT client successfully connected to: "));
            Serial.println(mqttConfig.host);
            return true;
        }
        else
        {
            Serial.print(F("MQTT client failed to connected to '"));
            Serial.print(mqttConfig.host);
            Serial.print(F("', status code from library: "));
            Serial.println(_pubSubClient.state());
        }

        uint16_t delayMs = 5000;
        Serial.print(F("MQTT client tries to reconnect in "));
        Serial.print(delayMs / 1000);
        Serial.println(F(" s"));
        delay(delayMs);
    } while (!isConnected() && noOfTries < maxNoOfTries);

    return isConnected();
}

bool MqttClient::disconnect()
{
    Serial.println(F("MQTT client requested to disconnect."));
    _pubSubClient.disconnect();

    return !isConnected();
}

bool MqttClient::isConnected()
{
    bool result = _pubSubClient.connected();

    Serial.println(result ? F("MQTT client is connected.") : F("MQTT client is disconnected."));

    return result;
}

bool MqttClient::publish(const char *message)
{
    Serial.print(F("MQTT client publishes in topic '"));
    Serial.print(mqttConfig.topic);
    Serial.print(F("' the following message: "));
    Serial.println(message);

    bool result = _pubSubClient.publish(mqttConfig.topic.c_str(), message, true);

    Serial.println(result ? F("MQTT client successfully published the message.") : F("MQTT client failed to publish the message."));

    return result;
}