#pragma once

#include "core.hpp"
#include "tags.hpp"
#include "utilities.hpp"

template <typename T> using Transformer = std::function<T(T &)>;

enum class Arrangement
{
    EMPTY,
    TRANSFORMED,
    MODIFIED,
    NOT_STACKED,
    STACKED,
};

enum class ComponentFlags
{
    NONE = 0,
    EMPTY,
};

struct DefaultComponent
{
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
            ECS_LOG_WARNING("Arrangement not found for", getTypeName<T>(), "!")
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
            ECS_LOG_WARNING("Arrangement not found for", getTypeName<T>(), "!")
        }
    }
    ComponentIterator(T *_component) : m_component(_component), isComponent(true)
    {
    }

    [[nodiscard]] T &operator*()
    {
        assert((isModified + isTransformed + isComponents + isComponent) == 1 &&
               "Iterator has conflicting modes!");

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
        assert((isModified + isTransformed + isComponents + isComponent) == 1 &&
               "Iterator has conflicting modes!");

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
        assert((isModified + isTransformed + isComponents + isComponent) == 1 &&
               "Iterator has conflicting modes!");

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

template <typename T> class Components
{
  public:
    Components(ComponentFlags _flag)
    {
    }

    template <typename... Args> Components(Args... _args)
    {
        emplace(_args...);
    }

    using Iterator = ComponentIterator<T>;

    Iterator begin()
    {
        switch (getArrangement())
        {
        case Arrangement::TRANSFORMED:
            return Iterator(transformed().begin(), Arrangement::TRANSFORMED);
        case Arrangement::MODIFIED:
            return Iterator(modified().begin(), Arrangement::MODIFIED);
        case Arrangement::NOT_STACKED:
            return Iterator(component());
        case Arrangement::STACKED:
            return Iterator(components().begin(), Arrangement::STACKED);
        default:
            throw std::runtime_error(std::string(getEnumString(getArrangement())) + " " + typeid(T).name() +
                                     " arrangement has no iterator!!");
        }

        return Iterator(nullptr);
    }

    Iterator end()
    {
        switch (getArrangement())
        {
        case Arrangement::TRANSFORMED:
            return Iterator(transformed().end(), Arrangement::TRANSFORMED);
        case Arrangement::MODIFIED:
            return Iterator(modified().end(), Arrangement::MODIFIED);
        case Arrangement::NOT_STACKED:
            return Iterator(nullptr);
        case Arrangement::STACKED:
            return Iterator(components().end(), Arrangement::STACKED);
        default:
            throw std::runtime_error(std::string(getEnumString(getArrangement())) + " " + typeid(T).name() +
                                     " arrangement has no iterator!!");
        }

        return Iterator(nullptr);
    }

    /**
     * @brief Standard read/write each function
     *
     * @param Function
     * @param Transformation pipeline behavior
     */
    template <typename Func>
    void mutate(Func &&fn)
        requires std::invocable<Func, T &>
    {
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, T &>, void>,
                      "Mutate function should not return a value.");

        if (isEmpty())
            return;

        // Cannot perform mutations on transformed components.
        // Maybe this could change in the future, but as of the time
        // of writing this, the transformation pipeline implementation
        // is such that the transformations are overwritten every time
        // a method if called anyway to ensure they are not stale
        // TODO Task : Reevaluate this and transformation pipelines
        handleTransformations(Transformation::PRESERVE);

        for (auto &comp : *this)
            fn(comp);
    }

    /**
     * @brief Read-only each function
     *
     * @param Function
     * @param Transformation pipeline behavior
     */
    template <typename Func>
    void inspect(Func &&fn, Transformation behavior = Transformation::DEFAULT)
        requires std::invocable<Func, const T &>
    {
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, const T &>, void>,
                      "Inspect function should not return a value.");

        if (isEmpty())
            return;

        handleTransformations(behavior);

        for (auto &comp : *this)
            fn(comp);
    }

    /**
     * @brief Allows for creating new derived data from the component
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return Data - Some derived data or a copy of a component property
     */
    template <typename P>
    [[nodiscard]] P derive(auto &&fn, P fallback, Transformation behavior = Transformation::DEFAULT)
        requires std::invocable<std::decay_t<decltype(fn)>, const T &>
    {
        static_assert(std::is_invocable_v<std::decay_t<decltype(fn)>, const T &>,
                      "Derive function must take const T& as argument.");
        static_assert(std::is_convertible_v<std::invoke_result_t<std::decay_t<decltype(fn)>, const T &>, P>,
                      "Derive function must return correct type.");

        if (isEmpty())
            return fallback;

        handleTransformations(behavior);

        for (const auto &comp : *this)
            return std::forward<decltype(fn)>(fn)(comp);
    }

    template <typename P>
    [[nodiscard]] P derive(auto &&fn, Transformation behavior = Transformation::DEFAULT)
        requires std::invocable<std::decay_t<decltype(fn)>, const T &>
    {
        static_assert(std::is_invocable_v<std::decay_t<decltype(fn)>, const T &>,
                      "Derive function must take const T& as argument.");
        static_assert(std::is_default_constructible_v<P>, "Property is not default constructable");
        static_assert(std::is_convertible_v<std::invoke_result_t<std::decay_t<decltype(fn)>, const T &>, P>,
                      "Derive function must return correct type.");

        if (isEmpty())
            return P{};

        handleTransformations(behavior);

        for (const auto &comp : *this)
            return std::forward<decltype(fn)>(fn)(comp);

        return P{};
    }

    /**
     * @brief Extract a value as readonly FROM A NON-STACKED COMPONENT.
     *
     * THROWS A RUNTIME ERROR IF THE COMPONENT IS EMPTY!
     *
     * @param T::Prop
     * @param Transformation pipeline behavior
     *
     * @return Property reference
     */
    template <typename Prop>
    [[nodiscard]] const Prop &peek(Prop T::*prop, Transformation behavior = Transformation::DEFAULT)
        requires(!Tags::isStacked<T>())
    {
        static_assert(!Tags::isStacked<T>(), "Cannot use peek method with a stacked component");

        if (isEmpty())
        {
            throw std::runtime_error("Property: " + getTypeName<decltype(prop)>() +
                                     " could not be peeked from Component: " + getTypeName<T>());
        }

        handleTransformations(behavior);

        for (auto &comp : *this)
            return comp.*prop;
    }

    /**
     * @brief Perform some check on the component
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return Bool - Results of the check
     */
    template <typename Func>
    [[nodiscard]] bool check(Func &&fn, Transformation behavior = Transformation::DEFAULT)
        requires(std::invocable<Func, const T &> && !Tags::isStacked<T>())
    {
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, const T &>, bool>,
                      "Check function must return bool.");
        static_assert(!Tags::isStacked<T>(), "Cannot use check methods with a stacked component");

        if (isEmpty())
            return false;

        handleTransformations(behavior);

        for (auto &comp : *this)
            if (fn(comp))
                return true;

        return false;
    }

    /**
     * @brief Filter components
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return New Components wrapper containing the filtered results
     */
    template <typename Func>
    [[nodiscard]] Components<T> filter(Func &&fn, Transformation behavior = Transformation::DEFAULT)
        requires std::invocable<Func, const T &>
    {
        static_assert(std::is_invocable_v<Func, const T &>,
                      "Filter function must take const T& as argument.");
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, const T &>, bool>,
                      "Filter function must return bool.");

        Components<T> newComps(ComponentFlags::EMPTY);
        newComps.setTransformer(m_transformer);

        if (isEmpty())
            return std::move(newComps);

        handleTransformations(behavior);
        bool shouldFilter = !shouldTransform(behavior);

        for (auto &comp : *this)
        {
            if (!fn(comp))
                continue;

            if (shouldFilter)
                newComps.modified().push_back(&comp);
            else
                newComps.transformed().push_back(T(comp));
        }

        return std::move(newComps);
    }

    /**
     * @brief Get the first instance of the component which passes the check
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return New Components wrapper containing the result
     */
    template <typename Func>
    [[nodiscard]] Components<T> find(Func &&fn, Transformation behavior = Transformation::DEFAULT)
        requires std::invocable<Func, const T &>
    {
        static_assert(std::is_invocable_v<Func, const T &>, "Find function must take const T& as argument.");
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, const T &>, bool>,
                      "Find function must return bool.");

        Components<T> newComps(ComponentFlags::EMPTY);
        newComps.setTransformer(m_transformer);

        if (isEmpty())
            return std::move(newComps);

        handleTransformations(behavior);

        for (auto &comp : *this)
        {
            if (!fn(comp))
                continue;

            if (shouldTransform(behavior))
                newComps.transformed().push_back(T(comp));
            else
                newComps.modified().push_back(&comp);

            break;
        }

        return std::move(newComps);
    }

    /**
     * @brief Get the first component
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return New Components wrapper containing the result
     */
    [[nodiscard]] Components<T> first(Transformation behavior = Transformation::DEFAULT)
    {
        Components<T> newComps(ComponentFlags::EMPTY);
        newComps.setTransformer(m_transformer);

        if (isEmpty())
            return std::move(newComps);

        handleTransformations(behavior);

        for (auto &comp : *this)
        {
            if (shouldTransform(behavior))
                newComps.transformed().push_back(T(comp));
            else
                newComps.modified().push_back(&comp);

            break;
        }

        return std::move(newComps);
    }

    /**
     * @brief Get sorted components
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return New Components wrapper containing the sorted results
     */
    template <typename Func>
    [[nodiscard]] Components<T> sort(Func &&fn, Transformation behavior = Transformation::DEFAULT)
        requires std::invocable<Func, const T &, const T &>
    {
        static_assert(std::is_invocable_v<Func, const T &, const T &>,
                      "Sort function must take two const T& as arguments.");
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, const T &, const T &>, bool>,
                      "Sort function must return bool.");

        Components<T> newComps(ComponentFlags::EMPTY);
        newComps.setTransformer(m_transformer);

        if (isEmpty())
            return std::move(newComps);

        handleTransformations(behavior);
        bool isTransformed = !shouldTransform(behavior);

        for (auto &comp : *this)
        {
            if (isTransformed)
                newComps.transformed().push_back(T(comp));
            else
                newComps.modified().push_back(&comp);
        }

        std::sort(newComps.modified().begin(), newComps.modified().end(),
                  [&](T *a, T *b) { return fn(*a, *b); });

        return std::move(newComps);
    }

    /**
     * @brief Reduce multiple components into a single component
     *
     * Creates an instance of the component with the reduced properties
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return Accumulator - Reduced Component instance
     */
    template <typename Func>
    [[nodiscard]] T reduce(Func &&fn, T reduced, Transformation behavior = Transformation::DEFAULT)
        requires std::invocable<Func, T &, const T &>
    {
        static_assert(std::is_invocable_v<Func, T &, const T &>,
                      "Reduce function must take const T& as argument.");
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, T &, const T &>, void>,
                      "Reduce function should not return a value.");

        if (isEmpty())
            return reduced;

        handleTransformations(behavior);

        for (auto &comp : *this)
            fn(reduced, comp);

        return reduced;
    }

    /**
     * @brief Reduce multiple components into a single component
     *
     * Creates an instance of the component with the reduced properties
     *
     * IMPORTANT: THE COMPONENT REQUIRES DEFAULT CONSTRUCTOR ARGUMENTS
     * TO CREATE THE NEW INSTANCE, AND THOSE ARGUMENTS SHOULD PROBABLY
     * BE 0/FALSY OR ELSE COULD LEAD TO UNEXPECTED BEHAVIOR
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return Accumulator - Reduced Component instance
     */
    template <typename Func>
    [[nodiscard]] T reduce(Func &&fn, Transformation behavior = Transformation::DEFAULT)
        requires std::invocable<Func, T &, const T &>
    {
        static_assert(std::is_invocable_v<Func, T &, const T &>,
                      "Reduce function must take const T& as argument.");
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, T &, const T &>, void>,
                      "Reduce function should not return a value.");
        static_assert(std::is_default_constructible_v<T>, "Component is not default constructable");

        return reduce(fn, T{}, behavior);
    }

    /**
     * @brief Remove components specified by the results of the function argument
     *
     * @param Function
     */
    template <typename Func>
    void remove(Func &&fn)
        requires std::invocable<Func, const T &>
    {
        static_assert(std::is_invocable_v<Func, const T &>,
                      "Remove function must take const T& as argument.");
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, const T &>, bool>,
                      "Remove function must return bool.");

        clearTransformed();

        if (isComponent())
        {
            if (fn(*component()))
                m_component.reset();

            return;
        }

        if (!isComponents())
            return;

        for (auto iter = components().begin(); iter != components().end();)
        {
            if (fn(*iter))
                iter = components().erase(iter);
            else
                ++iter;
        }
    }

    explicit operator bool() const
    {
        if (isEmpty())
            return false;

        return true;
    }

#ifdef ecs_allow_debug
    void printData()
    {
        auto arrangement = getArrangement();
        auto arrangementType = getEnumString(arrangement);
        PRINT("COMPONENT ARRANGEMENT:", arrangementType, " - SIZE:", size())
    }
#endif

    template <typename EntityId> friend class EntityComponentManager;

  private:
    template <typename... Args> void emplace_back(Args &&...args)
    {
        components().emplace_back(std::forward<Args>(args)...);
    }

    template <typename... Args> void emplace(Args... args)
    {
        if (!Tags::isStacked<T>())
        {
            m_component.emplace(args...);
            return;
        }

        emplace_back(args...);
    }

#ifdef ecs_allow_unsafe
  public:
#else
  private:
#endif
    /*
     * Allows direct access to components via pointer stored within the components wrapper.
     * Use cautiously as this bypasses all safeguards in place when using the
     * regular approach to accessing components. This is only intended to be used as an
     * escape hatch if some specific case arises that does not have a proper fix.
     */
    [[nodiscard]] std::vector<T *> getUnsafe()
    {

        if (isModified())
            return m_modified;

        std::vector<T *> vec;

        for (auto &comp : *this)
            vec.push_back(&comp);

        return std::move(vec);
    }

  private:
    [[nodiscard]] std::vector<T *> &modified()
    {
        return m_modified;
    }

    [[nodiscard]] std::vector<T> &transformed()
    {
        return m_transformed;
    }

    [[nodiscard]] std::vector<T> &components()
    {
        return m_components;
    }

    [[nodiscard]] T *component()
    {
        if (!m_component.has_value())
            return nullptr;

        return m_component.operator->();
    }

    [[nodiscard]] bool isEmpty() const
    {
        return !isModified() && !isTransformed() && !isComponent() && !isComponents();
    }

    [[nodiscard]] bool isModified() const
    {
        return !m_modified.empty();
    }

    [[nodiscard]] bool isTransformed() const
    {
        return !m_transformed.empty();
    }

    [[nodiscard]] bool isComponent() const
    {
        return m_component.has_value();
    }

    [[nodiscard]] bool isComponents() const
    {
        return !m_components.empty();
    }

    [[nodiscard]] bool isTransformer() const
    {
        return !!m_transformer;
    }

    void setTransformer(Transformer<T> transformerFn)
    {
        m_transformer = std::move(transformerFn);
    }

    [[nodiscard]] bool shouldTransform(Transformation behavior)
    {
        if (!isTransformer() || isTransformed())
            return false;

        return (Tags::isTransform<T>() && behavior == Transformation::DEFAULT) ||
               behavior == Transformation::TRANSFORM;
    }

    void createTransformed()
    {
        for (auto &comp : *this)
            transformed().push_back(m_transformer(comp));
    }

    void clearTransformed()
    {
        transformed().clear();
    }

    void handleTransformations(Transformation behavior)
    {
        if (!isTransformer())
            return;

        if (isTransformed())
            clearTransformed();

        if (shouldTransform(behavior))
            createTransformed();
    }

    [[nodiscard]] Arrangement getArrangement()
    {
        if (isTransformed())
            return Arrangement::TRANSFORMED;

        if (isModified())
            return Arrangement::MODIFIED;

        if (isComponent())
            return Arrangement::NOT_STACKED;

        if (isComponents())
            return Arrangement::STACKED;

        return Arrangement::EMPTY;
    }

  private:
    std::vector<T *> m_modified;
    std::vector<T> m_transformed;
    std::vector<T> m_components;
    std::optional<T> m_component;

    Transformer<T> m_transformer;

#ifdef ecs_allow_debug
  public:
#else
  private:
#endif
    [[nodiscard]] size_t size()
    {
        if (isModified())
            return modified().size();

        if (isTransformed())
            return transformed().size();

        if (isComponents())
            return components().size();

        if (isComponent())
            return 1;

        return 0;
    }
};
