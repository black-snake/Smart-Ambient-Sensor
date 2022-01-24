#ifndef MqttConfig_h
#define MqttConfig_h

#include "Config.h"

struct MqttConfig : public Config
{
public:
    MqttConfig();

    String host;
    uint16_t port = 1883;
    String clientId;
    String topic;
    String username;
    String password;

    const char *getFileName();
    String serialize();
    static MqttConfig deserialize(const char *content);
};

#endif