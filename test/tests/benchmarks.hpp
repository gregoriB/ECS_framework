#pragma once

#include "../helpers/components.hpp"
#include "../helpers/utils.hpp"

inline constexpr int COUNT_2K = 2000;
inline constexpr int COUNT_65K = 65000;
inline constexpr int COUNT_100K = 100000;
inline constexpr int COUNT_200K = 200000;
inline constexpr int COUNT_1M = 1000000;
inline constexpr int COUNT_2M = 2000000;

using Timer = ECS::internal::Utilities::Timer;

inline void setupBenchmark(CM &cm, int entityCount)
{
    createEntityWithComponents<TestVelocityComponent, TestPositionComponent>(cm, entityCount);
}

inline void test_benchmark_2M_create(CM &cm)
{
    PRINT("BENCHMARKING CREATING 2M ENTITIES W/ 2 COMPONENTS...")

    Timer timer{1};
    createEntityWithComponents<TestVelocityComponent, TestPositionComponent>(cm, COUNT_2M);

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_destroy(CM &cm)
{
    PRINT("BENCHMARKING DESTROYING 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        cm.remove<TestVelocityComponent>(i);
        cm.remove<TestPositionComponent>(i);
    }

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_get_single_entity_single_type(CM &cm)
{
    PRINT("BENCHMARKING GET 2M ENTITIES W/ 2 COMPONENTS SINGLE ENTITY SINGLE TYPE...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        auto [testVelSet, testPosSet] = cm.get<TestVelocityComponent, TestPositionComponent>(i);
        testVelSet.inspect([&](auto &_) { count1++; });
        testPosSet.inspect([&](auto &_) { count2++; });
    }

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_get_multiple_entities_single_type(CM &cm)
{
    PRINT("BENCHMARKING GET 2M ENTITIES W/ 2 COMPONENTS MULTIPLE ENTITY SINGLE TYPE...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M + 1; ++i)
    {
        auto [vel1, vel2] = cm.get<TestVelocityComponent>(i, i + 1);
        auto [pos1, pos2] = cm.get<TestPositionComponent>(i + 1, 1);
        auto &targetVel = i % 2 == 0 ? vel1 : vel2;
        auto &targetPos = i % 2 == 0 ? pos1 : pos2;
        targetVel.inspect([&](auto &_) { count1++; });
        targetPos.inspect([&](auto &_) { count2++; });
    }

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_gather(CM &cm)
{
    PRINT("BENCHMARKING GATHER 2M ENTITIES W/ 2 COMPONENTS...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        auto [velComp, posComp] = cm.get<TestVelocityComponent, TestPositionComponent>(i);
        velComp.inspect([&](auto &_) { count1++; });
        posComp.inspect([&](auto &_) { count2++; });
    }

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_get_all(CM &cm)
{
    PRINT("BENCHMARKING GET ALL 2M ENTITIES W/ 2 COMPONENTS...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    auto [velComps] = cm.getAll<TestVelocityComponent>();
    auto [posComps] = cm.getAll<TestPositionComponent>();
    velComps.each([&](EId eId, auto &comps) { comps.inspect([&](auto &_) { count1++; }); });
    posComps.each([&](EId eId, auto &comps) { comps.inspect([&](auto &_) { count2++; }); });

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_gather_all(CM &cm)
{
    PRINT("BENCHMARKING GATHER ALL 2M ENTITIES W/ 2 COMPONENTS...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    auto [velComps, posComps] = cm.getAll<TestVelocityComponent, TestPositionComponent>();
    velComps.each([&](EId eId, auto &comps) { comps.inspect([&](auto &_) { count1++; }); });
    posComps.each([&](EId eId, auto &comps) { comps.inspect([&](auto &_) { count2++; }); });

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_gather_group(CM &cm)
{
    PRINT("BENCHMARKING GATHER GROUP 2M ENTITIES W/ 2 COMPONENTS...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    auto group = cm.getGroup<TestVelocityComponent, TestPositionComponent>();
    group.each([&](EId eId, auto &velComps, auto &posComps) {
        velComps.inspect([&](auto &_) { count1++; });
        posComps.inspect([&](auto &_) { count2++; });
    });

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_access(CM &cm)
{
    PRINT("BENCHMARKING ACCESSED 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    int j{};
    for (int i = 1; i <= COUNT_2M; ++i)
    {
        auto [testVelSet, testPosSet] = cm.get<TestVelocityComponent, TestPositionComponent>(i);
        testVelSet.inspect([&](const TestVelocityComponent &testComp) { j = testComp.x; });
        testPosSet.inspect([&](const TestPositionComponent &testComp) { j = testComp.x; });
    }

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_update(CM &cm)
{
    PRINT("BENCHMARKING UPDATING 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        auto [testVelSet, testPosSet] = cm.get<TestVelocityComponent, TestPositionComponent>(i);
        testVelSet.mutate([&](TestVelocityComponent &testComp) {
            testComp.x = 10.0f;
            testComp.y = 10.0f;
        });
        testPosSet.mutate([&](TestPositionComponent &testComp) {
            testComp.x = 10.0f;
            testComp.y = 10.0f;
        });
    }

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_clear(CM &cm)
{
    PRINT("BENCHMARKING CLEAR 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        cm.clear<TestVelocityComponent>();
        cm.clear<TestPositionComponent>();
    }

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_remove(CM &cm)
{
    PRINT("BENCHMARKING REMOVING 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {

        auto [testVelSet, testPosSet] = cm.get<TestVelocityComponent, TestPositionComponent>(i);
        testVelSet.remove([&](const TestVelocityComponent &testComp) { return true; });
        testPosSet.remove([&](const TestPositionComponent &testComp) { return true; });
    }

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_remove_and_auto_prune(CM &cm)
{
    PRINT("BENCHMARKING REMOVING AND AUTO PRUNE 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(cm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        auto [testVelSet, testPosSet] = cm.get<TestVelocityComponent, TestPositionComponent>(i);
        testVelSet.remove([&](const TestVelocityComponent &testComp) { return true; });
        testPosSet.remove([&](const TestPositionComponent &testComp) { return true; });
    }

    auto [velComps] = cm.getAll<TestVelocityComponent>();
    auto [posComps] = cm.getAll<TestPositionComponent>();
    velComps.each([&](EId eId, auto &_) {});
    posComps.each([&](EId eId, auto &_) {});

    auto elapsed = timer.getElapsedTime();

    assert(velComps.size() == 0);
    assert(posComps.size() == 0);

    PRINT("TIME:", elapsed, "seconds");
}
