#pragma once

#include "../helpers/components.hpp"
#include "../helpers/utils.hpp"

inline constexpr int COUNT_2K = 2000;
inline constexpr int COUNT_65K = 65000;
inline constexpr int COUNT_100K = 100000;
inline constexpr int COUNT_200K = 200000;
inline constexpr int COUNT_1M = 1000000;
inline constexpr int COUNT_2M = 2000000;

using Timer = ECS::Timer;

inline void setupBenchmark(ECM &ecm, int entityCount)
{
    createEntityWithComponents<TestVelocityComponent, TestPositionComponent>(ecm, entityCount);
}

inline void test_benchmark_2M_create(ECM &ecm)
{
    PRINT("BENCHMARKING CREATING 2M ENTITIES W/ 2 COMPONENTS...")

    Timer timer{1};
    createEntityWithComponents<TestVelocityComponent, TestPositionComponent>(ecm, COUNT_2M);

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_destroy(ECM &ecm)
{
    PRINT("BENCHMARKING DESTROYING 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        ecm.remove<TestVelocityComponent>(i);
        ecm.remove<TestPositionComponent>(i);
    }

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_get_single_entity_single_type(ECM &ecm)
{
    PRINT("BENCHMARKING GET 2M ENTITIES W/ 2 COMPONENTS SINGLE ENTITY SINGLE TYPE...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        auto [testVelSet, testPosSet] = ecm.get<TestVelocityComponent, TestPositionComponent>(i);
        testVelSet.inspect([&](auto &_) { count1++; });
        testPosSet.inspect([&](auto &_) { count2++; });
    }

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_get_multiple_entities_single_type(ECM &ecm)
{
    PRINT("BENCHMARKING GET 2M ENTITIES W/ 2 COMPONENTS MULTIPLE ENTITY SINGLE TYPE...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M + 1; ++i)
    {
        auto [vel1, vel2] = ecm.get<TestVelocityComponent>(i, i + 1);
        auto [pos1, pos2] = ecm.get<TestPositionComponent>(i + 1, 1);
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

inline void test_benchmark_2M_gather(ECM &ecm)
{
    PRINT("BENCHMARKING GATHER 2M ENTITIES W/ 2 COMPONENTS...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        auto [velComp, posComp] = ecm.get<TestVelocityComponent, TestPositionComponent>(i);
        velComp.inspect([&](auto &_) { count1++; });
        posComp.inspect([&](auto &_) { count2++; });
    }

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_get_all(ECM &ecm)
{
    PRINT("BENCHMARKING GET ALL 2M ENTITIES W/ 2 COMPONENTS...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    auto [velComps] = ecm.getAll<TestVelocityComponent>();
    auto [posComps] = ecm.getAll<TestPositionComponent>();
    velComps.each([&](EId eId, auto &comps) { comps.inspect([&](auto &_) { count1++; }); });
    posComps.each([&](EId eId, auto &comps) { comps.inspect([&](auto &_) { count2++; }); });

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_gather_all(ECM &ecm)
{
    PRINT("BENCHMARKING GATHER ALL 2M ENTITIES W/ 2 COMPONENTS...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    auto [velComps, posComps] = ecm.getAll<TestVelocityComponent, TestPositionComponent>();
    velComps.each([&](EId eId, auto &comps) { comps.inspect([&](auto &_) { count1++; }); });
    posComps.each([&](EId eId, auto &comps) { comps.inspect([&](auto &_) { count2++; }); });

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_gather_group(ECM &ecm)
{
    PRINT("BENCHMARKING GATHER GROUP 2M ENTITIES W/ 2 COMPONENTS...")

    uint32_t count1{};
    uint32_t count2{};

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    auto group = ecm.getGroup<TestVelocityComponent, TestPositionComponent>();
    group.each([&](EId eId, auto &velComps, auto &posComps) {
        velComps.inspect([&](auto &_) { count1++; });
        posComps.inspect([&](auto &_) { count2++; });
    });

    auto elapsed = timer.getElapsedTime();

    assert(count1 == COUNT_2M);
    assert(count2 == COUNT_2M);

    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_access(ECM &ecm)
{
    PRINT("BENCHMARKING ACCESSED 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    int j{};
    for (int i = 1; i <= COUNT_2M; ++i)
    {
        auto [testVelSet, testPosSet] = ecm.get<TestVelocityComponent, TestPositionComponent>(i);
        testVelSet.inspect([&](const TestVelocityComponent &testComp) { j = testComp.x; });
        testPosSet.inspect([&](const TestPositionComponent &testComp) { j = testComp.x; });
    }

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_update(ECM &ecm)
{
    PRINT("BENCHMARKING UPDATING 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        auto [testVelSet, testPosSet] = ecm.get<TestVelocityComponent, TestPositionComponent>(i);
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

inline void test_benchmark_2M_clear(ECM &ecm)
{
    PRINT("BENCHMARKING CLEAR 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        ecm.clear<TestVelocityComponent>();
        ecm.clear<TestPositionComponent>();
    }

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_remove(ECM &ecm)
{
    PRINT("BENCHMARKING REMOVING 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {

        auto [testVelSet, testPosSet] = ecm.get<TestVelocityComponent, TestPositionComponent>(i);
        testVelSet.remove([&](const TestVelocityComponent &testComp) { return true; });
        testPosSet.remove([&](const TestPositionComponent &testComp) { return true; });
    }

    auto elapsed = timer.getElapsedTime();
    PRINT("TIME:", elapsed, "seconds");
}

inline void test_benchmark_2M_remove_and_auto_prune(ECM &ecm)
{
    PRINT("BENCHMARKING REMOVING AND AUTO PRUNE 2M ENTITIES W/ 2 COMPONENTS...")

    setupBenchmark(ecm, COUNT_2M);
    Timer timer{1};

    for (int i = 1; i <= COUNT_2M; ++i)
    {
        auto [testVelSet, testPosSet] = ecm.get<TestVelocityComponent, TestPositionComponent>(i);
        testVelSet.remove([&](const TestVelocityComponent &testComp) { return true; });
        testPosSet.remove([&](const TestPositionComponent &testComp) { return true; });
    }

    auto [velComps] = ecm.getAll<TestVelocityComponent>();
    auto [posComps] = ecm.getAll<TestPositionComponent>();
    velComps.each([&](EId eId, auto &_) {});
    posComps.each([&](EId eId, auto &_) {});

    auto elapsed = timer.getElapsedTime();

    assert(velComps.size() == 0);
    assert(posComps.size() == 0);

    PRINT("TIME:", elapsed, "seconds");
}
