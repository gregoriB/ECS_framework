#pragma once

#include "../core.hpp"
#include "tests/benchmarks.hpp"
#include "tests/components.hpp"
#include "tests/utilities.hpp"

// clang-format off
namespace TestSystem
{
enum class Tests {
    NONE = 0,
    COMPONENT_MANAGER,
    UTILITIES,
    BENCHMARKS,
};

using testFn = std::function<void(ECM &)>;
inline std::vector<testFn> componentManagerTests{
    test_component_mutate_fn,
    test_component_remove_fn,
    test_component_remove_conditionally,

    /* test_effect_cleanup, */
    /* test_effect_cleanup_timed, */
    /* test_effect_cleanup_only_effect_components, */
    
    test_get_component,
    test_gather_component,

    test_add_unique_component,
    test_add_more_unique_components_fail,
    test_add_stacked_components,
    test_add_event_components,
    test_add_effect_components,
    
    test_clear_all_components,
    test_clear_components_by_tag,
    test_clear_all_by_entity,
    
    test_prune,
    test_prune_multi,

    /* test_prune_all, */
    /* test_prune_by_tag, */

#ifndef game_disable_auto_prune
    test_sparse_set_auto_prune,
    test_sparse_set_auto_prune_after_removal,
#endif
};

inline std::vector<testFn> utiltiesTests{
    test_enum_string_converter,
    test_get_enum_string,
    test_get_enum_size_count,
    test_get_enum_array,
};

inline std::vector<testFn> benchmarkTests{
    test_benchmark_2M_create,
    test_benchmark_2M_get_single_entity_single_type,
    test_benchmark_2M_get_multiple_entities_single_type,
    test_benchmark_2M_get_all,
    test_benchmark_2M_gather,
    test_benchmark_2M_gather_all,
    test_benchmark_2M_access,
    test_benchmark_2M_update,
    test_benchmark_2M_destroy,
    test_benchmark_2M_clear,
    test_benchmark_2M_remove,
    test_benchmark_2M_remove_and_auto_prune,
};

inline bool runTests(Tests testType) {

    std::vector<testFn> tests;
    switch(testType)
    {
        case Tests::COMPONENT_MANAGER:
            tests = componentManagerTests;
            break;
        case Tests::UTILITIES:
            tests = utiltiesTests;
            break;
        case Tests::BENCHMARKS:
            tests = benchmarkTests;
            break;
        default:
            break;
    }

    int count{0};
    while (count < tests.size())
    {
        auto test = tests[count];
        EntityComponentManager<EntityId> componentManager;
        ResourceManager resource_manager;

        test(componentManager);

        count++;
    }

    PRINT("#################### RESULTS #####################")
    PRINT("              ", count, " TESTS PASSED"            )
    PRINT("##################################################")

    return true;
};

inline auto update = [](ECM & ecm, RM & rm) -> bool { 
#ifndef game_bench_test
    PRINT()
    PRINT("=================== RUNNING COMPONENT MANAGER TESTS ====================")
    PRINT()
    runTests(Tests::COMPONENT_MANAGER);
#ifdef game_disable_auto_prune
    PRINT("!! AUTO PRUNE TESTS SKIPPED -- CHECK AUTO PRUNE FLAG: 'game_disable_auto_prune' !!")
#endif
    return true;

    PRINT("====================== RUNNING UTILITY TESTS ==========================")
    PRINT()
    runTests(Tests::UTILITIES); 

#else
    PRINT("====================== RUNNING BENCHMARK TESTS ==========================")
    PRINT()
    runTests(Tests::BENCHMARKS); 
#endif

    return true;
};

// Mock system
inline auto init = [](ECM &ecm, RM &rm) {};
inline auto getState = [](ECM &ecm) {};
inline auto setDelta = [](ECM &ecm, float delta) {};
inline auto handleInputs = [](ECM &ecm, std::vector<Inputs> &inputs) {};
inline auto checkHealth = []() {};
inline auto getRenders = [](ECM &ecm, RM &rm) {
    return std::vector<Renderer::RenderableElement>{};
};

// clang-format on
} // namespace TestSystem
