#include "core.hpp"
#include "macros.hpp"
#include "utilities.hpp"

enum class Arrangement
{
    EMPTY,
    TRANSFORMED,
    MODIFIED,
    NOT_STACKED,
    STACKED,
};

template <typename T> class ComponentIterator
{
  public:
    std::vector<T *>::iterator m_modifiedIter;
    bool isModified{false};

    std::vector<T>::iterator m_transformedIter;
    bool isTransformed{false};

    std::vector<T>::iterator m_componentsIter;
    bool isComponents{false};

    T *m_component;
    bool isComponent{false};

    ComponentIterator(std::vector<T *>::iterator _iter, Arrangement _arrangement)
    {
        switch (_arrangement)
        {
        case Arrangement::MODIFIED:
            isModified = true;
            m_modifiedIter = _iter;
            break;
        default:
            ECS_LOG_WARNING("Arrangement not found for", ECS::internal::Utilities::getTypeName<T>(), "!")
        }
    }
    ComponentIterator(std::vector<T>::iterator _iter, Arrangement _arrangement)
    {
        switch (_arrangement)
        {
        case Arrangement::TRANSFORMED:
            isTransformed = true;
            m_transformedIter = _iter;
            break;
        case Arrangement::STACKED:
            isComponents = true;
            m_componentsIter = _iter;
            break;
        default:
            ECS_LOG_WARNING("Arrangement not found for", ECS::internal::Utilities::getTypeName<T>(), "!")
        }
    }
    ComponentIterator(T *_component) : m_component(_component), isComponent(true)
    {
    }

    [[nodiscard]] T &operator*()
    {
        ECS_ASSERT((isModified + isTransformed + isComponents + isComponent) == 1,
                   "Iterator has conflicting modes!")

        if (isComponent)
            return *m_component;
        if (isModified)
            return *(*m_modifiedIter);
        if (isTransformed)
            return *m_transformedIter;
        if (isComponents)
            return (*m_componentsIter);

        ECS_LOG_WARNING("Something has gone wrong if you've reached this point!")
        return *m_component;
    }

    ComponentIterator &operator++()
    {
        ECS_ASSERT((isModified + isTransformed + isComponents + isComponent) == 1,
                   "Iterator has conflicting modes!")

        if (isComponent)
            m_component = nullptr;
        else if (isModified)
            ++m_modifiedIter;
        else if (isTransformed)
            ++m_transformedIter;
        else if (isComponents)
            ++m_componentsIter;

        return *this;
    }

    bool operator==(const ComponentIterator &other) const
    {
        ECS_ASSERT((isModified + isTransformed + isComponents + isComponent) == 1,
                   "Iterator has conflicting modes!")

        if (isComponent)
            return m_component == other.m_component;
        if (isModified)
            return m_modifiedIter == other.m_modifiedIter;
        if (isTransformed)
            return m_transformedIter == other.m_transformedIter;
        if (isComponents)
            return m_componentsIter == other.m_componentsIter;

        ECS_LOG_WARNING("Something went wrong if you reached this point!")
        return m_component == other.m_component;
    }

    bool operator!=(const ComponentIterator &other) const
    {
        return !(*this == other);
    }
};
