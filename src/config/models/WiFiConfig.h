#ifndef WiFiConfig_h
#define WiFiConfig_h

#include "Config.h"

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

    const char *getFileName();
    String serialize();
    static WiFiConfig deserialize(const char *content);
};

#endif