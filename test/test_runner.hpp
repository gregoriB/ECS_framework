#pragma once

#include "../example/invaders/core.hpp"
#include "tests/benchmarks.hpp"
/* #include "tests/components.hpp" */
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
    /* test_component_mutate_fn, */
    /* test_component_remove_fn, */
    /* test_component_remove_conditionally, */

#ifdef ecs_allow_experimental
    test_effect_cleanup,
    test_effect_cleanup_timed,
    test_effect_cleanup_only_effect_components,
#endif
    
    /* test_get_component, */
    /* test_gather_component, */
    /* test_gather_group, */
    /*  */
    /* test_add_non_stack_component, */
    /* test_add_more_non_stack_components_fail, */
    /* test_add_stacked_components, */
    /* test_add_event_components, */
    /* test_add_effect_components, */
    /*  */
    /* test_clear_all_components, */
    /* test_clear_components_by_tag, */
    /* test_clear_all_by_entity, */
    /*  */
    /* test_prune, */
    /* test_prune_multi, */
    /*  */
#ifdef ecs_allow_experimental
    test_prune_all,
    test_prune_by_tag,
#endif

#ifndef ecs_disable_auto_prune
    /* test_sparse_set_auto_prune, */
    /* test_sparse_set_auto_prune_after_removal, */
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
    test_benchmark_2M_gather_group,
    test_benchmark_2M_access,
    test_benchmark_2M_update,
    test_benchmark_2M_destroy,
    test_benchmark_2M_clear,
    test_benchmark_2M_remove,
#ifndef ecs_disable_auto_prune
    test_benchmark_2M_remove_and_auto_prune,
#endif
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

        test(componentManager);

        count++;
    }

    PRINT("#################### RESULTS #####################")
    PRINT("              ", count, " TESTS PASSED"            )
    PRINT("##################################################")

    return true;
};

inline void run() { 
#ifndef ecs_bench_test
    PRINT()
    PRINT("=================== RUNNING COMPONENT MANAGER TESTS ====================")
    PRINT()
    runTests(Tests::COMPONENT_MANAGER);
#ifdef ecs_disable_auto_prune
    PRINT("!! AUTO PRUNE TESTS SKIPPED -- CHECK AUTO PRUNE FLAG: 'ecs_disable_auto_prune' !!")
#endif
    return;

    PRINT("====================== RUNNING UTILITY TESTS ==========================")
    PRINT()
    runTests(Tests::UTILITIES); 

#else
    PRINT("====================== RUNNING BENCHMARK TESTS ==========================")
    PRINT()
    runTests(Tests::BENCHMARKS); 
#endif
};
// clang-format on
} // namespace TestSystem
