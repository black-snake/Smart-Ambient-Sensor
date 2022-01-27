#include "ResetConfig.h"

ResetConfig::ResetConfig()
{
}

const char *ResetConfig::getFileName()
{
    return "reset.json";
}

String ResetConfig::serialize()
{
    return F("{}");
}

ResetConfig ResetConfig::deserialize(const char *content)
{
    ResetConfig resetConfig;
    return resetConfig;
}