#pragma once

#include "core.hpp"

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

