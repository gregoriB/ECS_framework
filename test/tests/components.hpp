#pragma once

#include "../core.hpp"
#include "../helpers/components.hpp"
#include "../helpers/utils.hpp"
#include <iostream>

inline void test_get_component(CM &cm)
{
    PRINT("TESTING MANAGER GET METHOD")

    EntityId id = 2;
    cm.add<TestNonStackedComp>(id);

    auto [testStack] = cm.get<TestNonStackedComp>(id);

    assert(testStack.size() == 1);
}

inline void test_gather_component(CM &cm)
{
    PRINT("TESTING MANAGER GATHER METHOD")

    EntityId id = 2;
    cm.add<TestNonStackedComp>(id);
    cm.add<TestStackedComp>(id);
    cm.add<TestStackedComp>(id);

    auto [nonStackedComps, stackedComps] = cm.get<TestNonStackedComp, TestStackedComp>(id);

    assert(nonStackedComps.size() == 1);
    assert(stackedComps.size() == 2);
}

inline void test_gather_group(CM &cm)
{
    PRINT("TESTING MANAGER GATHER GROUP METHOD")

    EntityId id1 = 1;
    EntityId id2 = 2;

    // Test group with 1 component and 2 entities
    cm.add<TestNonStackedComp>(id1);
    cm.add<TestNonStackedComp>(id2);
    auto group1 = cm.getGroup<TestNonStackedComp>();

    assert(group1.size() == 2);
    assert(group1.getIds().size() == 2);

    std::vector<EntityId> fromEach1;
    group1.each([&](EId eId, auto &testStackComps) { fromEach1.push_back(eId); });

    assert(fromEach1.size() == 2);

    // Test group with 2 components and 1 entity
    cm.add<TestStackedComp>(id2);
    auto group2 = cm.getGroup<TestNonStackedComp, TestStackedComp>();

    assert(group2.size() == 1);
    assert(group2.getIds().size() == 1);

    std::vector<EntityId> fromEach2;
    group2.each([&](EId eId, auto &testNonStackComps, auto &testStackComps) { fromEach2.push_back(eId); });

    assert(fromEach2.size() == 1);
    assert(fromEach2[0] == id2);
}

inline void test_component_mutate_fn(CM &cm)
{
    PRINT("TESTING COMPONENT MUTATE METHOD")

    EntityId id = 2;
    cm.add<TestStackedComp>(id);
    cm.add<TestStackedComp>(id);
    auto [testStack] = cm.get<TestStackedComp>(id);

    testStack.mutate([&](TestStackedComp &testStacked) { testStacked.val = 100; });

    auto ptrVec = testStack.unpack();

    assert((*ptrVec[0]).val == 100);
}

inline void test_component_inspect_fn(CM &cm)
{
    PRINT("TESTING COMPONENT INSPECT METHOD")

    EntityId id = 2;
    cm.add<TestStackedComp>(id);
    cm.add<TestStackedComp>(id);
    auto [testStack] = cm.get<TestStackedComp>(id);
    int count{0};

    testStack.inspect([&](const TestStackedComp &testStacked) {
        count++;
        assert(testStacked.message == "this is a stacked component");
    });

    assert(count == 2);
}

inline void test_component_remove_fn(CM &cm)
{
    PRINT("TESTING COMPONENT REMOVE METHOD")

    EntityId id = 2;
    cm.add<TestStackedComp>(id);
    cm.add<TestStackedComp>(id);
    auto [testStack] = cm.get<TestStackedComp>(id);

    assert(testStack.size() == 2);

    testStack.remove([&](const TestStackedComp &testStacked) { return true; });

    assert(testStack.size() == 0);
}

inline void test_component_remove_conditionally(CM &cm)
{
    PRINT("TESTING COMPONENT REMOVE CONDITIONALLY")

    EntityId id = 2;
    cm.add<TestStackedComp>(id, 1);
    cm.add<TestStackedComp>(id, 2);
    cm.add<TestStackedComp>(id, 3);
    auto [testStack] = cm.get<TestStackedComp>(id);

    assert(testStack.size() == 3);

    testStack.remove([&](const TestStackedComp &testStacked) { return testStacked.val == 2; });

    assert(testStack.size() == 2);
}

#ifdef ecs_allow_experimental
inline void test_effect_cleanup_timed(CM &cm)
{
    PRINT("TESTING EFFECT CLEANUP TIMED")

    EntityId id = 2;
    cm.add<TestEffectCompTimed>(id, 0.0f);

    cm.eachByTag<ECS::Tags::Effect>(
        [&](EntityId eId, auto &effectComps) { effectComps.remove(isEffectExpired); });

    auto [testEffectTimed] = cm.get<TestEffectCompTimed>(id);
    assert(testEffectTimed.size() == 0);
}

inline void test_effect_cleanup(CM &cm)
{
    PRINT("TESTING EFFECT CLEANUP")

    EntityId id = 2;
    cm.add<TestEffectComp>(id);
    auto [testEffect] = cm.get<TestEffectComp>(id);
    testEffect.mutate(markForCleanup);
    cm.eachByTag<ECS::Tags::Effect>(
        [&](EntityId eId, auto &effectComps) { effectComps.remove(isEffectExpired); });

    testEffect.mutate([&](TestEffectComp &testEffect) { testEffect.value = 2; });

    assert(testEffect.size() == 0);
}

inline void test_effect_cleanup_only_effect_components(CM &cm)
{
    PRINT("TESTING EFFECT CLEANUP ONLY EFFECTS")

    EntityId id = 2;
    cm.add<TestEventComp>(id);
    cm.add<TestTransformComp>(id);
    cm.add<TestNonStackedComp>(id);

    cm.eachByTag<ECS::Tags::Effect>(
        [&](EntityId eId, auto &effectComps) { effectComps.remove(isEffectExpired); });

    auto [testEvents] = cm.get<TestEventComp>(id);
    auto [testnonstacks] = cm.get<TestNonStackedComp>(id);
    auto [testTransformeds] = cm.get<TestTransformComp>(id);

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

inline void test_add_non_stack_component(CM &cm)
{
    PRINT("TESTING ADD NON STACKED COMPONENT")

    EntityId id = 2;
    assert(!cm.contains<TestNonStackedComp>(id));

    cm.add<TestNonStackedComp>(id);
    auto [comp] = cm.get<TestNonStackedComp>(id);

    assert(comp.size() == 1);
}

inline void test_add_more_non_stack_components_fail(CM &cm)
{
    PRINT("TESTING ADD MORE NON STACKED COMPONENTS FAIL")

    EntityId id = 2;

    cm.add<TestNonStackedComp>(id);
    auto [comp] = cm.get<TestNonStackedComp>(id);

    assert(comp.size() == 1);

    cm.add<TestNonStackedComp>(id);

    assert(comp.size() == 1);
}

inline void test_add_stacked_components(CM &cm)
{
    PRINT("TESTING ADD STACKED COMPONENTS")

    EntityId id = 2;
    auto [comp] = cm.get<TestStackedComp>(id);
    assert(comp.size() == 0);

    cm.add<TestStackedComp>(id);

    assert(comp.size() == 1);

    cm.add<TestStackedComp>(id);

    assert(comp.size() == 2);
}

inline void test_add_event_components(CM &cm)
{
    PRINT("TESTING ADD EVENT COMPONENTS")

    EntityId id = 2;
    assert(!cm.contains<TestEventComp>(id));

    cm.add<TestEventComp>(id);

    auto [comp] = cm.get<TestEventComp>(id);
    assert(comp.size() == 1);

    cm.add<TestEventComp>(id);

    assert(comp.size() == 2);
}

inline void test_add_effect_components(CM &cm)
{
    PRINT("TESTING ADD EFFECT COMPONENTS")

    EntityId id = 2;

    cm.add<TestEffectComp>(id);

    auto [comp] = cm.get<TestEffectComp>(id);
    assert(comp.size() == 1);

    cm.add<TestEffectComp>(id);

    assert(comp.size() == 2);
}

inline void test_remove_single_entities_for_multiple_components(CM &cm)
{
    PRINT("TESTING REMOVE SINGLE ID FROM MULTIPLE COMPONENT SETS")

    EntityId id1 = 1;
    cm.add<TestEventComp>(id1);
    cm.add<TestStackedComp>(id1);
    cm.add<TestNonStackedComp>(id1);

    cm.remove<TestEventComp, TestStackedComp, TestNonStackedComp>(id1);

    assert(!cm.contains<TestEventComp>(id1));
    assert(!cm.contains<TestStackedComp>(id1));
    assert(!cm.contains<TestNonStackedComp>(id1));
}

inline void test_remove_multiple_entities_for_multiple_components(CM &cm)
{
    PRINT("TESTING REMOVE MULTIPLE IDS FROM MULTIPLE COMPONENT SETS")

    EntityId id1 = 1;
    EntityId id2 = 2;
    EntityId id3 = 3;

    cm.add<TestEventComp>(id1);
    cm.add<TestEventComp>(id2);
    cm.add<TestEventComp>(id3);
    cm.add<TestStackedComp>(id1);
    cm.add<TestStackedComp>(id2);
    cm.add<TestStackedComp>(id3);
    cm.add<TestNonStackedComp>(id1);
    cm.add<TestNonStackedComp>(id2);
    cm.add<TestNonStackedComp>(id3);

    cm.remove<TestEventComp, TestStackedComp, TestNonStackedComp>(id1, id2, id3);

    assert(!cm.contains<TestEventComp>(id1));
    assert(!cm.contains<TestStackedComp>(id1));
    assert(!cm.contains<TestNonStackedComp>(id1));

    assert(!cm.contains<TestEventComp>(id2));
    assert(!cm.contains<TestStackedComp>(id2));
    assert(!cm.contains<TestNonStackedComp>(id2));

    assert(!cm.contains<TestEventComp>(id3));
    assert(!cm.contains<TestStackedComp>(id3));
    assert(!cm.contains<TestNonStackedComp>(id3));
}

inline void test_remove_multiple_entities_for_multiple_components_by_vector(CM &cm)
{
    PRINT("TESTING REMOVE MULTIPLE IDS FOR MULTIPLE COMPONENT SETS BY VECTOR")

    EntityId id1 = 1;
    EntityId id2 = 2;
    EntityId id3 = 3;

    cm.add<TestEventComp>(id1);
    cm.add<TestEventComp>(id2);
    cm.add<TestEventComp>(id3);
    cm.add<TestStackedComp>(id1);
    cm.add<TestStackedComp>(id2);
    cm.add<TestStackedComp>(id3);
    cm.add<TestNonStackedComp>(id1);
    cm.add<TestNonStackedComp>(id2);
    cm.add<TestNonStackedComp>(id3);

    cm.remove<TestEventComp, TestStackedComp, TestNonStackedComp>(std::vector{id1, id2, id3});

    assert(!cm.contains<TestEventComp>(id1));
    assert(!cm.contains<TestStackedComp>(id1));
    assert(!cm.contains<TestNonStackedComp>(id1));

    assert(!cm.contains<TestEventComp>(id2));
    assert(!cm.contains<TestStackedComp>(id2));
    assert(!cm.contains<TestNonStackedComp>(id2));

    assert(!cm.contains<TestEventComp>(id3));
    assert(!cm.contains<TestStackedComp>(id3));
    assert(!cm.contains<TestNonStackedComp>(id3));
}

inline void test_clear_all_components(CM &cm)
{
    PRINT("TESTING CLEAR ALL COMPONENTS")

    EntityId id1 = 1;
    EntityId id2 = 2;
    EntityId id3 = 3;
    cm.add<TestEventComp>(id1);
    cm.add<TestStackedComp>(id2);
    cm.add<TestNonStackedComp>(id3);

    cm.clear<TestEventComp, TestStackedComp, TestNonStackedComp>();

    assert(!cm.exists<TestEventComp>());
    assert(!cm.exists<TestStackedComp>());
    assert(!cm.exists<TestNonStackedComp>());
}

inline void test_clear_components_by_tag(CM &cm)
{
    PRINT("TESTING CLEAR COMPONENTS BY TAG")

    EntityId id1 = 1;
    EntityId id2 = 2;
    EntityId id3 = 3;
    cm.add<TestEventComp>(id1);
    cm.add<TestEventComp>(id2);
    cm.add<TestEventComp>(id3);

    auto [testEventCompSet] = cm.getAll<TestEventComp>();
    assert(testEventCompSet.size() == 3);

    cm.clear<ECS::Tags::Event>();

    assert(!cm.exists<TestEventComp>());
}

inline void test_clear_all_by_entity(CM &cm)
{
    PRINT("TESTING CLEAR ALL COMPONENTS BY ENTITY ID")

    EntityId id1 = 1;
    EntityId id2 = 2;
    EntityId id3 = 3;

    cm.add<TestEventComp>(id1);
    cm.add<TestEffectComp>(id1);
    cm.add<TestStackedComp>(id1);
    cm.add<TestNonStackedComp>(id1);

    cm.add<TestEventComp>(id2);
    cm.add<TestEffectComp>(id2);
    cm.add<TestStackedComp>(id2);
    cm.add<TestNonStackedComp>(id2);

    cm.add<TestEventComp>(id3);
    cm.add<TestEffectComp>(id3);
    cm.add<TestStackedComp>(id3);
    cm.add<TestNonStackedComp>(id3);

    auto [testEventCompSet, testEffectCompSet, testStackedCompSet, testNonStackCompSet] =
        cm.getAll<TestEventComp, TestEffectComp, TestStackedComp, TestNonStackedComp>();
    assert(testEventCompSet.size() == 3);
    assert(testEffectCompSet.size() == 3);
    assert(testStackedCompSet.size() == 3);
    assert(testNonStackCompSet.size() == 3);

    cm.remove(id2);

    assert(testEventCompSet.size() == 2);
    assert(testEffectCompSet.size() == 2);
    assert(testStackedCompSet.size() == 2);
    assert(testNonStackCompSet.size() == 2);

    assert(!cm.contains<TestNonStackedComp>(id2));
    assert(!cm.contains<TestEventComp>(id2));
    assert(!cm.contains<TestEffectComp>(id2));
    assert(!cm.contains<TestStackedComp>(id2));
}

inline void test_prune(CM &cm)
{
    PRINT("TESTING PRUNE")

    EntityId id{1};
    cm.add<TestEffectComp>(id, 1);
    cm.add<TestEffectComp>(id, 2);

    auto [compsSet] = cm.getAll<TestEffectComp>();
    assert(compsSet.size() == 1);

    auto [comps] = cm.get<TestEffectComp>(id);
    comps.remove([&](const TestEffectComp &testComp) { return true; });

    assert(comps.size() == 0);

    cm.prune<TestEffectComp>();

    assert(!cm.exists<TestEffectComp>());
}

inline void test_prune_multi(CM &cm)
{
    PRINT("TESTING PRUNE MULTIPLE")

    EntityId id{1};
    cm.add<TestEffectComp>(id, 1);
    cm.add<TestNonStackedComp>(id, 1);
    cm.add<TestStackedComp>(id, 1);

    auto [effectComps, nonstackComps, stackedComps] =
        cm.get<TestEffectComp, TestNonStackedComp, TestStackedComp>(id);
    effectComps.remove([&](const TestEffectComp &testComp) { return true; });
    nonstackComps.remove([&](const TestNonStackedComp &testComp) { return true; });
    stackedComps.remove([&](const TestStackedComp &testComp) { return true; });

    assert(effectComps.size() == 0);
    assert(nonstackComps.size() == 0);
    assert(stackedComps.size() == 0);

    cm.prune<TestEffectComp, TestStackedComp, TestNonStackedComp>();

    assert(!cm.exists<TestEffectComp>());
    assert(!cm.exists<TestNonStackedComp>());
    assert(!cm.exists<TestStackedComp>());
}

#ifdef ecs_allow_experimental
inline void test_prune_all(CM &cm)
{
    PRINT("TESTING PRUNE ALL")

    EntityId id{1};
    cm.add<TestEffectComp>(id, 1);
    cm.add<TestEffectComp>(id, 2);
    cm.add<TestNonStackedComp>(id, 3);
    cm.add<TestNonStackedComp>(id, 4);

    auto [compsSet] = cm.getAll<TestEffectComp>();
    assert(compsSet.size() == 1);

    auto [comps1, comps2] = cm.get<TestEffectComp, TestNonStackedComp>(id);
    comps1.remove([&](const TestEffectComp &testComp) { return true; });
    comps2.remove([&](const TestNonStackedComp &testComp) { return true; });

    assert(comps1.size() == 0);
    assert(comps2.size() == 0);

    cm.pruneAll();

    assert(!cm.exists<TestEffectComp>());
    assert(!cm.exists<TestNonStackedComp>());
}
#endif

inline void test_sparse_set_auto_prune(CM &cm)
{
    PRINT("TESTING AUTO PRUNE")

    EntityId id{1};
    cm.add<TestEffectComp>(id, 1);
    cm.add<TestEffectComp>(id, 2);

    auto [compsSet] = cm.getAll<TestEffectComp>();
    assert(compsSet.size() == 1);

    auto [comps] = cm.get<TestEffectComp>(id);
    comps.remove([&](const TestEffectComp &testComp) { return true; });

    assert(comps.size() == 0);

    int count{};
    compsSet.each([&](EId eId, auto &testComps) { count++; });

    assert(!cm.exists<TestEffectComp>());
}

inline void test_sparse_set_auto_prune_after_removal(CM &cm)
{
    PRINT("TESTING AUTO PRUNE AFTER REMOVE")

    EntityId id{1};
    cm.add<TestEffectComp>(id, 1);
    cm.add<TestEffectComp>(id, 2);

    auto [compsSet] = cm.getAll<TestEffectComp>();
    assert(compsSet.size() == 1);

    compsSet.each(
        [&](EId eId, auto &comps) { comps.remove([&](const TestEffectComp &testComp) { return true; }); });

    assert(!cm.exists<TestEffectComp>());
}

#ifdef ecs_allow_experimental
inline void test_prune_by_tag(CM &cm)
{
    PRINT("TESTING PRUNE BY TAG")

    EntityId id1{1};
    EntityId id2{2};
    EntityId id3{3};
    EntityId id4{4};
    EntityId id5{5};
    cm.add<TestEffectComp>(id1, 1);
    cm.add<TestEffectComp>(id1, 2);

    cm.add<TestEffectComp>(id2, 1);
    cm.add<TestEffectComp>(id2, 2);

    cm.add<TestEffectCompTimed>(id3);
    cm.add<TestEffectCompTimed>(id3, 3);
    /* cm.add<TestEffectCompTimed>(id3, 0.0f); */

    cm.add<TestEffectCompTimed>(id4);
    cm.add<TestEffectCompTimed>(id4, 4);

    cm.add<TestEffectCompTimed>(id5);
    cm.add<TestEffectCompTimed>(id5, 5);

    auto [untimedEffectSet, timedEffectSet] = cm.getAll<TestEffectComp, TestEffectCompTimed>();
    auto [testEffectComp1, testEffectComp2] = cm.get<TestEffectComp>(id1, id2);
    auto [testEffectCompTimed3, testEffectCompTimed4, testEffectCompTimed5] =
        cm.get<TestEffectComp>(id3, id4, id5);

    assert(untimedEffectSet.size() == 2);
    assert(timedEffectSet.size() == 3);
    assert(testEffectComp1.size() == 2);
    assert(testEffectComp2.size() == 2);
    assert(testEffectCompTimed3.size() == 2);
    assert(testEffectCompTimed4.size() == 2);

    testEffectComp1.remove([&](auto &testComp) { return true; });
    testEffectComp2.remove([&](auto &testComp) { return testComp.value == 2; });

    testEffectCompTimed3.remove([&](auto &testComp) { return testComp.value == 3; });
    testEffectCompTimed4.remove([&](auto &testComp) { return true; });
    testEffectCompTimed5.remove([&](auto &testComp) { return testComp.value == 5; });

    assert(testEffectComp1.size() == 0);
    assert(testEffectComp2.size() == 1);
    assert(testEffectCompTimed3.size() == 1);
    assert(testEffectCompTimed4.size() == 0);
    assert(testEffectCompTimed5.size() == 1);

    assert(untimedEffectSet.size() == 2);
    assert(timedEffectSet.size() == 3);

    cm.pruneByTag<ECS::Tags::Effect>();

    assert(untimedEffectSet.size() == 1);
    assert(timedEffectSet.size() == 2);
}
#endif
