#pragma once

#include "core.hpp"
#include "entities.hpp"

// clang-format off
namespace UI
{
inline std::function<EntityId(ECM &ecm, float, float, float, float)> getEntityConstructor(char c)
{
    switch (c)
    {
    case 'S':
        return playerScore;
    case 'L':
        return playerLives;
    }

    return NULL;
};

const std::vector<std::string_view> ui{
    " S                        L   ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ", 
    "                              ", 
    "                              ", 
    "                              ", 
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
};

const std::vector<std::string_view> gameOver{
    "                              ",
    "                              ",
    "    @@@@@ @@@@@ @   @ @@@@    ",
    "    @     @   @ @@ @@ @       ",
    "    @ @@@ @@@@@ @ @ @ @@@     ", 
    "    @   @ @   @ @   @ @       ",
    "    @@@@@ @   @ @   @ @@@@    ", 
    "                              ",
    "    @@@@@ @   @ @@@@@ @@@@    ", 
    "    @   @ @   @ @     @   @   ", 
    "    @   @  @ @  @@@@  @@@@    ",
    "    @   @  @ @  @     @@@     ", 
    "    @@@@@   @   @@@@@ @  @    ",
    "                              ",
    "                              ", 
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
};

inline std::vector<std::string_view> getUI(int _ui)
{
   switch(_ui) 
   {
    default:
        return ui;
   }
}

} // namespace UI
// clang-format on
