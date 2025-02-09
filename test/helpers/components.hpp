#pragma once

#include "../../example/invaders/core.hpp"
#include <string>

struct TestEffectCompTimed : Tags::Effect, Tags::Stack
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

struct TestEffectComp : Tags::Effect, Tags::Stack
{
    Vector2 vec2{};
    int value{};

    TestEffectComp()
    {
    }
    TestEffectComp(int v) : value(v)
    {
    }
};

struct TestStackedComp : Tags::Stack
{
    Vector2 vec2{};
    int val{};
    std::string message{"this is a stacked component"};

    TestStackedComp()
    {
    }
    TestStackedComp(int v) : val(v)
    {
    }
};

struct TestNonStackedComp : public Tags::NoStack
{
    int val{};
    Vector2 vec2{};
    std::string message{"this is a non-stacked component"};

    TestNonStackedComp()
    {
    }
    TestNonStackedComp(int v) : val(v)
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
    float x{1.0f};
    float y{1.0f};
};

struct TestPositionComponent
{
    float x{0.0f};
    float y{0.0f};
};
