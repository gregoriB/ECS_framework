#pragma once

#include "core.hpp"
#include "entities.hpp"

// clang-format off
namespace Stages
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
    case 'H':
        return hive;
    case '@':
        return redBlock;
    case '!':
        return startBlock;
    case '#':
        return greenBlock;
    case '%':
        return titleBlockSm;
    case '&':
        return titleBlock;
    }

    return NULL;
};

const std::vector<std::string_view> titlePage{
    "                               ",
    "                               ",
    "   &&&  &     &&   &&& &  &    ", 
    "   &  & &    &  & &    & &     ",
    "   &&&  &    &  & &    &&      ", 
    "   &  & &    &  & &    & &     ",
    "   &&&  &&&&  &&   &&& &  &    ",
    "                               ",
    "  & &  %&   % & && &&%&&  &&&  ", 
    "  & && %&   %& %& %&& & % &&   ",
    "  & & &% & & &&%& %&  &&    &  ", 
    "  & &  %  &  & %&& &&%& % &&&  ",
    "                               ", 
    "                               ", 
    "                               ",
    "               !               ", 
    "                               ",
    "                               ",
    "                               ", 
    "                               ",
    "                               ",
    "               P               ",
};

const std::vector<std::string_view> stage1{
    "  H                           ",
    "                              ",
    "     S S S S S S S S S S S    ", 
    "                              ",
    "     M M M M M M M M M M M    ", 
    "                              ",
    "     M M M M M M M M M M M    ",
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "                              ", 
    "                              ", 
    "                              ", 
    "                              ", 
    "                              ",
    "  ####    ####   ####   ####  ", 
    "  #  #    #  #   #  #   #  #  ",
    "                              ",
    "                              ", 
    "                              ",
};

const std::vector<std::string_view> stage2{
    " H                            ",
    "                              ",
    "                              ",
    "     S S S S S S S S S S S    ", 
    "                              ",
    "     M M M M M M M M M M M    ", 
    "                              ",
    "     M M M M M M M M M M M    ",
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ", 
    "                              ", 
    "                              ", 
    "                              ", 
    "                              ",
    "  ####    ####   ####   ####  ", 
    "  #  #    #  #   #  #   #  #  ",
    "                              ",
    "                              ", 
    "                              ",
};

const std::vector<std::string_view> stage3{
    " H                            ",
    "                              ",
    "                              ",
    "                              ",
    "     S S S S S S S S S S S    ", 
    "                              ",
    "     M M M M M M M M M M M    ", 
    "                              ",
    "     M M M M M M M M M M M    ",
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ", 
    "                              ", 
    "                              ", 
    "                              ", 
    "  ####    ####   ####   ####  ", 
    "  #  #    #  #   #  #   #  #  ",
    "                              ",
    "                              ", 
    "                              ",
};

const std::vector<std::string_view> stage4{
    " H                            ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "     S S S S S S S S S S S    ", 
    "                              ",
    "     M M M M M M M M M M M    ", 
    "                              ",
    "     M M M M M M M M M M M    ",
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ", 
    "                              ", 
    "                              ", 
    "  ####    ####   ####   ####  ", 
    "  #  #    #  #   #  #   #  #  ",
    "                              ",
    "                              ", 
    "                              ",
};

const std::vector<std::string_view> stage5{
    " H                            ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "     S S S S S S S S S S S    ", 
    "                              ",
    "     M M M M M M M M M M M    ", 
    "                              ",
    "     M M M M M M M M M M M    ",
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ", 
    "                              ",
    "  ####    ####   ####   ####  ", 
    "  #  #    #  #   #  #   #  #  ",
    "                              ",
    "                              ", 
    "                              ",
};

const std::vector<std::string_view> stage6{
    " H                            ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "     S S S S S S S S S S S    ", 
    "                              ",
    "     M M M M M M M M M M M    ", 
    "                              ",
    "     M M M M M M M M M M M    ",
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "  ####    ####   ####   ####  ", 
    "  #  #    #  #   #  #   #  #  ",
    "                              ",
    "                              ", 
    "                              ",
};

const std::vector<std::string_view> stage7{
    " H                            ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "                              ",
    "     S S S S S S S S S S S    ", 
    "                              ",
    "     M M M M M M M M M M M    ", 
    "                              ",
    "     M M M M M M M M M M M    ",
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "                              ", 
    "                              ",
    "                              ",
    "                              ",
};

const std::vector<std::string_view> stage8{
    " H                             ",
    "                               ",
    "                               ",
    "                               ",
    "                               ",
    "                               ",
    "                               ",
    "                               ",
    "                               ",
    "     S S S S S S S S S S S     ", 
    "                               ",
    "     M M M M M M M M M M M     ", 
    "                               ",
    "     M M M M M M M M M M M     ",
    "                               ",
    "     L L L L L L L L L L L     ", 
    "                               ",
    "     L L L L L L L L L L L     ", 
    "                               ", 
    "                               ",
    "                               ",
    "                               ",
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
    " H                          ",
    "                            ",
    "                          S ", 
    "                            ",
    "                            ", 
    "                            ",
    "                            ",
    "                            ",
    "                            ", 
    "                            ",
    "                            ", 
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

const std::vector<std::string_view> gameOver{
    "                              ",
    "                              ",
    "    @@@@@ @@@@@ @   @ @@@@    ",
    "    @     @   @ @@ @@ @       ",
    "    @ @@@ @@@@@ @ @ @ @@@     ", 
    "    @   @ @   @ @   @ @       ",
    "    @@@@@ @   @ @   @ @@@@    ", 
    "                              ",
    "    @@@@@ @   @ @@@@@ @@@     ", 
    "    @   @ @   @ @     @  @    ", 
    "    @   @  @ @  @@@@  @@@     ",
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

const std::vector<std::string_view> test1{
    "  H                           ",
    "                              ",
    "     S S S S S S S S S S S    ", 
    "                              ",
    "     M M M M M M M M M M M    ", 
    "                              ",
    "     M M M M M M M M M M M    ",
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "     L L L L L L L L L L L    ", 
    "                              ",
    "                              ", 
    "                              ", 
    "                              ", 
    "                              ", 
    "                              ",
    "  ####    ####   ####   ####  ", 
    "  #  #    #  #   #  #   #  #  ",
    "                              ",
    "               P              ", 
    "                              ",
};

inline std::vector<std::string_view> getStage(int stage)
{
   switch(stage) 
   {
    case 1:
        return stage1;
    case 2:
        return stage2;
    case 3:
        return stage3;
    case 4:
        return stage4;
    case 5:
        return stage5;
    case 999:
        return titlePage;
    case -1:
        return test1;
    default:
        return gameOver;
   }
}

} // namespace Stage1
// clang-format on
