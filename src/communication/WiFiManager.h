#ifndef WiFiManager_h
#define WiFiManager_h

#include "config/models/WiFiConfig.h"

#ifdef ESP32
#include <esp_wifi.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiMulti.h>
#else
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WiFiMulti.h>
#endif

#define USE_AVAILABLE_PAGES false
#define USE_ESP_WIFIMANAGER_NTP false
#define _ESPASYNC_WIFIMGR_LOGLEVEL_ 4

#include <ESPAsync_WiFiManager.hpp>

// TODO: implement static IP configuration
class WiFiManager
{
private:
#ifdef ESP32
    WiFiMulti _wifiMulti;
#else
    ESP8266WiFiMulti _wifiMulti;
#endif

    uint8_t _noOfWiFiCredentials = 0;
    std::vector<ESPAsync_WMParameter *> _configParameters = {};

    using ConfigPortalCallback = void (*)(bool success, WiFiConfig &wiFiConfig, std::map<const char *, const char *> &configParameters);
    ConfigPortalCallback _configPortalCallback = nullptr;

    WiFiCredential _lastWiFiCredential;

    void printWiFiDetails();

public:
    enum LabelPlacement
    {
        None = 0,
        Before = 1,
        After = 2
    };

    WiFiManager(const String &hostname, const WiFiConfig &wiFiConfig);
    ~WiFiManager();

    String hostname;

    bool isConnected();
    bool tryAutoConnect(uint8_t maxNoOfTries = 20);
    bool reconnect(uint8_t maxNoOfTries = 15);
    bool disconnect();

    void addConfigParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *customHtml = "", LabelPlacement labelPlacement = Before);
    void setConfigPortalCallback(const ConfigPortalCallback configPortalCallback);
    bool startConfigPortal(const char *apSsid, unsigned long timeoutSeconds = 900);
};

#endif