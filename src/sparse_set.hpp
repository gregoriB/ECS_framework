#pragma once

#include "components.hpp"
#include "core.hpp"
#include "utilities.hpp"

template <typename Id, typename T> class BaseSparseSet
{
  protected:
    using EachFn = std::function<void(Id, T &)>;
    using BreakableEachFn = std::function<bool(Id, T &)>;

  public:
    template <typename EntityId> friend class EntityComponentManager;
    virtual ~BaseSparseSet() = default;

    virtual void erase(Id id) = 0;
    virtual size_t size() const = 0;

    template <typename Func> void each(Func fn)
    {
    }

  private:
    void eachWithEmpty(EachFn fn)
    {
    }

    virtual void prune()
    {
    }
};

template <typename Id, typename T> class SparseSet : public BaseSparseSet<Id, Components<DefaultComponent>>
{
  public:
    template <typename EntityId> friend class EntityComponentManager;
    template <typename EntityId, typename... Ts> friend class Grouping;

    explicit SparseSet(size_t _initialSize, size_t _resize) : m_resize(_resize)
    {
        m_pointers.resize(_initialSize, -1);
        m_values.reserve(_initialSize);
        m_ids.reserve(_initialSize);
    }

    explicit operator bool() const
    {
        return size() > 0;
    }

    [[nodiscard]] const std::vector<Id> &getIds()
    {

#ifndef ecs_disable_auto_prune
        prune();
#endif
        return m_ids;
    }

    /**
     * @brief Basic each loop
     *
     * The function argument can optionally return a bool to determine the loop-breaking behavior.
     * A false return value is a break.
     *
     * The loop will skip empty values.
     *
     * @param Function
     */
    template <typename Func> void each(Func &&func)
    {
        static_assert(std::is_invocable_v<Func, Id, T &>, "Each function must take T& as argument.");

        if constexpr (ReturnsBool<Func, Id, T &>)
            eachWithBreak(func);
        else
            eachNoBreak(func);
    }

    SparseSet(const SparseSet &) = delete;
    SparseSet &operator=(const SparseSet &) = delete;

  private:
    template <typename Func> void eachNoBreak(Func &&func)
    {
        for (auto i = 0; i < m_ids.size();)
        {
            if (m_pointers[m_ids[i]] != -1)
            {
                if (m_values[i])
                    func(m_ids[i], m_values[i]);

#ifndef ecs_disable_auto_prune
                if (!m_values[i])
                {
                    erase(m_ids[i]);
                    continue;
                }
#endif
            }

            ++i;
        }
    }
    template <typename Func> void eachWithBreak(Func &&func)
    {
        for (auto i = 0; i < m_ids.size();)
        {
            if (m_pointers[m_ids[i]] != -1)
            {
                if (m_values[i] && !func(m_ids[i], m_values[i]))
                    break;

#ifndef ecs_disable_auto_prune
                if (!m_values[i])
                {
                    erase(m_ids[i]);
                    continue;
                }
#endif
            }

            ++i;
        }
    }

    template <typename Func> void eachWithEmpty(Func &&func)
    {
        for (auto i = 0; i < m_ids.size(); ++i)
            if (m_pointers[m_ids[i]] != -1)
                func(m_ids[i], m_values[i]);
    }

    [[nodiscard]] T *get(Id id)
    {
        return contains(id) ? &(m_values[m_pointers[id]]) : nullptr;
    }

    [[nodiscard]] std::pair<Id, T *> getFirst()
    {
        if (m_ids.empty())
            return {0, nullptr};

        return {m_ids[0], &m_values[0]};
    }

    void lock()
    {
        m_isLocked = true;
    }

    void unlock()
    {
        m_isLocked = false;
    }

    [[nodiscard]] bool isLocked() const
    {
        return m_isLocked;
    }

    void insert(Id id, T value)
    {
        if (isLocked())
        {
            ECS_LOG_WARNING(typeid(T).name(), "is locked.  Cannot add to it");
            return;
        }
        if (contains(id))
        {
            ECS_LOG_WARNING(id, "Already contains", typeid(T).name(), "Add failed");
            return;
        }

        while (id >= m_pointers.size())
        {
            auto pSize = m_pointers.size();
            auto newSize = pSize > 0 ? pSize + (pSize / 2) : m_resize;
            m_pointers.resize(newSize, -1);
            m_values.reserve(newSize);
            m_ids.reserve(newSize);
        }

        m_pointers[id] = m_ids.size();
        // TODO Performance : See if using a pair to store id with component is better
        m_ids.push_back(id);
        m_values.push_back(std::move(value));
    }

    template <typename... Args> T *emplace(Id id, Args... args)
    {
        if (isLocked())
        {
            ECS_LOG_WARNING(typeid(T).name(), "is locked.  Cannot add to it");
            return nullptr;
        }
        if (contains(id))
        {
            ECS_LOG_WARNING(id, "Already contains", typeid(T).name(), "Add failed");
            return nullptr;
        }

        while (id >= m_pointers.size())
        {
            auto pSize = m_pointers.size();
            auto newSize = pSize > 0 ? pSize + (pSize / 2) : m_resize;
            m_pointers.resize(newSize, -1);
            m_values.reserve(newSize);
            m_ids.reserve(newSize);
        }

        m_pointers[id] = m_ids.size();
        m_ids.push_back(id);
        return &m_values.emplace_back(args...);
    }

    void overwrite(Id id, T value)
    {
        if (!contains(id))
        {
            ECS_LOG_WARNING(id, "does not contain", typeid(T).name(), "Overwrite failed");
            return;
        }

        m_values[m_pointers[id]] = std::move(value);
    }

    void erase(Id id1) override
    {
        if (!contains(id1))
            return;

        auto valIndex = m_pointers[id1];
        auto lastIndex = m_ids.size() - 1;

        auto lastId = m_ids[lastIndex];

        std::swap(m_values[valIndex], m_values[lastIndex]);
        m_values.pop_back();

        std::swap(m_ids[valIndex], m_ids[lastIndex]);
        m_ids.pop_back();

        m_pointers[lastId] = valIndex;
        m_pointers[id1] = -1;
    }

    [[nodiscard]] bool contains(Id id)
    {
        return id < m_pointers.size() && m_pointers[id] != -1;
    }

    void prune() override
    {
        for (auto i = 0; i < m_ids.size();)
        {
            if (m_pointers[m_ids[i]] != -1)
            {
                if (!m_values[i])
                {
                    erase(m_ids[i]);
                    continue;
                }
            }

            ++i;
        }
    }

  private:
    using value_type = T;
    size_t m_resize{};
    bool m_isLocked{false};

    std::vector<size_t> m_pointers{};
    std::vector<T> m_values{};
    std::vector<Id> m_ids{};

#ifdef ecs_allow_debug
  public:
#else
  private:
#endif
    [[nodiscard]] size_t size() const override
    {
        return m_ids.size();
    }
};
