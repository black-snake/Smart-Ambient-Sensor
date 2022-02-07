#ifndef WiFiConfig_h
#define WiFiConfig_h

#include "Config.h"
#include "IPAddress.h"

// TODO: implement static IP configuration
struct WiFiCredential
{
public:
    WiFiCredential();
    WiFiCredential(const String &ssid, const String &psk);

    String ssid;
    String psk;
};

struct WiFiConfig : public Config
{
public:
    WiFiConfig();

    std::vector<WiFiCredential> credentials;

    IPAddress ip;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns1;
    IPAddress dns2;

    const char *getFileName();
    String serialize();
    static WiFiConfig deserialize(const char *content);
};

#endif