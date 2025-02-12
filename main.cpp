#include "example/invaders/game.hpp"

#ifdef ecs_test
#include "test/test_runner.hpp"
#endif

Benchmark runWithBenchmarks()
{
    int frameLimit{500000};
    int totalSets{3};

    std::vector<Benchmark> benchmarks{};
    for (int i = 0; i < totalSets; ++i)
    {
        Game game{};
        benchmarks.push_back(game.run(frameLimit));
    }

    Benchmark benches{};

    for (const auto& bench : benchmarks)
    {
        benches.cycles += bench.cycles;
        benches.average += bench.average;
    }

    benches.average /= totalSets;

    return benches;
}

void run()
{
    Game game{};
    game.run();
}

int main() {
#ifdef ecs_test
    TestSystem::run();

    return 0;
#endif

#ifdef ecs_with_benchmarks
    Benchmark bench = runWithBenchmarks();
    bench.printBenchmarks();

    return 0;
#endif

    run();
    
    return 0;
}
