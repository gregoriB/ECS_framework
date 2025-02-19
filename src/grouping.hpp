#pragma once

#include "macros.hpp"
#include "utilities.hpp"

namespace ECS
{
namespace internal
{

/**
 * @brief A grouping of entities which have all of the specified components.
 */
template <typename EntityId, typename... Ts> class Grouping
{
  private:
    std::vector<EntityId> m_ids;
    std::tuple<Ts *...> m_values;

  public:
    Grouping(std::vector<EntityId> _ids = {}) {};
    Grouping(std::vector<EntityId> _ids, std::tuple<Ts *...> _values) : m_ids(_ids), m_values(_values) {};

    /**
     * @brief Iterate over component set and pass the entity components into the function
     *
     * The function argument can optionally return a bool to determine the loop-breaking behavior.
     * A false return value is a break.
     *
     * @param Function which accepts the entity id and component types
     */
    template <typename Func> void each(Func &&fn)
    {
        if constexpr (Utilities::ReturnsBool<Func, EntityId, Ts... >)
            eachWithBreak(fn);
        else
            eachNoBreak(fn);

#ifdef ecs_allow_experimental
        m_filterFn.reset();
#endif
    }

    /**
     * @brief Get the number of entities which share all specified component types
     *
     * @return size_t
     */
    [[nodiscard]] size_t size() const
    {
        return m_ids.size();
    }

    /**
     * @brief Evaluate by number of entities
     *
     * @return size_t
     */
    [[nodiscard]] explicit operator bool() const
    {
        return !!size();
    }

    /**
     * @brief Get a const reference of the entity ids
     *
     * @return const std::vector<EntityId>
     */
    [[nodiscard]] const std::vector<EntityId> &getIds() const
    {
        return m_ids;
    }

private:
    template <typename Func> void eachWithBreak(Func &&fn)
    {
        for (const auto &id : m_ids)
        {
#ifdef ecs_allow_experimental
            if (!passesFilter(id))
                continue;
#endif
            // TODO Safety : Add null set check and handling
            if (!fn(id, *std::get<Ts *>(m_values)->get(id)...))
                break;
        }

    }

    template <typename Func> void eachNoBreak(Func &&fn)
    {
        for (const auto &id : m_ids)
        {
#ifdef ecs_allow_experimental
            if (!passesFilter(id))
                continue;
#endif
            // TODO Safety : Add null set check and handling
            fn(id, *std::get<Ts *>(m_values)->get(id)...);
        }
    }

#ifdef ecs_allow_experimental
  private:
    std::optional<std::function<bool(EntityId)>> m_filterFn;

    bool passesFilter(EntityId id)
    {
        if (!m_filterFn.has_value())
            return true;

        return m_filterFn.value()(id);
    }

  public:
    template <typename Func> Grouping<EntityId, Ts...> &select(Func &&fn)
    {
        m_filterFn = fn;
        return (*this);
    }
#endif
};

}
};
