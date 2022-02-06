#include "WiFiConfig.h"

WiFiCredential::WiFiCredential()
{
}

WiFiCredential::WiFiCredential(const String &ssid, const String &psk)
{
    this->ssid = ssid;
    this->psk = psk;
}

WiFiConfig::WiFiConfig()
{
    credentials.reserve(2);
}

const char *WiFiConfig::getFileName()
{
    return "wifi.json";
}

String WiFiConfig::serialize()
{
    StaticJsonDocument<512> doc;

    JsonArray credentials = doc.createNestedArray("credentials");

    for (WiFiCredential credential : this->credentials)
    {
        JsonObject obj = credentials.createNestedObject();
        obj["ssid"] = credential.ssid;
        obj["psk"] = credential.psk;
    }

    String output;
    serializeJson(doc, output);
    return output;
}

WiFiConfig WiFiConfig::deserialize(const char *content)
{
    StaticJsonDocument<512> doc;

    DeserializationError error = deserializeJson(doc, content);

    if (error)
    {
        Serial.print(F("Error: deserialization of WiFi config failed: "));
        Serial.println(error.f_str());
    }

    WiFiConfig wiFiConfig;

    for (JsonObject obj : doc["credentials"].as<JsonArray>())
    {
        WiFiCredential credential;
        credential.ssid = obj["ssid"].as<String>();
        credential.psk = obj["psk"].as<String>();
        wiFiConfig.credentials.push_back(credential);
    }

    return wiFiConfig;
}