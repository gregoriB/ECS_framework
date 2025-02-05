#pragma once

#include "../../example/invaders/core.hpp"
#include "../helpers/components.hpp"
#include "../helpers/utils.hpp"
#include <cassert>

inline void test_get_component(ECM &ecm)
{
    PRINT("TESTING MANAGER GET METHOD")

    EntityId id = 2;
    ecm.add<TestNonStackedComp>(id);

    auto &testStack = ecm.get<TestNonStackedComp>(id);

    assert(testStack.size() == 1);
}

inline void test_gather_component(ECM &ecm)
{
    PRINT("TESTING MANAGER GATHER METHOD")

    EntityId id = 2;
    ecm.add<TestNonStackedComp>(id);
    ecm.add<TestStackedComp>(id);
    ecm.add<TestStackedComp>(id);

    auto [nonStackedComps, stackedComps] = ecm.gather<TestNonStackedComp, TestStackedComp>(id);

    assert(nonStackedComps.size() == 1);
    assert(stackedComps.size() == 2);
}

inline void test_component_mutate_fn(ECM &ecm)
{
    PRINT("TESTING COMPONENT MUTATE METHOD")

    EntityId id = 2;
    ecm.add<TestStackedComp>(id);
    ecm.add<TestStackedComp>(id);
    auto &testStack = ecm.get<TestStackedComp>(id);

    testStack.mutate([&](TestStackedComp &testStacked) { testStacked.val = 100; });

    auto ptrVec = testStack.getUnsafe();

    assert((*ptrVec[0]).val == 100);
}

inline void test_component_inspect_fn(ECM &ecm)
{
    PRINT("TESTING COMPONENT INSPECT METHOD")

    EntityId id = 2;
    ecm.add<TestStackedComp>(id);
    ecm.add<TestStackedComp>(id);
    auto &testStack = ecm.get<TestStackedComp>(id);
    int count{0};

    testStack.inspect([&](const TestStackedComp &testStacked) {
        count++;
        assert(testStacked.message == "this is a stacked component");
    });

    assert(count == 2);
}

inline void test_component_remove_fn(ECM &ecm)
{
    PRINT("TESTING COMPONENT REMOVE METHOD")

    EntityId id = 2;
    ecm.add<TestStackedComp>(id);
    ecm.add<TestStackedComp>(id);
    auto &testStack = ecm.get<TestStackedComp>(id);

    assert(testStack.size() == 2);

    testStack.remove([&](const TestStackedComp &testStacked) { return true; });

    assert(testStack.size() == 0);
}

inline void test_component_remove_conditionally(ECM &ecm)
{
    PRINT("TESTING COMPONENT REMOVE CONDITIONALLY")

    EntityId id = 2;
    ecm.add<TestStackedComp>(id, 1);
    ecm.add<TestStackedComp>(id, 2);
    ecm.add<TestStackedComp>(id, 3);
    auto &testStack = ecm.get<TestStackedComp>(id);

    assert(testStack.size() == 3);

    testStack.remove([&](const TestStackedComp &testStacked) { return testStacked.val == 2; });

    assert(testStack.size() == 2);
}

#ifdef ecs_allow_experimental
inline void test_effect_cleanup_timed(ECM &ecm)
{
    PRINT("TESTING EFFECT CLEANUP TIMED")

    EntityId id = 2;
    ecm.add<TestEffectCompTimed>(id, 0.0f);

    ecm.eachByTag<Tags::Effect>(
        [&](EntityId eId, auto &effectComps) { effectComps.remove(isEffectExpired); });

    auto &testEffectTimed = ecm.get<TestEffectCompTimed>(id);
    assert(testEffectTimed.size() == 0);
}

inline void test_effect_cleanup(ECM &ecm)
{
    PRINT("TESTING EFFECT CLEANUP")

    EntityId id = 2;
    ecm.add<TestEffectComp>(id);
    auto &testEffect = ecm.get<TestEffectComp>(id);
    testEffect.mutate(markForCleanup);
    ecm.eachByTag<Tags::Effect>(
        [&](EntityId eId, auto &effectComps) { effectComps.remove(isEffectExpired); });

    testEffect.mutate([&](TestEffectComp &testEffect) { testEffect.value = 2; });

    assert(testEffect.size() == 0);
}

inline void test_effect_cleanup_only_effect_components(ECM &ecm)
{
    PRINT("TESTING EFFECT CLEANUP ONLY EFFECTS")

    EntityId id = 2;
    ecm.add<TestEventComp>(id);
    ecm.add<TestTransformComp>(id);
    ecm.add<TestNonStackedComp>(id);

    ecm.eachByTag<Tags::Effect>(
        [&](EntityId eId, auto &effectComps) { effectComps.remove(isEffectExpired); });

    auto &testEvents = ecm.get<TestEventComp>(id);
    auto &testnonstacks = ecm.get<TestNonStackedComp>(id);
    auto &testTransformeds = ecm.get<TestTransformComp>(id);

    testEvents.mutate(
        [&](TestEventComp &testEvent) { assert(testEvent.message == "this is an event component"); });
    testnonstacks.mutate([&](TestNonStackedComp &testnonstack) {
        assert(testnonstack.message == "this is a nonStacked component");
    });
    testTransformeds.mutate([&](TestTransformComp &testTransformed) {
        assert(testTransformed.message == "this is a transform component");
    });

    assert(testEvents.size() == 1);
    assert(testnonstacks.size() == 1);
    assert(testTransformeds.size() == 1);
}
#endif

inline void test_add_non_stack_component(ECM &ecm)
{
    PRINT("TESTING ADD NON STACKED COMPONENT")

    EntityId id = 2;
    assert(ecm.get<TestNonStackedComp>(id).size() == 0);

    ecm.add<TestNonStackedComp>(id);
    auto &comp = ecm.get<TestNonStackedComp>(id);

    assert(comp.size() == 1);
}

inline void test_add_more_non_stack_components_fail(ECM &ecm)
{
    PRINT("TESTING ADD MORE NON STACKED COMPONENTS FAIL")

    EntityId id = 2;
    assert(ecm.get<TestNonStackedComp>(id).size() == 0);

    ecm.add<TestNonStackedComp>(id);
    auto &comp = ecm.get<TestNonStackedComp>(id);

    assert(comp.size() == 1);

    ecm.add<TestNonStackedComp>(id);

    assert(comp.size() == 1);
}

inline void test_add_stacked_components(ECM &ecm)
{
    PRINT("TESTING ADD STACKED COMPONENTS")

    EntityId id = 2;
    auto &comp = ecm.get<TestStackedComp>(id);
    assert(comp.size() == 0);

    ecm.add<TestStackedComp>(id);

    assert(comp.size() == 1);

    ecm.add<TestStackedComp>(id);

    assert(comp.size() == 2);
}

inline void test_add_event_components(ECM &ecm)
{
    PRINT("TESTING ADD EVENT COMPONENTS")

    EntityId id = 2;
    assert(ecm.get<TestEventComp>(id).size() == 0);

    ecm.add<TestEventComp>(id);

    auto &comp = ecm.get<TestEventComp>(id);
    assert(comp.size() == 1);

    ecm.add<TestEventComp>(id);

    assert(comp.size() == 2);
}

inline void test_add_effect_components(ECM &ecm)
{
    PRINT("TESTING ADD EFFECT COMPONENTS")

    EntityId id = 2;
    assert(ecm.get<TestEffectComp>(id).size() == 0);

    ecm.add<TestEffectComp>(id);

    auto &comp = ecm.get<TestEffectComp>(id);
    assert(comp.size() == 1);

    ecm.add<TestEffectComp>(id);

    assert(comp.size() == 2);
}

inline void test_clear_all_components(ECM &ecm)
{
    PRINT("TESTING CLEAR ALL COMPONENTS")

    EntityId id1 = 1;
    EntityId id2 = 2;
    EntityId id3 = 3;
    ecm.add<TestEventComp>(id1);
    ecm.add<TestStackedComp>(id2);
    ecm.add<TestNonStackedComp>(id3);

    ecm.clear<TestEventComp, TestStackedComp, TestNonStackedComp>();

    assert(ecm.getAll<TestEventComp>().size() == 0);
    assert(ecm.getAll<TestStackedComp>().size() == 0);
    assert(ecm.getAll<TestNonStackedComp>().size() == 0);
}

inline void test_clear_components_by_tag(ECM &ecm)
{
    PRINT("TESTING CLEAR COMPONENTS BY TAG")

    EntityId id1 = 1;
    EntityId id2 = 2;
    EntityId id3 = 3;
    ecm.add<TestEventComp>(id1);
    ecm.add<TestEventComp>(id2);
    ecm.add<TestEventComp>(id3);

    assert(ecm.getAll<TestEventComp>().size() == 3);

    ecm.clearByTag<Tags::Event>();

    assert(ecm.getAll<TestEventComp>().size() == 0);
}

inline void test_clear_all_by_entity(ECM &ecm)
{
    PRINT("TESTING CLEAR ALL COMPONENTS BY ENTITY ID")

    EntityId id1 = 1;
    EntityId id2 = 2;
    EntityId id3 = 3;

    ecm.add<TestEventComp>(id1);
    ecm.add<TestEffectComp>(id1);
    ecm.add<TestStackedComp>(id1);
    ecm.add<TestNonStackedComp>(id1);

    ecm.add<TestEventComp>(id2);
    ecm.add<TestEffectComp>(id2);
    ecm.add<TestStackedComp>(id2);
    ecm.add<TestNonStackedComp>(id2);

    ecm.add<TestEventComp>(id3);
    ecm.add<TestEffectComp>(id3);
    ecm.add<TestStackedComp>(id3);
    ecm.add<TestNonStackedComp>(id3);

    assert(ecm.getAll<TestEventComp>().size() == 3);
    assert(ecm.getAll<TestEffectComp>().size() == 3);
    assert(ecm.getAll<TestStackedComp>().size() == 3);
    assert(ecm.getAll<TestNonStackedComp>().size() == 3);

    ecm.clearEntity(id2);

    assert(ecm.getAll<TestEventComp>().size() == 2);
    assert(ecm.getAll<TestEffectComp>().size() == 2);
    assert(ecm.getAll<TestStackedComp>().size() == 2);
    assert(ecm.getAll<TestNonStackedComp>().size() == 2);

    assert(ecm.get<TestNonStackedComp>(id2).size() == 0);
    assert(ecm.get<TestEventComp>(id2).size() == 0);
    assert(ecm.get<TestEffectComp>(id2).size() == 0);
    assert(ecm.get<TestStackedComp>(id2).size() == 0);
}

inline void test_prune(ECM &ecm)
{
    PRINT("TESTING PRUNE")

    EntityId id{1};
    ecm.add<TestEffectComp>(id, 1);
    ecm.add<TestEffectComp>(id, 2);

    auto &compsSet = ecm.getAll<TestEffectComp>();
    assert(compsSet.size() == 1);

    auto &comps = ecm.get<TestEffectComp>(id);
    comps.remove([&](const TestEffectComp &testComp) { return true; });

    assert(comps.size() == 0);

    ecm.prune<TestEffectComp>();

    assert(ecm.getAll<TestEffectComp>().size() == 0);
}

inline void test_prune_multi(ECM &ecm)
{
    PRINT("TESTING PRUNE MULTIPLE")

    EntityId id{1};
    ecm.add<TestEffectComp>(id, 1);
    ecm.add<TestNonStackedComp>(id, 1);
    ecm.add<TestStackedComp>(id, 1);

    auto &effectComps = ecm.get<TestEffectComp>(id);
    effectComps.remove([&](const TestEffectComp &testComp) { return true; });
    auto &nonstackComps = ecm.get<TestNonStackedComp>(id);
    nonstackComps.remove([&](const TestNonStackedComp &testComp) { return true; });
    auto &stackedComps = ecm.get<TestStackedComp>(id);
    stackedComps.remove([&](const TestStackedComp &testComp) { return true; });

    assert(effectComps.size() == 0);
    assert(nonstackComps.size() == 0);
    assert(stackedComps.size() == 0);

    ecm.prune<TestEffectComp, TestStackedComp, TestNonStackedComp>();

    assert(ecm.getAll<TestEffectComp>().size() == 0);
    assert(ecm.getAll<TestNonStackedComp>().size() == 0);
    assert(ecm.getAll<TestStackedComp>().size() == 0);
}

#ifdef ecs_allow_experimental
inline void test_prune_all(ECM &ecm)
{
    PRINT("TESTING PRUNE ALL")

    EntityId id{1};
    ecm.add<TestEffectComp>(id, 1);
    ecm.add<TestEffectComp>(id, 2);
    ecm.add<TestNonStackedComp>(id, 3);
    ecm.add<TestNonStackedComp>(id, 4);

    auto &compsSet = ecm.getAll<TestEffectComp>();
    assert(compsSet.size() == 1);

    auto &comps1 = ecm.get<TestEffectComp>(id);
    comps1.remove([&](const TestEffectComp &testComp) { return true; });
    auto &comps2 = ecm.get<TestNonStackedComp>(id);
    comps2.remove([&](const TestNonStackedComp &testComp) { return true; });

    assert(comps1.size() == 0);
    assert(comps2.size() == 0);

    ecm.pruneAll();

    assert(ecm.getAll<TestEffectComp>().size() == 0);
    assert(ecm.getAll<TestNonStackedComp>().size() == 0);
}
#endif

inline void test_sparse_set_auto_prune(ECM &ecm)
{
    PRINT("TESTING AUTO PRUNE")

    EntityId id{1};
    ecm.add<TestEffectComp>(id, 1);
    ecm.add<TestEffectComp>(id, 2);

    auto &compsSet = ecm.getAll<TestEffectComp>();
    assert(compsSet.size() == 1);

    auto &comps = ecm.get<TestEffectComp>(id);
    comps.remove([&](const TestEffectComp &testComp) { return true; });

    assert(comps.size() == 0);

    int count{};
    compsSet.each([&](EId eId, auto &testComps) { count++; });

    assert(ecm.getAll<TestEffectComp>().size() == 0);
}

inline void test_sparse_set_auto_prune_after_removal(ECM &ecm)
{
    PRINT("TESTING AUTO PRUNE AFTER REMOVE")

    EntityId id{1};
    ecm.add<TestEffectComp>(id, 1);
    ecm.add<TestEffectComp>(id, 2);

    auto &compsSet = ecm.getAll<TestEffectComp>();
    assert(compsSet.size() == 1);

    compsSet.each(
        [&](EId eId, auto &comps) { comps.remove([&](const TestEffectComp &testComp) { return true; }); });

    assert(ecm.getAll<TestEffectComp>().size() == 0);
}

#ifdef ecs_allow_experimental
inline void test_prune_by_tag(ECM &ecm)
{
    PRINT("TESTING PRUNE BY TAG")

    EntityId id1{1};
    EntityId id2{2};
    EntityId id3{3};
    EntityId id4{4};
    EntityId id5{5};
    ecm.add<TestEffectComp>(id1, 1);
    ecm.add<TestEffectComp>(id1, 2);

    ecm.add<TestEffectComp>(id2, 1);
    ecm.add<TestEffectComp>(id2, 2);

    ecm.add<TestEffectCompTimed>(id3);
    ecm.add<TestEffectCompTimed>(id3, 3);
    /* ecm.add<TestEffectCompTimed>(id3, 0.0f); */

    ecm.add<TestEffectCompTimed>(id4);
    ecm.add<TestEffectCompTimed>(id4, 4);

    ecm.add<TestEffectCompTimed>(id5);
    ecm.add<TestEffectCompTimed>(id5, 5);

    auto &untimedEffectSet = ecm.getAll<TestEffectComp>();
    auto &timedEffectSet = ecm.getAll<TestEffectCompTimed>();

    assert(untimedEffectSet.size() == 2);
    assert(timedEffectSet.size() == 3);
    assert(ecm.get<TestEffectComp>(id1).size() == 2);
    assert(ecm.get<TestEffectComp>(id2).size() == 2);
    assert(ecm.get<TestEffectCompTimed>(id3).size() == 2);
    assert(ecm.get<TestEffectCompTimed>(id4).size() == 2);

    ecm.get<TestEffectComp>(id1).remove([&](auto &testComp) { return true; });
    ecm.get<TestEffectComp>(id2).remove([&](auto &testComp) { return testComp.value == 2; });

    ecm.get<TestEffectCompTimed>(id3).remove([&](auto &testComp) { return testComp.value == 3; });
    ecm.get<TestEffectCompTimed>(id4).remove([&](auto &testComp) { return true; });
    ecm.get<TestEffectCompTimed>(id5).remove([&](auto &testComp) { return testComp.value == 5; });

    assert(ecm.get<TestEffectComp>(id1).size() == 0);
    assert(ecm.get<TestEffectComp>(id2).size() == 1);
    assert(ecm.get<TestEffectCompTimed>(id3).size() == 1);
    assert(ecm.get<TestEffectCompTimed>(id4).size() == 0);
    assert(ecm.get<TestEffectCompTimed>(id5).size() == 1);

    assert(untimedEffectSet.size() == 2);
    assert(timedEffectSet.size() == 3);

    ecm.pruneByTag<Tags::Effect>();

    assert(untimedEffectSet.size() == 1);
    assert(timedEffectSet.size() == 2);
}
#endif
