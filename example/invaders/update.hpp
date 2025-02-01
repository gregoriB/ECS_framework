#pragma once

#include "components.hpp"
#include "core.hpp"
#include "systems/ai.hpp"
#include "systems/attack.hpp"
#include "systems/collision.hpp"
#include "systems/death.hpp"
#include "systems/game.hpp"
#include "systems/input.hpp"
#include "systems/movement.hpp"
#include "systems/player.hpp"
#include "systems/position.hpp"

#include <functional>

namespace Update
{
using CleanupFunc = std::function<void(ECM &)>;

template <typename CleanupFuncs> inline void cleanup(ECM &ecm, CleanupFuncs &cleanupFuncs)
{
    for (auto &func : cleanupFuncs)
        func(ecm);

    ecm.clearByTag<Tags::Event>();
}

inline bool run(ECM &ecm)
{
    // clang-format off
    std::array<CleanupFunc, 9> cleanupFuncs{
        Systems::AI::update(ecm),
        Systems::Input::update(ecm),
        Systems::Attack::update(ecm),
        Systems::Movement::update(ecm),
        Systems::Position::update(ecm),
        Systems::Collision::update(ecm),
        Systems::Death::update(ecm),
        Systems::Player::update(ecm),
        Systems::Game::update(ecm),
    };

    // clang-format on
    cleanup(ecm, cleanupFuncs);

    auto [gameId, gameComps] = ecm.getUniqueEntity<GameComponent>();

    return !gameComps.peek(&GameComponent::isGameOver);
}
}; // namespace Update
