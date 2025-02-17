#pragma once

#include "../../src/components.hpp"
#include "../../src/entity_component_manager.hpp"
#include "../../src/sparse_set.hpp"
#include "../../src/tags.hpp"

namespace ECS
{
/**
 * A sparse set for storing components of the same type.
 */
template <typename EntityId, typename T> using Set = internal::SparseSet<EntityId, T>;

/**
 * The main entry point into the ECS.  Used to add, query, and remove components and component
 * sets, as well as perform other operations.
 */
template <typename EntityId> using Manager = internal::EntityComponentManager<EntityId>;

/**
 * A grouping of entities which have all of the specified components.
 */
template <typename EntityId, typename T> using Group = internal::Grouping<EntityId, T>;

/**
 * A wrapper for a component of the specific type.  The wrapper controls how the component is
 * arranged and provides access methods for the component data.
 */
template <typename T> using Components = internal::ComponentsWrapper<T>;
} // namespace ECS

#undef ECS_LOG_WARNING
#undef ECS_ASSERT
