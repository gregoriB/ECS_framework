#pragma once

#include "../../core.hpp"
#include <string>

struct TestEffectCompTimed : public Tags::Effect
{
    Vector2 vec2{};
    int value{};

    TestEffectCompTimed(float duration) : Effect(duration)
    {
    }

    TestEffectCompTimed(int v) : value(v)
    {
    }

    TestEffectCompTimed()
    {
    }
};

struct TestEffectComp : public Tags::Effect
{
    int value{};
    Vector2 vec2{};

    TestEffectComp()
    {
    }
    TestEffectComp(int v) : value(v)
    {
    }
};

struct TestStackedComp : public Tags::Component
{
    int val{};
    Vector2 vec2{};
    std::string message{"this is a stacked component"};

    TestStackedComp()
    {
    }
    TestStackedComp(int v) : val(v)
    {
    }
};

struct TestUniqueComp : public Tags::Unique
{
    int val{};
    Vector2 vec2{};
    std::string message{"this is a unique component"};

    TestUniqueComp()
    {
    }
    TestUniqueComp(int v) : val(v)
    {
    }
};

struct TestTransformComp : public Tags::Transform
{
    Vector2 vec2{};
    std::string message{"this is a transform component"};
};

struct TestEventComp : public Tags::Event
{
    Vector2 vec2{};
    std::string message{"this is an event component"};
};

struct TestVelocityComponent
{
    float x{1.0F};
    float y{1.0F};
};

struct TestPositionComponent
{
    float x{0.0F};
    float y{0.0F};
};
