#ifndef ResetConfig_h
#define ResetConfig_h

#include "Config.h"

struct ResetConfig : public Config
{
public:
    ResetConfig();

    const char *getFileName();
    String serialize();
    static ResetConfig deserialize(const char *content);
};

#endif