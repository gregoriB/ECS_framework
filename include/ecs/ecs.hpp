#pragma once

#include "../../src/components.hpp"
#include "../../src/entity_component_manager.hpp"
#include "../../src/sparse_set.hpp"
#include "../../src/tags.hpp"

namespace ECS
{
template <typename EntityId, typename T> using Set = internal::SparseSet<EntityId, T>;

template <typename EntityId> using Manager = internal::EntityComponentManager<EntityId>;

template <typename EntityId, typename T> using Group = internal::Grouping<EntityId, T>;

template <typename T> using Components = internal::ComponentsWrapper<T>;
} // namespace ECS
