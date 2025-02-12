#pragma once

#include "../../src/entity_component_manager.hpp"
#include "../../src/tags.hpp"
#include "../../src/utilities.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <stack>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

using EntityId = uint32_t;
using EId = EntityId;
using ECM = EntityComponentManager<EntityId>;

constexpr bool BREAK = false;
constexpr bool CONTINUE = true;

enum class Inputs
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SHOOT,
    MENU,
    QUIT,
};

struct ScreenConfig
{
    int width = 640;
    int height = 480;
};

inline const int SCREEN_FPS = 300000;
inline const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

class Vector2
{
  public:
    float x{}, y{};

    Vector2()
    {
    }
    Vector2(float _x, float _y) : x(_x), y(_y)
    {
    }
};

class Bounds
{
  public:
    Vector2 position;
    Vector2 size;

    Bounds()
    {
    }
    Bounds(Vector2 _position, Vector2 _size) : position(_position), size(_size)
    {
    }
    Bounds(float _x, float _y, float _w, float _h) : position(_x, _y), size(_w, _h)
    {
    }

    const std::array<float, 4> box() const
    {
        return {position.x, position.y, position.x + size.x, position.y + size.y};
    }

    const std::array<float, 4> get() const
    {
        return {position.x, position.y, size.x, size.y};
    }
};
