#include "example/invaders/game.hpp"
#ifdef ecs_test
#include "test/test_runner.hpp"
#endif

#ifdef ecs_test
int main() {
    TestSystem::run();
}
#endif

#ifndef ecs_test
int main() {

    Game game{};

    game.run();

    return 0;
}
#endif
