#pragma once

#include "components.hpp"
#include "macros.hpp"
#include "sparse_set.hpp"
#include "tags.hpp"
#include "utilities.hpp"
#include "grouping.hpp"

namespace ECS
{
namespace internal
{

/**
 * @brief The main entry point into the ECS.  Used to add, query, and remove components and component sets, as
 * well as perform other operations.
 */
template <typename EntityId> class EntityComponentManager
{
  private:
    template <typename T> using Components = ComponentsWrapper<T>;
    template <typename T> using ComponentSet = SparseSet<EntityId, Components<T>>;
    template <typename T> using ComponentSetMap = std::unordered_map<size_t, std::unique_ptr<T>>;
    template <typename... Ts> using ComponentSetGroup = Grouping<EntityId, ComponentSet<Ts>...>;

    using ErasedComponent = Components<DefaultComponent>;
    using ErasedComponentSet = BaseSparseSet<EntityId, ErasedComponent>;

    using StoredComponents = ComponentSetMap<ErasedComponentSet>;
    using StoredTags = std::unordered_map<size_t, std::unordered_set<size_t>>;

    template <typename T> using TransformationFn = std::function<T(EntityId, T)>;
    using StoredTransformationFn = std::function<DefaultComponent(EntityId, DefaultComponent &)>;
    using StoredTransformationFnMap = std::unordered_map<size_t, StoredTransformationFn>;

  public:
    /**
     * @brief Entity Component Manager constructor
     *
     * @param reservedEntities - Number of entity ids to keep in reserve, starting from 1
     * @param minSetSize - Minimum number of elements a set should contain
     * @param setSize - Specific number of elements a set should contain in most cases
     */
    EntityComponentManager(EntityId reservedEntities = 10, size_t minSetSize = 100, size_t setSize = 10024)
    {
        m_nextEntityId = static_cast<EntityId>(reservedEntities);
        m_minSetSize = minSetSize;
        m_standardSetSize = setSize;
    }

    /**
     * @brief Creates a new unique entity id
     *
     * @return EntityId
     */
    EntityId createEntity()
    {
        return ++m_nextEntityId;
    }

    /**
     * @brief Constructs and add a component to a set by entity id
     *
     * If the component set has not yet been created, it is created before
     * the entity component is added.
     *
     * @tparam T - Component type
     * @tparam Ids - Variadiac id arguments
     *
     * @param Entity Id
     * @param Variable arguments for the component constructor
     */
    template <typename T, typename... Args> void add(EntityId eId, Args... args)
    {
        if (eId == 0)
            return;

        if constexpr (Utilities::isUnique<T>())
        {
            addUnique<T>(eId, args...);
            return;
        }

        addComponent<T>(eId, args...);
    }

    /**
     * @brief Overwrites a components instance for the specified entity
     *
     * @tparam Args - Variadic args
     * @tparam Ids - Variadiac id arguments
     *
     * @param Entity Id
     * @param Variable arguments for the component constructor
     */
    template <typename T, typename... Args> void overwrite(EntityId eId, Args... args)
    {
        if (eId == 0)
            return;

        auto cSet = getComponentSetPtr<T>();
        if (!cSet)
            return;

        if constexpr (Utilities::isUnique<T>())
        {
            overwriteUnique<T>(eId, *cSet, args...);
            return;
        }

        overwriteComponent<T>(eId, *cSet, args...);
    }

    /**
     * @brief Get specified components for the entity
     *
     * @tparam Ts - Component types
     * @tparam Ids - Variadiac id arguments
     *
     * @param Entity Id
     *
     * @return Entity component reference
     */
    template <typename... T> [[nodiscard]] std::tuple<Components<T> &...> get(EntityId eId)
    {
        return {getComponents<T>(eId)...};
    }

    /**
     * @brief Get a specified component for multiple entities
     *
     *
     * @tparam T - Component type
     * @tparam Ids - Variadiac id arguments
     *
     * @param Entity ids
     *
     * @return Container of components
     */
    template <typename T, typename... Ids> [[nodiscard]] auto get(EntityId id, Ids... ids)
    {
        return getComponentsHelper<T>(getComponentSet<T>(), id, ids...);
    }

    /**
     * @brief Get a specified unique component
     *
     * @tparam T - Component type
     *
     * @return Container with the entity id and component
     */
    template <typename T>
    [[nodiscard]] std::pair<EntityId, Components<T> &> getUnique()
        requires(Utilities::isUnique<T>())
    {
        auto &cSet = getComponentSet<T>();
        ECS_ASSERT(Utilities::isUnique<T>(), Utilities::getTypeName<T>() + " is not a unique component!");

        EntityId id{0};
        Components<T> *compsPtr;
        cSet.each([&](EntityId eId, auto &comps) {
            if (id == 0)
            {
                id = eId;
                compsPtr = &comps;
            }
            // Should not break the loop after the first element
            // because auto-pruning will clean up dummy components
            // as it iterates over them
        });

        if (compsPtr != nullptr)
            return {id, *compsPtr};

        // If the set is empty, create a dummy component for a fake entity
        std::tuple<Components<T> &> compsTuple = get<T>(0);
        return {0, std::get<0>(compsTuple)};
    }

    /**
     * @brief Get the entity ids which are persistent across all specified component types
     *
     * @tparam T - Variadiac type arguments
     *
     * @return Container of entity ids
     */
    template <typename T, typename... Ts> [[nodiscard]] const std::vector<EntityId> getEntityIds()
    {
        if constexpr (sizeof...(Ts) == 0)
            return getComponentSet<T>(m_minSetSize).getIds();

        auto &cSet = getComponentSet<T>();
        std::unordered_set<EntityId> ids(cSet.getIds().begin(), cSet.getIds().end());
        (
            [&]() {
                auto &cSet = getComponentSet<Ts>();
                cSet.prune();
                for (auto iter = ids.begin(); iter != ids.end();)
                    iter = cSet.contains(*iter) ? std::next(iter) : ids.erase(iter);
            }(),
            ...);

        return std::vector<EntityId>(ids.begin(), ids.end());
    }

    /**
     * @brief Find overlapping entities for the specified types
     *
     * Creates a group of entities with all of the specified types in common
     *
     * @tparam Ts - Component types
     *
     * @return Grouping of entities
     */
    // CHANGE NAME: group() , groupCommon() , groupOverlapping() , groupShared() ?
    template <typename... Ts> ComponentSetGroup<Ts...> getGroup()
    {
        bool shouldBreak{};
        std::tuple<ComponentSet<Ts> *...> sets{};
        std::unordered_set<EntityId> ids{};
        (
            [&]() {
                if (shouldBreak)
                    return;

                auto cSet = getComponentSetPtr<Ts>();
                if (!cSet)
                {
                    ids.clear();
                    shouldBreak = true;
                    return;
                }

                if (ids.empty())
                {
                    auto &entityIds = cSet->getIds();
                    ids = std::unordered_set<EntityId>(entityIds.begin(), entityIds.end());
                }
                else
                {
                    // TODO Task : Reevalue auto-pruning on the component set
                    // Maybe do it on .contains(id) call instead, for each set element
                    // which contains but evaluates to falsy
                    cSet->prune();
                    for (auto iter = ids.begin(); iter != ids.end();)
                        iter = cSet->contains(*iter) ? std::next(iter) : ids.erase(iter);
                }

                if (!ids.empty())
                    std::get<ComponentSet<Ts> *>(sets) = cSet;
                else
                    shouldBreak = true;
            }(),
            ...);

        if (ids.empty())
            return ComponentSetGroup<Ts...>();

        return ComponentSetGroup<Ts...>(std::vector<EntityId>(ids.begin(), ids.end()), std::move(sets));
    }

    /**
     * @brief Gets entire component sets
     *
     * @tparam Ts - Component types
     *
     * @return Container of component sets
     */
    template <typename... Ts> [[nodiscard]] std::tuple<ComponentSet<Ts> &...> getAll()
    {
        return {getComponentSet<Ts>(m_minSetSize)...};
    }

    /**
     * @brief Check whether or not the component set contains the entity id
     *
     * @tparam T - Component type
     *
     * @param Entity Id
     *
     * @return Bool - true if contains the entity
     */
    template <typename T> [[nodiscard]] bool contains(EntityId eId)
    {
        auto cSetPtr = getComponentSetPtr<T>();
        if (!cSetPtr)
            return false;

        auto compsPtr = cSetPtr->get(eId);
        if (!compsPtr || !(*compsPtr))
            return false;

        return true;
    }

    /**
     * @brief Check whether or not the component set exists
     *
     * @tparam T - Component type
     *
     * @return Bool - true if component set exists
     */
    template <typename T> [[nodiscard]] bool exists()
    {
        auto cSetPtr = getComponentSetPtr<T>();
        if (!cSetPtr || !(*cSetPtr))
            return false;

        return true;
    }

    /**
     * @brief Clear entire component sets
     *
     * @tparam Ts - Component types
     */
    template <typename... Ts> void clear()
    {
#ifdef ecs_allow_debug
        (debugCheckRequired<Ts>("Clear"), ...);
#endif
        clearComponents<Ts...>();
    }

    /**
     * @brief Remove specified ids from each specified set
     *
     * @tparam Ts - Component types
     */
    template <typename... Ts> void remove(const std::vector<EntityId> &ids)
    {
        (removeIds<Ts>(ids), ...);
    }

    /**
     * @brief Remove specified ids from each specified set
     *
     * @tparam Ts - Component types
     * @tparam Ids - Variadic ids arguments
     */
    template <typename... Ts, typename... Ids> void remove(Ids... ids)
    {
        (removeIds<Ts>(ids...), ...);
    }

    /**
     * @brief Remove the id from each specified set
     *
     * @tparam Ts - Component types
     */
    template <typename... Ts> void remove(EntityId eId)
    {
        if constexpr (sizeof...(Ts) == 0)
        {
            removeEntity(eId);
            return;
        }

        (removeIds<Ts>(eId), ...);
    }

    /**
     * @brief Remove specified ids from EVERY set
     */
    void remove(const std::vector<EntityId> &ids)
    {
        for (const auto &id : ids)
            remove(id);
    }

    /**
     * @brief Remove specified ids from EVERY set
     *
     * @tparam Ids - Variadic ids arguments
     */
    template <typename... Ids> void remove(Ids... ids)
    {
        // TODO Performance : Use a less heavy-handed approach if possible
        (removeEntity(ids), ...);
    }

    /**
     * @brief Iterate over and remove empty components from the specified sets
     *
     * @tparam Ts - Component types
     */
    template <typename... Ts> void prune()
    {
        (prune<Ts>(getComponentHash<Ts>()), ...);
    }

    /**
     * @brief Stores a transformation function for the specified component
     *
     * @param Transformation function
     */
    template <typename T> constexpr void registerTransformation(TransformationFn<T> transformationFn)
    {
        auto casted = reinterpret_cast<StoredTransformationFn &>(transformationFn);
        m_transformationMap.emplace(getComponentHash<T>(), std::move(casted));
    }

    EntityComponentManager(const EntityComponentManager &) = delete;
    EntityComponentManager &operator=(const EntityComponentManager &) = delete;

  private:
    template <typename T> void removeIds(const std::vector<EntityId> &ids)
    {
        auto cSetPtr = getComponentSetPtr<T>();
        if (!cSetPtr)
            return;

        cSetPtr->erase(ids);
    }

    template <typename T, typename... Ids> void removeIds(Ids... ids)
    {
        auto cSetPtr = getComponentSetPtr<T>();
        if (!cSetPtr)
            return;

        cSetPtr->erase(ids...);
    }

    void removeEntity(EntityId eId)
    {
        for (auto iter = getStoredComponents().begin(); iter != getStoredComponents().end(); ++iter)
            getSetFromIterator(iter).erase(eId);
    }

    template <typename T, typename... Args> void addUnique(EntityId eId, Args... args)
    {
        addComponent<T>(eId, args...);
        getComponentSet<T>(m_minSetSize).lock();
    }

    template <typename T, typename... Args>
    void overwriteUnique(EntityId eId, ComponentSet<T> cSet, Args... args)
    {
        auto [uniqueId, _] = getUnique<T>();

        ECS_ASSERT(eId == uniqueId,
                   "Enitty ID: " + std::to_string(eId) +
                       " is not owning entity for unique component: " + Utilities::getTypeName<T>())
        ECS_ASSERT(Utilities::isUnique<T>(), Utilities::getTypeName<T>() + " is not unique!")

        overwriteComponent<T>(eId, cSet, args...);
    }

#ifdef ecs_allow_debug
    template <typename Component> void debugCheckRequired(std::string operation)
    {
        if (!Utilities::isRequired<Component>())
            return;

        ECS_LOG_WARNING(operation, "operation performed on a required component: " +
                                       Utilities::getTypeName<Component>() + "!");
    }

    template <typename T> void debugCheckForConflictingTags()
    {
        static_assert(!(Utilities::isStacked<T>() && Utilities::isNotStacked<T>()),
                      "Conflicting tags detected!");
    }
#endif

    template <typename T, typename Id> std::tuple<Components<T> &> getComponentsHelper(auto &cSet, Id id)
    {
        return std::tuple<Components<T> &>{getOrCreateComponent<T>(cSet, id)};
    }

    template <typename T, typename Id, typename... Rest>
    auto getComponentsHelper(auto &cSet, Id id, Rest... rest)
    {
        Components<T> &firstComponent = getOrCreateComponent<T>(cSet, id);
        std::tuple<Components<T> &> restComponents = getComponentsHelper<T>(cSet, rest...);

        return std::tuple_cat(std::tuple<Components<T> &>(firstComponent), restComponents);
    }

    /*
     * @brief Iterate over specified component sets to cleanup empty sets
     *
     * @param compHash - Hash of component to prune
     */
    template <typename T = void *> void prune(size_t compHash)
    {
        // TODO Task & Performance : Evaluate memory usage and performance gains
        auto iter = getStoredComponents().find(compHash);
        if (iter == getStoredComponents().end())
            return;

        auto &cSet = castErasedTo<T>(iter);
        cSet.prune();
        if (!cSet.size())
            getStoredComponents().erase(iter);
    }

    template <typename T> ComponentSet<T> &getComponentSet()
    {
        return getComponentSet<T>(m_standardSetSize);
    }

    template <typename T> ComponentSet<T> *getComponentSetPtr()
    {
        auto hash = getComponentHash<T>();
        auto iter = getStoredComponents().find(hash);
        if (iter == getStoredComponents().end())
            return nullptr;

        return &castErasedTo<T>(iter);
    }

    template <typename T> ComponentSet<T> &getComponentSet(size_t maxSize)
    {
        auto hash = getComponentHash<T>();
        auto iter = getStoredComponents().find(hash);
        if (iter == getStoredComponents().end())
        {
            createComponentSet<T>(maxSize);
            iter = getStoredComponents().find(hash);
        }
        return castErasedTo<T>(iter);
    }

    template <typename T> Components<T> &getComponents(EntityId eId)
    {
        auto &cSet = getComponentSet<T>();
        if (!cSet)
            ECS_ASSERT(!Utilities::isRequired<T>(), Utilities::getTypeName<T>() + " is a required component!")

        return getOrCreateComponent<T>(cSet, eId);
    }

    template <typename T> Components<T> &getOrCreateComponent(ComponentSet<T> &cSet, EntityId eId)
    {
#ifdef ecs_allow_debug
        debugCheckForConflictingTags<T>();
#endif
        auto comps = cSet.get(eId);
        if (!comps)
        {
            Components<T> dummy{Components<T>::ComponentFlags::EMPTY};
            if (cSet.isLocked())
            {
                cSet.unlock();
                cSet.insert(eId, std::move(dummy));
                cSet.lock();
            }
            else
                cSet.insert(eId, std::move(dummy));

            comps = cSet.get(eId);
        }

        return *comps;
    }

    template <typename T, typename... Args> void addComponent(EntityId eId, Args... args)
    {
        ComponentSet<T> &cSet = getComponentSet<T>();
        ECS_ASSERT(!cSet.isLocked(),
                   "Attempt to add to a locked component set for " + Utilities::getTypeName<T>())

        auto comps = cSet.get(eId);
        if (!comps)
        {
            auto newCompsPtr = cSet.emplace(eId, args...);
            if (!newCompsPtr)
                return;

            setTransformer(eId, *newCompsPtr);
            return;
        }

        if (!Utilities::shouldStack<T>() && comps->size() >= 1)
        {
            ECS_LOG_WARNING(eId, "Already contains a NoStack-tagged ", Utilities::getTypeName<T>(),
                            "Add failed!");

            return;
        }

        comps->emplace_back(args...);
        setTransformer(eId, *comps);
    }

    template <typename T, typename... Args>
    void overwriteComponent(EntityId eId, ComponentSet<T> &cSet, Args... args)
    {
        auto comps = cSet.get(eId);
        if (!comps)
        {
            ECS_LOG_WARNING(eId, "does not contain", Utilities::getTypeName<T>(), "Overwrite failed!");
            return;
        }

        auto newComps = Components<T>(args...);
        cSet.overwrite(eId, std::move(newComps));
    }

    template <typename T> void createComponentSet(size_t maxSize)
    {
#ifdef ecs_allow_debug
        debugCheckForConflictingTags<T>();
#endif

        auto componentHash = getComponentHash<T>();
        auto cSet = std::make_unique<ComponentSet<T>>(maxSize, m_standardSetSize);
        getStoredComponents().insert({componentHash, std::move(cSet)});

        auto tagHashes = getTagHashes<T>();

        for (const auto &tagHash : tagHashes)
        {
            if (!tagHash)
                continue;

            auto tagIter = m_tagMap.find(tagHash);
            if (tagIter == m_tagMap.end())
                tagIter = m_tagMap.emplace(tagHash, std::unordered_set<size_t>()).first;

            tagIter->second.insert(componentHash);
        }
    }

    template <typename... Ts> void clearComponents()
    {
        (
            [&]() {
                // TODO Task : Update to support more tags
                if constexpr (std::is_same_v<Ts, Tags::Event>)
                    clearComponentsByTag<Ts>();
                else
                    getStoredComponents().erase(getComponentHash<Ts>());
            }(),
            ...);
    }

    template <typename Tag> void clearComponentsByTag()
    {
        auto tagHashes = getTagHashes<Tag>();
        for (auto &tagHash : tagHashes)
        {
            if (!tagHash || m_tagMap.find(tagHash) == m_tagMap.end())
                continue;

            for (auto &componentHash : m_tagMap[tagHash])
                getStoredComponents().erase(componentHash);

            m_tagMap.erase(tagHash);
        }
    }

    template <typename... Ts> void clearEntityComponent(EntityId eId)
    {
        // TODO Performance : See if there better way to do this
        (getComponentSet<Ts>(m_minSetSize).erase(eId), ...);
    }

    template <typename Tag, typename Func> void eachComponentByTag(Func fn)
    {
        auto tagHashes = getTagHashes<Tag>();
        for (auto &tagHash : tagHashes)
        {
            if (!tagHash)
                continue;

            auto tagIter = m_tagMap.find(tagHash);
            if (tagIter == m_tagMap.end())
                continue;

            auto &hashSet = tagIter->second;
            for (auto &componentHash : hashSet)
            {
                auto compIter = getStoredComponents().find(componentHash);
                if (compIter == getStoredComponents().end())
                    continue;

                castErasedTo<Tag>(compIter).each(fn);
            }
        }
    }

    template <typename T> size_t getComponentHash() const
    {
        return typeid(T).hash_code();
    }

    template <typename T> const std::array<size_t, 7> getTagHashes() const
    {
        return {
            Utilities::isEvent<T>() ? typeid(Tags::Event).hash_code() : 0,
            Utilities::isEffect<T>() ? typeid(Tags::Effect).hash_code() : 0,
            Utilities::isNotStacked<T>() ? typeid(Tags::NoStack).hash_code() : 0,
            Utilities::isStacked<T>() ? typeid(Tags::Stack).hash_code() : 0,
            Utilities::isTransform<T>() ? typeid(Tags::Transform).hash_code() : 0,
            Utilities::isRequired<T>() ? typeid(Tags::Required).hash_code() : 0,
            Utilities::isUnique<T>() ? typeid(Tags::Unique).hash_code() : 0,
        };
    }

    template <typename T> ComponentSet<T> &castErasedTo(StoredComponents::iterator &iter)
    {
#ifdef ecs_unsafe_casts
        return *static_cast<ComponentSet<T> *>(&getSetFromIterator(iter));
#else
        auto casted = dynamic_cast<ComponentSet<T> *>(&getSetFromIterator(iter));
        ECS_ASSERT(casted, Utilities::getTypeName<T>() + " Failed dynamic_cast!")

        return *casted;
#endif
    }

    template <typename T> void setTransformer(EntityId eId, Components<T> &comps)
    {
        TransformationFn<T> *transformFnPtr = getTransformation<T>(eId);
        if (!transformFnPtr)
            return;

        auto &transformationFn = *transformFnPtr;
        comps.setTransformer(
            [&, eId, transformationFn](T &component) -> T { return transformationFn(eId, component); });
    }

    template <typename T> TransformationFn<T> *getTransformation(EntityId eId)
    {
        auto hash = getComponentHash<T>();
        auto iter = m_transformationMap.find(hash);

        if (iter == m_transformationMap.end())
            return nullptr;

        auto &uncastedFn = iter->second;
        return &reinterpret_cast<TransformationFn<T> &>(uncastedFn);
    }

    StoredComponents &getStoredComponents()
    {
        return m_componentMap;
    }

    ErasedComponentSet &getSetFromIterator(StoredComponents::iterator &iter)
    {
        return *iter->second;
    }

  private:
    StoredComponents m_componentMap{};
    StoredTags m_tagMap{};
    StoredTransformationFnMap m_transformationMap{};
    EntityId m_nextEntityId{0};

    size_t m_standardSetSize = 10024;
    size_t m_minSetSize = 100;

#ifdef ecs_allow_experimental
  public:
    /*
     * !!! NEED MORE TESTING TO BE SURE THAT THIS WORKS !!!
     * @brief Iterate over every component set to cleanup empty sets
     */
    void pruneAll()
    {
        for (auto iter = getStoredComponents().begin(); iter != getStoredComponents().end();)
        {
            std::vector<EntityId> ids;

            auto &cSet = getSetFromIterator(iter);
            cSet.prune();

            if (iter->first, !cSet.size())
            {
                iter = getStoredComponents().erase(iter);
                continue;
            }

            ++iter;
        }
    }

    /*
     * !!! CAUSES UNDEFINED BEHAVIOR - DO NOT USE !!!
     * // TODO Task : See if casting can be updated to make this a
     * // viable method without undefined behavior
     */
    template <typename Tag, typename Func> void eachByTag(Func fn)
    {
        eachComponentByTag<Tag>(fn);
    }

    /*
     * !!! POTENTIALLY CAUSES UNDEFINED BEHAVIOR !!!
     * @brief Iterate over component sets by tag to cleanup empty components
     *
     * Iterate over specific sets by tag to either remove stale entities, or
     * delete the entire set if all entity components within are stale.
     */
    template <typename Tag> void pruneByTag()
    {
        auto tagHashes = getTagHashes<Tag>();
        for (auto &tagHash : tagHashes)
        {
            if (!tagHash)
                continue;

            auto tagIter = m_tagMap.find(tagHash);
            if (tagIter == m_tagMap.end())
                continue;

            auto &hashSet = tagIter->second;
            for (auto &componentHash : hashSet)
            {
                prune<Tag>(componentHash);
            }
        }
    }

#endif

#ifdef ecs_allow_unsafe
  public:
#else
  private:
#endif
    template <typename... Ts> [[nodiscard]] std::tuple<Components<Ts> *...> getUnsafe(EntityId eId)
    {
        std::tuple<Components<Ts> *...> components{};

        (
            [&]() {
                auto cSetPtr = getComponentSetPtr<Ts>();
                if (!cSetPtr)
                {
                    std::get<Components<Ts> *>(components) = nullptr;
                    return;
                }

                std::get<Components<Ts> *>(components) = cSetPtr->get(eId);
            }(),
            ...);

        return components;
    }

    template <typename... Ts> [[nodiscard]] std::tuple<ComponentSet<Ts> *...> getAllUnsafe()
    {
        return {getComponentSetPtr<Ts>()...};
    }
};
}; // namespace internal
}; // namespace ECS
