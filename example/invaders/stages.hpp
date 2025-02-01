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
    case 'A':
        return alien;
    }

    return NULL;
};

const std::vector<std::string_view> stage{
    "                            ",
    "                            ",
    "    A A A A A A A A A A A   ", 
    "                            ",
    "    A A A A A A A A A A A   ", 
    "                            ",
    "    A A A A A A A A A A A   ",
    "                            ",
    "    A A A A A A A A A A A   ", 
    "                            ",
    "    A A A A A A A A A A A   ", 
    "                            ",
    "    A A A A A A A A A A A   ",
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
    "                          R ", 
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
