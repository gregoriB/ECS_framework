#pragma once

#include "components_iterator.hpp"
#include "macros.hpp"
#include "tags.hpp"
#include "utilities.hpp"
#include <functional>
#include <tuple>

namespace ECS
{
namespace internal
{

template <typename T> using Transformer = std::function<T(T &)>;

struct DefaultComponent
{
};

enum class Transformation
{
    DEFAULT,
    PRESERVE,
    TRANSFORM
};

/**
 * @brief A wrapper for a component of the specific type.  The wrapper controls how the component is arranged
 * and provides access methods for the component data
 *
 * This components wrapper is intended to be the only way to access component data, and to allow the user to
 * safely access component data Access is very controlled, and some methods are not even available unless
 * certain criteria is met, such as a specific component tag is used
 *
 * Components are stored in various configuration, depending on how their tag and/or how they were created.
 * For instance, NoStack-tagged components are not store or accessed via vector
 * Filtered, narrowed, and sorted components are via a vector of pointers to the original components
 * Transformed components are at this time stored in a vector, regardless of their tag
 *
 * Accessor and filtering methods are provided.  However, the only way to make any mutations on a component
 * are via the .mutate method This means every other method either provides const references or copies The
 * exception to this it the unsafe .unpack() method, which is provides raw pointers and is only accessible via
 * compiler flag.  This is an escape hatch and not intended for regular use
 */
template <typename T> class ComponentsWrapper
{
  public:
    enum class ComponentFlags
    {
        NONE = 0,
        EMPTY,
    };

    ComponentsWrapper(ComponentFlags _flag)
    {
    }

    template <typename... Args> ComponentsWrapper(Args... _args)
    {
        emplace(_args...);
    }

    template <typename U> using Components = ComponentsWrapper<U>;

    /**
     * @brief Standard read/write for each function
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
        // a method is called to ensure the transformations are not stale.
        //
        // TODO Task : Reevaluate this and transformation pipelines
        handleTransformations(Transformation::PRESERVE);

        for (auto &comp : *this)
            fn(comp);
    }

    /**
     * @brief Read-only for each function
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
     * @brief NON-STACKED COMPONENT ONLY! Allows for creating new derived data from the component
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return Data - Some derived data or a copy of a component property
     */
    template <typename P>
    [[nodiscard]] P derive(auto &&fn, P fallback, Transformation behavior = Transformation::DEFAULT)
        requires(std::invocable<std::decay_t<decltype(fn)>, const T &> && !Utilities::shouldStack<T>())
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

    /**
     * @brief NON-STACKED COMPONENT ONLY! Allows for creating new derived data from the component
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return Derived data instance
     */
    template <typename P>
    [[nodiscard]] P derive(auto &&fn, Transformation behavior = Transformation::DEFAULT)
        requires(std::invocable<std::decay_t<decltype(fn)>, const T &> && !Utilities::shouldStack<T>())
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
     * @brief NON-STACKED COMPONENT ONLY! - Extract a value as readonly
     *
     * @param T::Prop
     * @param Transformation pipeline behavior
     *
     * @return Component property const reference
     */
    template <typename Prop>
    [[nodiscard]] const Prop &peek(Transformation behavior, Prop T::*prop)
        requires(!Utilities::shouldStack<T>())
    {
        static_assert(!Utilities::shouldStack<T>(), "Cannot use peek method with a stacked component");

        ECS_ASSERT(!isEmpty(), "Property: " + Utilities::getTypeName<decltype(prop)>() +
                                   " could not be peeked from Component: " + Utilities::getTypeName<T>());

        handleTransformations(behavior);

        return getConstProp(prop);
    }

    /**
     * @brief NON-STACKED COMPONENT ONLY! - Extract a value as readonly
     *
     * @param T::Prop
     * @param Transformation pipeline behavior
     *
     * @return Component property const reference
     */
    template <typename Prop>
    [[nodiscard]] const Prop &peek(Prop T::*prop)
        requires(!Utilities::shouldStack<T>())
    {
        return peek(Transformation::DEFAULT, prop);
    }

    /**
     * @brief NON-STACKED COMPONENT ONLY! - Extract a value as readonly
     *
     * @param T::Prop
     * @param Transformation pipeline behavior
     *
     * @return Container of const references to component properties
     */
    template <typename... Props>
    [[nodiscard]] std::tuple<const Props &...> peek(Transformation behavior, Props T::*...props)
        requires(!Utilities::shouldStack<T>())
    {
        ECS_ASSERT(!isEmpty(), "One or more properties could not be peeked from Component: " +
                                   Utilities::getTypeName<T>());

        handleTransformations(behavior);

        return std::tie(getConstProp(props)...);
    }

    /**
     * @brief NON-STACKED COMPONENT ONLY! - Extract a value as readonly
     *
     * @param T::Prop
     * @param Transformation pipeline behavior
     *
     * @return Container of const references to component properties
     */
    template <typename... Props>
    [[nodiscard]] std::tuple<const Props &...> peek(Props T::*...props)
        requires(!Utilities::shouldStack<T>())
    {
        return peek(Transformation::DEFAULT, props...);
    }

    /**
     * @brief Filter components
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return New Components wrapper instance containing the filtered results
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
     * @return New Components wrapper instance containing the found results
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
     * Intended to be used as a companion to .sort()
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return New Components wrapper instance containing the first component
     */
    [[nodiscard]] Components<T> first(Transformation behavior = Transformation::DEFAULT)
    {
        Components<T> newComps(ComponentFlags::EMPTY);
        newComps.setTransformer(m_transformer);

        if (isEmpty())
            return std::move(newComps);

        handleTransformations(behavior);

        auto &comp = *begin();

        if (shouldTransform(behavior))
            newComps.transformed().push_back(T(comp));
        else
            newComps.modified().push_back(&comp);

        return std::move(newComps);
    }

    /**
     * @brief Get the last component
     *
     * Intended to be used as a companion to .sort()
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return New Components wrapper instance containing the last component
     */
    [[nodiscard]] Components<T> last(Transformation behavior = Transformation::DEFAULT)
    {
        Components<T> newComps(ComponentFlags::EMPTY);
        newComps.setTransformer(m_transformer);

        if (isEmpty())
            return std::move(newComps);

        handleTransformations(behavior);

        auto &comp = *(end() - 1);

        if (shouldTransform(behavior))
            newComps.transformed().push_back(T(comp));
        else
            newComps.modified().push_back(&comp);

        return std::move(newComps);
    }

    /**
     * @brief Get sorted components
     *
     * @param Function
     * @param Transformation pipeline behavior
     *
     * @return New Components wrapper instance containing the sorted components
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

        if (newComps.size() > 1)
            std::sort(newComps.modified().begin(), newComps.modified().end(),
                      [&](T *a, T *b) { return fn(*a, *b); });

        return std::move(newComps);
    }

    /**
     * @brief Reduce multiple components into a single component
     *
     * Creates an instance of the component with the reduced properties
     *
     * @param Reducer function
     * @param Transformation pipeline behavior
     *
     * @return Reduced Component instance
     */
    template <typename Func>
    [[nodiscard]] T reduce(Func &&fn, T reduced, Transformation behavior = Transformation::DEFAULT)
        requires std::invocable<Func, T &, const T &>
    {
        static_assert(std::is_invocable_v<Func, T &, const T &>,
                      "Reducer function must take const T& as argument.");
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, T &, const T &>, void>,
                      "Reducer function should not return a value.");

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
     * @param Reducer function
     * @param Transformation pipeline behavior
     *
     * @return Accumulator - Reduced Component instance
     */
    template <typename Func>
    [[nodiscard]] T reduce(Func &&fn, Transformation behavior = Transformation::DEFAULT)
        requires std::invocable<Func, T &, const T &>
    {
        static_assert(std::is_invocable_v<Func, T &, const T &>,
                      "Reducer function must take const T& as argument.");
        static_assert(std::is_convertible_v<std::invoke_result_t<Func, T &, const T &>, void>,
                      "Reducer function should not return a value.");
        static_assert(std::is_default_constructible_v<T>, "Component is not default constructable");

        return reduce(fn, T{}, behavior);
    }

    /**
     * @brief Remove component if it evaluates to true
     *
     * @param Removal check function
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

    /**
     * @brief Evaluate truthiness based on the existence of stored values
     */
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
        auto arrangementType = Utilities::getEnumString(arrangement);
        Utilities::print("COMPONENT ARRANGEMENT:", arrangementType, " - SIZE:", size());
    }
#endif

    template <typename EntityId> friend class EntityComponentManager;

  private:
    using Iterator = ComponentsIterator<T>;

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
            throw std::runtime_error(std::string(Utilities::getEnumString(getArrangement())) + " " +
                                     typeid(T).name() + " arrangement has no iterator!!");
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
            throw std::runtime_error(std::string(Utilities::getEnumString(getArrangement())) + " " +
                                     typeid(T).name() + " arrangement has no iterator!!");
        }

        return Iterator(nullptr);
    }

    template <typename... Args> void emplace_back(Args &&...args)
    {
        components().emplace_back(std::forward<Args>(args)...);
    }

    template <typename... Args> void emplace(Args... args)
    {
        if (!Utilities::shouldStack<T>())
        {
            m_component.emplace(args...);
            return;
        }

        emplace_back(args...);
    }

    template <typename Prop> [[nodiscard]] const Prop &getConstProp(Prop T::*prop)
    {
        return *begin().*prop;
    }

#ifdef ecs_allow_unsafe
  public:
#else
  private:
#endif
    /*
     * Allows direct access to components stored within the components wrapper.
     * Be aware that this bypasses all safeguards in place when using the
     * regular approach to accessing components.
     */
    [[nodiscard]] std::vector<T *> unpack()
    {
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

        return (Utilities::isTransform<T>() && behavior == Transformation::DEFAULT) ||
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
}; // namespace internal
}; // namespace ECS
