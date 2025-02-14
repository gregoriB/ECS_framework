#pragma once

#include "../core.hpp"

struct TestEffectCompTimed : Effect, Stack
{
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

struct TestEffectComp : Effect, Stack
{
    int value{};

    TestEffectComp()
    {
    }
    TestEffectComp(int v) : value(v)
    {
    }
};

struct TestStackedComp : Stack
{
    int val{};
    std::string message{"this is a stacked component"};

    TestStackedComp()
    {
    }
    TestStackedComp(int v) : val(v)
    {
    }
};

struct TestNonStackedComp : public NoStack
{
    int val{};
    std::string message{"this is a non-stacked component"};

    TestNonStackedComp()
    {
    }
    TestNonStackedComp(int v) : val(v)
    {
    }
};

struct TestTransformComp : public Transform
{
    std::string message{"this is a transform component"};
};

struct TestEventComp : public Event
{
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
