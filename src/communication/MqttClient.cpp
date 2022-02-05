#include "MqttClient.h"

MqttClient::MqttClient(MqttConfig mqttConfig) : _pubSubClient(_wiFiClient), mqttConfig(mqttConfig)
{
    if (!mqttConfig.baseTopic.endsWith("/"))
    {
        mqttConfig.baseTopic += "/";
    }
}

MqttClient::~MqttClient()
{
}

bool MqttClient::connect(uint8_t maxNoOfTries)
{
    if (isConnected())
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
    uint16_t delayMs = 2500;
    do
    {
        ++noOfTries;
        Serial.print(F("MQTT client tries to connect, #"));
        Serial.print(noOfTries);
        Serial.print(F(" of "));
        Serial.println(maxNoOfTries);

        if (_pubSubClient.connect(mqttConfig.clientId.c_str(), mqttConfig.username.c_str(), mqttConfig.password.c_str()))
        {
            break;
        }

        if (noOfTries != maxNoOfTries)
        {
            delayMs *= 2;
            Serial.print(F("MQTT client tries to reconnect in "));
            Serial.print(delayMs / 1000.f);
            Serial.println(F(" s"));
            delay(delayMs);
        }
    } while (!isConnected() && noOfTries < maxNoOfTries);

    bool result = isConnected();

    if (result)
    {
        Serial.print(F("MQTT client successfully connected to: "));
        Serial.println(mqttConfig.host);
    }
    else
    {
        Serial.print(F("MQTT client failed to connected to '"));
        Serial.print(mqttConfig.host);
        Serial.print(F("', status code from library: "));
        Serial.println(_pubSubClient.state());
    }

    return result;
}

bool MqttClient::disconnect()
{
    Serial.println(F("MQTT client requested to disconnect."));
    _pubSubClient.disconnect();

    return !isConnected();
}

bool MqttClient::isConnected()
{
    return _pubSubClient.connected();
}

bool MqttClient::publish(const String &subtopic, const String &message)
{
    String topic = mqttConfig.baseTopic + subtopic;

    Serial.print(F("MQTT client publishes in topic '"));
    Serial.print(topic);
    Serial.print(F("' the following message: "));
    Serial.println(message);

    bool result = _pubSubClient.publish(topic.c_str(), message.c_str(), true);

    Serial.println(result ? F("MQTT client successfully published the message.") : F("MQTT client failed to publish the message."));

    return result;
}