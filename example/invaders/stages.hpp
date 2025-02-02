#pragma once

#include "core.hpp"
#include "entities.hpp"

// clang-format off
namespace Stage1
{
inline std::function<EntityId(ECM &ecm, float, float, float, float)> getEntityConstructor(char c)
{
    switch (c)
    {
    case 'P':
        return player;
    case 'S':
        return hiveAlienSmall;
    case 'M':
        return hiveAlienMedium;
    case 'L':
        return hiveAlienLarge;
    }

    return NULL;
};

const std::vector<std::string_view> stage{
    "                            ",
    "                            ",
    "    S S S S S S S S S S S   ", 
    "                            ",
    "    M M M M M M M M M M M   ", 
    "                            ",
    "    M M M M M M M M M M M   ",
    "                            ",
    "    L L L L L L L L L L L   ", 
    "                            ",
    "    L L L L L L L L L L L   ", 
    "                            ",
    "                            ", 
    "                            ", 
    "                            ",
    "                            ", 
    "                            ",
    "                            ",
    "             P              ", 
    "                            ",
    "                            ",
    "                            ",
};

const std::vector<std::string_view> testStage1{
    "                            ", 
    "                            ", 
    "                            ",
    "                            ", 
    "                            ", 
    "                            ",
    "             P              ", 
    "                            ", 
    "                            ",
    "                            ", 
    "                            ", 
    "                            ",
};

const std::vector<std::string_view> testStage2{
    "                            ", 
    "                          S ", 
    "                            ",
    "                            ", 
    "                            ", 
    "                            ",
    "             P              ", 
    "                            ", 
    "                            ",
    "                            ", 
    "                            ", 
    "                            ",
};
} // namespace Stage1
// clang-format on
