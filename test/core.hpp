#pragma once

#include <cassert>
#include <cstdint>
#include <string_view>

#include <ecs/ecs.hpp>

using EntityId = uint32_t;
/* using ECM = struct Testing{}; */
using ECM = ECS::EntityComponentManager<EntityId>;
using EId = EntityId;

using Effect = ECS::Tags::Effect;
using Stack = ECS::Tags::Stack;
using NoStack = ECS::Tags::NoStack;
using Event = ECS::Tags::Event;
using Transform = ECS::Tags::Transform;
