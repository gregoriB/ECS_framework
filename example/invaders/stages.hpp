#pragma once

#include "core.hpp"
#include "entities.hpp"

namespace Stage1
{
inline std::function<void(ECM &ecm, float, float, float, float)> getEntityConstructor(char c)
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
    "    A A A A A A A A A A A   ", "    A A A A A A A A A A A   ", "    A A A A A A A A A A A   ",
    "    A A A A A A A A A A A   ", "    A A A A A A A A A A A   ", "    A A A A A A A A A A A   ",
    "                            ", "                            ", "                            ",
    "                            ", "             P              ", "                            ",
};
} // namespace Stage1
