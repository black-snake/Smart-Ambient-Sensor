#include "WiFiManager.h"

#include <ESPAsync_WiFiManager.h>

void WiFiManager::printWiFiDetails()
{
    bool status = isConnected();

    Serial.print(F("WiFi connection status: "));
    Serial.println(status ? F("CONNECTED") : F("NOT CONNECTED"));

    if (status)
    {
        Serial.print(F("SSID: '"));
        Serial.print(WiFi.SSID());
        Serial.print(F("', RSSI="));
        Serial.print(WiFi.RSSI());
        Serial.print(F(", Channel: "));
        Serial.println(WiFi.channel());

        Serial.print(F("IP address: "));
        Serial.print(WiFi.localIP());
        Serial.print(F(", Gateway IP address: "));
        Serial.print(WiFi.gatewayIP());
        Serial.print(F(", DNS IP address: "));
        Serial.println(WiFi.dnsIP());
    }
}

WiFiManager::WiFiManager(const String &hostname, const WiFiConfig &wiFiConfig)
{
    this->hostname = hostname;
    WiFi.setHostname(this->hostname.c_str());

#ifdef ESP32
    if (wiFiConfig.ip.toString() == "0.0.0.0")
#else
    if (wiFiConfig.ip.isSet())
#endif
    {
        WiFi.config(wiFiConfig.ip, wiFiConfig.gateway, wiFiConfig.subnet, wiFiConfig.dns1, wiFiConfig.dns2);
    }

    for (WiFiCredential wiFiCredential : wiFiConfig.credentials)
    {
        if (wiFiCredential.ssid == NULL || wiFiCredential.ssid[0] == '\0')
        {
            Serial.println(F("Rejected WiFi credential because it was invalid."));
            continue;
        }

        bool result = _wifiMulti.addAP(wiFiCredential.ssid.c_str(), wiFiCredential.psk.c_str());

        if (result)
        {
            ++_noOfWiFiCredentials;

            Serial.print(F("SSID added for connection: "));
            Serial.println(wiFiCredential.ssid);
            Serial.print(F("PSK added for connection: "));
            Serial.println(wiFiCredential.psk);
        }
    }
}

WiFiManager::~WiFiManager()
{
    // pointers in _configParameters must NOT be freed because this is -- strangely -- handled by ESPAsync_WiFiManager!
    // although ESPAsync_WiFiManager did not create the objects, it "takes care" of their disposal
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiManager::tryAutoConnect(uint8_t maxNoOfTries)
{
    if (_noOfWiFiCredentials == 0)
    {
        Serial.println(F("WiFi auto connect called but no valid credentials provided."));
        return false;
    }

#ifdef ESP32
// For ESP32, this better be 0 to shorten the connect time.
// For ESP32-S2/C3, must be > 500
#if (USING_ESP32_S2 || USING_ESP32_C3)
#define WIFI_MULTI_1ST_CONNECT_WAITING_MS 500L
#else
// For ESP32 core v1.0.6, must be >= 500
#define WIFI_MULTI_1ST_CONNECT_WAITING_MS 800L
#endif
#else
// For ESP8266, this better be 2200 to enable connect the 1st time
#define WIFI_MULTI_1ST_CONNECT_WAITING_MS 2200L
#endif

#define WIFI_MULTI_CONNECT_WAITING_MS 500L

    Serial.println(F("Trying WiFi auto connect."));

    int i = 0;
    auto status = _wifiMulti.run();
    delay(WIFI_MULTI_1ST_CONNECT_WAITING_MS);

    while ((i++ < maxNoOfTries) && (status != WL_CONNECTED))
    {
        status = WiFi.status();

        if (status == WL_CONNECTED)
            break;
        else
            delay(WIFI_MULTI_CONNECT_WAITING_MS);
    }

    printWiFiDetails();

    _lastWiFiCredential.ssid = String(WiFi.SSID());
    _lastWiFiCredential.psk = String(WiFi.psk());

    return status == WL_CONNECTED;
}

bool WiFiManager::reconnect(uint8_t maxNoOfTries)
{
    Serial.println(F("Trying connect to WiFi."));

    if (isConnected())
    {
        Serial.println(F("WiFi is already connected."));
        return true;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(_lastWiFiCredential.ssid.c_str(), _lastWiFiCredential.psk.c_str());

    uint8_t noOfTries = 0;
    uint16_t delayMs = 2500;
    do
    {
        ++noOfTries;
        Serial.print(F("Trying to connect to WiFi, #"));
        Serial.print(noOfTries);
        Serial.print(F(" of "));
        Serial.println(maxNoOfTries);

        if (isConnected())
        {
            break;
        }

        if (noOfTries != maxNoOfTries)
        {
            delayMs *= 2;
            Serial.print(F("Trying to reconnect to WiFi in "));
            Serial.print(delayMs / 1000.f);
            Serial.println(F(" s"));
            delay(delayMs);
        }
    } while (!isConnected() && noOfTries < maxNoOfTries);

    bool result = isConnected();

    if (result)
    {
        Serial.println(F("Successfully connected to WiFi."));
    }
    else
    {
        Serial.println(F("Failed to connect to WiFi."));
    }

    return result;
}

bool WiFiManager::disconnect()
{
    Serial.println(F("WiFi requested to disconnect."));
    return WiFi.disconnect();
}

void WiFiManager::addConfigParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *customHtml, LabelPlacement labelPlacement)
{
    ESPAsync_WMParameter *configParameter = new ESPAsync_WMParameter(id, placeholder, defaultValue, length + 1, customHtml, labelPlacement);
    _configParameters.push_back(configParameter);
}

void WiFiManager::setConfigPortalCallback(const ConfigPortalCallback configPortalCallback)
{
    _configPortalCallback = configPortalCallback;
}

bool WiFiManager::startConfigPortal(const char *apSsid, unsigned long timeoutSeconds)
{
    AsyncWebServer webServer(80);
    DNSServer dnsServer;
    ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, &dnsServer, hostname.c_str());

    ESPAsync_wifiManager.setDebugOutput(true);
    ESPAsync_wifiManager.setConfigPortalTimeout(timeoutSeconds);
    ESPAsync_wifiManager.setConnectTimeout(30);
    ESPAsync_wifiManager.setConfigPortalChannel(0); // 0 = random

    for (ESPAsync_WMParameter *configParameter : _configParameters)
    {
        ESPAsync_wifiManager.addParameter(configParameter);
    }

    bool result = ESPAsync_wifiManager.startConfigPortal(apSsid);

    printWiFiDetails();

    if (_configPortalCallback == nullptr)
    {
        return result;
    }

    int parametersCount = ESPAsync_wifiManager.getParametersCount();
    std::map<const char *, const char *> configParameters{};

    ESPAsync_WMParameter **parameters = ESPAsync_wifiManager.getParameters();

    for (int i = 0; i < parametersCount; i++)
    {
        const char *key = parameters[i]->getID();
        configParameters[key] = parameters[i]->getValue();
    }

    WiFiConfig wiFiConfig;

    WiFi_STA_IPConfig staConfig;
    ESPAsync_wifiManager.getSTAStaticIPConfig(staConfig);
    wiFiConfig.ip = staConfig._sta_static_ip;
    wiFiConfig.gateway = staConfig._sta_static_gw;
    wiFiConfig.subnet = staConfig._sta_static_sn;
    wiFiConfig.dns1 = staConfig._sta_static_dns1;
    wiFiConfig.dns2 = staConfig._sta_static_dns2;

    wiFiConfig.credentials.push_back(WiFiCredential(ESPAsync_wifiManager.getSSID(), ESPAsync_wifiManager.getPW()));
    wiFiConfig.credentials.push_back(WiFiCredential(ESPAsync_wifiManager.getSSID1(), ESPAsync_wifiManager.getPW1()));

    _configPortalCallback(result, wiFiConfig, configParameters);

    return result;
}