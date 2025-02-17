#pragma once

#include <cassert>
#include <cstdint>
#include <string_view>

#include <ecs/ecs.hpp>

using EntityId = uint32_t;
using ComponentManager = ECS::Manager<EntityId>;
using CM = ComponentManager;
using EId = EntityId;

using Effect = ECS::Tags::Effect;
using Stack = ECS::Tags::Stack;
using NoStack = ECS::Tags::NoStack;
using Event = ECS::Tags::Event;
using Transform = ECS::Tags::Transform;

#define PRINT(...) ECS::internal::Utilities::print(__VA_ARGS__);
