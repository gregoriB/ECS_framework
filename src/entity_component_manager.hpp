#pragma once

#include "components.hpp"
#include "core.hpp"
#include "sparse_set.hpp"
#include "tags.hpp"
#include "utilities.hpp"
#include <stdexcept>

template <typename EntityId> class EntityComponentManager
{
  private:
    template <typename T> using ComponentSet = SparseSet<EntityId, Components<T>>;
    template <typename T> using ComponentSetMap = std::unordered_map<size_t, std::unique_ptr<T>>;

    using ErasedComponent = Components<DefaultComponent>;
    using ErasedComponentSet = BaseSparseSet<EntityId, ErasedComponent>;

    using StoredComponents = ComponentSetMap<ErasedComponentSet>;
    using StoredTags = std::unordered_map<size_t, std::unordered_set<size_t>>;

    template <typename T> using TransformationFn = std::function<T(EntityId, T)>;
    using StoredTransformationFn = std::function<DefaultComponent(EntityId, DefaultComponent &)>;
    using StoredTransformationFnMap = std::unordered_map<size_t, StoredTransformationFn>;

  public:
    // TODO Task : Move default value to configurable file or a compiler flag
    size_t m_standardSetSize = 10024;
    size_t m_minSetSize = 100;

  public:
    EntityComponentManager()
    {
    }

    EntityId createEntity()
    {
        return m_nextEntityId++;
    }

    /**
     * @brief Constructs and add a component to a set by entity id
     *
     * @param Entity Id
     * @param Variable arguments for the component constructor
     *
     * If the component set has not yet been created, it is created before
     * the entity component is added.
     */
    template <typename T, typename... Args> void add(EntityId eId, Args... args)
    {
        addComponent<T>(eId, args...);
    }

    template <typename T, typename... Args> void addUnique(EntityId eId, Args... args)
    {
        addComponent<T>(eId, args...);
        getComponentSet<T>(m_minSetSize).lock();
    }

    template <typename T, typename... Args> void overwriteUnique(EntityId eId, Args... args)
    {
        auto [uniqueId, _] = getUnique<T>();

        if (eId != uniqueId)
            throw std::runtime_error("Enitty ID: " + std::to_string(eId) +
                                     " is not owning entity for unique component: " + getTypeName<T>());

        auto &cSet = getComponentSet<T>();
        if (!cSet.isLocked())
            throw std::runtime_error(getTypeName<T>() + " is not unique!");

        overwriteComponent<T>(eId, cSet, args...);
    }

    template <typename T, typename... Args> void overwrite(EntityId eId, Args... args)
    {
        auto &cSet = getComponentSet<T>();
        if (cSet.isLocked())
            throw std::runtime_error(
                getTypeName<T>() +
                " is unique. Cannot overwrite. Use `overwriteUnique` to overwrite a unique component");

        overwriteComponent<T>(eId, cSet, args...);
    }

    /**
     * @brief Get a specified component for the entity
     *
     * @param Entity Id
     *
     * @return Entity component reference
     */
    template <typename T> [[nodiscard]] Components<T> &get(EntityId eId)
    {
        return getComponents<T>(eId);
    }

    /**
     * @brief Get a specified component for multiple entities
     *
     * @params Entity IDs
     *
     * @return Tuple of component references
     */
    template <typename T, typename... Ids> [[nodiscard]] auto get(EntityId id, Ids... ids)
    {
        auto &cSet = getComponentSet<T>();
        return getComponentsHelper<T>(cSet, id, ids...);
    }

    template <typename... T> [[nodiscard]] std::tuple<Components<T> &...> gather(EntityId eId)
    {
        return {getComponents<T>(eId)...};
    }

    /*
     * @brief Get the id for an entity which is unique for the type
     *
     * @return Entity Id
     */
    template <typename T> [[nodiscard]] std::pair<EntityId, Components<T> &> getUnique()
    {
        auto &cSet = getComponentSet<T>(m_minSetSize);
        if (!cSet.size())
            throw std::runtime_error(getTypeName<T>() + " unique component set does not exist!");

        if (!cSet.isLocked())
            throw std::runtime_error(getTypeName<T>() + " is not unique!");

        EntityId id{0};
        Components<T> *compsPtr = nullptr;
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

        return {id, *compsPtr};
    }

    template <typename T> [[nodiscard]] const std::vector<EntityId> &getEntityIds()
    {
        return getComponentSet<T>(m_minSetSize).getIds();
    }

    template <typename T> [[nodiscard]] ComponentSet<T> &getAll()
    {
        return getComponentSet<T>(m_minSetSize);
    }

    template <typename... T> [[nodiscard]] std::tuple<ComponentSet<T> &...> gatherAll()
    {
        return {getComponentSet<T>(m_minSetSize)...};
    }

    template <typename T> [[nodiscard]] bool isUnique() const
    {
        return getComponentSet<T>().isLocked();
    }

    template <typename... Components> void clear()
    {
        clearComponents<Components...>();
    }

    template <typename Tag> void clearByTag()
    {
        clearComponentsByTag<Tag>();
    }

    template <typename... Components> void clearByEntity(EntityId eId)
    {
        clearEntityComponent<Components...>(eId);
    }

    void clearAllByEntity(EntityId eId)
    {
        for (auto iter = getStoredComponents().begin(); iter != getStoredComponents().end(); ++iter)
        {
            auto &cSet = getSetFromIterator(iter);
            cSet.erase(eId);
        }
    }

    template <typename... Components> void prune()
    {
        (prune<Components>(getComponentHash<Components>()), ...);
    }

    /*
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

    template <typename T> constexpr void registerTransformation(TransformationFn<T> transformationFn)
    {
        auto casted = reinterpret_cast<StoredTransformationFn &>(transformationFn);
        m_transformationMap.emplace(getComponentHash<T>(), std::move(casted));
    }

#ifdef game_allow_debug
    /*
     * !!! CURRENTLY DOES NOT WORK - USE AUTO PRUNE OR MANUAL PRUNING INSTEAD !!!
     * @brief Iterate over every component set to cleanup empty sets
     *
     * Heavy-handed approach.  Iterate over every set and delete the
     * entire set if all entities are stale.
     */
    // TODO Task : Make this work. May need to be casted to work properly.
    void pruneAll()
    {
        for (auto iter = getStoredComponents().begin(); iter != getStoredComponents().end();)
        {
            std::vector<EntityId> ids;

            auto &cSet = getSetFromIterator(iter);
            cSet.eachWithEmpty([&](EntityId eId, auto &components) {
                if (!components.size())
                    ids.push_back(eId);
            });

            if (!cSet.size() || ids.size() == cSet.size())
            {
                iter = getStoredComponents().erase(iter);
                continue;
            }

            for (const auto &id : ids)
                cSet.erase(id);

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
#endif

    EntityComponentManager(const EntityComponentManager &) = delete;
    EntityComponentManager &operator=(const EntityComponentManager &) = delete;

  private:
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

        std::vector<EntityId> ids;
        auto &cSet = castErasedTo<T>(iter);
        cSet.eachWithEmpty([&](EntityId eId, auto &components) {
            if (!components.size())
                ids.push_back(eId);
        });

        if (ids.size() == cSet.size())
        {
            getStoredComponents().erase(iter);
            return;
        }

        for (const auto &id : ids)
            cSet.erase(id);

        if (!cSet.size())
            getStoredComponents().erase(iter);
    }

    template <typename T> ComponentSet<T> &getComponentSet()
    {
        return getComponentSet<T>(m_standardSetSize);
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
        return getOrCreateComponent<T>(cSet, eId);
    }

    template <typename T> Components<T> &getOrCreateComponent(ComponentSet<T> &cSet, EntityId eId)
    {
        auto comps = cSet.get(eId);
        if (!comps)
        {

            Components<T> dummy{ComponentFlags::EMPTY};
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
        if (cSet.isLocked())
            throw std::runtime_error("Attempt to add to a unique component set for " + getTypeName<T>());

        auto comps = cSet.get(eId);
        if (!comps)
        {
            auto newCompsPtr = cSet.emplace(eId, args...);
            if (!newCompsPtr)
                return;

            setTransformer(eId, *newCompsPtr);
            return;
        }

        if (!Tags::isStacked<T>() && comps->size() >= 1)
        {
            ECS_LOG_WARNING(eId, "Already contains a unique", getTypeName<T>(), "Add failed");

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
            ECS_LOG_WARNING(eId, "does not contain", getTypeName<T>(), "Overwrite failed");
            return;
        }

        auto newComps = Components<T>(args...);
        cSet.overwrite(eId, std::move(newComps));
    }

    template <typename T> void createComponentSet(size_t maxSize)
    {
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

    template <typename... Components> void clearComponents()
    {
        (getStoredComponents().erase(getComponentHash<Components>()), ...);
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

    template <typename... Components> void clearEntityComponent(EntityId eId)
    {
        // TODO Performance : See if there better way to do this
        (getComponentSet<Components>(m_minSetSize).erase(eId), ...);
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

    template <typename T> const std::array<size_t, 4> getTagHashes() const
    {
        return {
            Tags::isEvent<T>() ? typeid(Tags::Event).hash_code() : 0,
            Tags::isEffect<T>() ? typeid(Tags::Effect).hash_code() : 0,
            Tags::isNotStacked<T>() ? typeid(Tags::NoStack).hash_code() : 0,
            Tags::isTransform<T>() ? typeid(Tags::Transform).hash_code() : 0,
        };
    }

    template <typename T> ComponentSet<T> &castErasedTo(StoredComponents::iterator &iter)
    {
#ifdef unsafe_casts
        return *static_cast<ComponentSet<T> *>(&getSetFromIterator(iter));
#else
        auto casted = dynamic_cast<ComponentSet<T> *>(&getSetFromIterator(iter));
        if (!casted)
        {
            auto errorMessage = getTypeName<T>() + " Failed dynamic_cast!";
            ECS_LOG_WARNING(errorMessage);
            throw std::runtime_error(errorMessage);
        }
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
    EntityId m_nextEntityId{static_cast<EntityId>(reservedEntities)};
};
