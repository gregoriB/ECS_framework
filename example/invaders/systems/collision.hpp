#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::Collision
{
inline void cleanup(ECM &ecm)
{
}

inline void handleCollisions(ECM &ecm)
{
    ecm.getAll<CollisionCheckEvent>().each([&](EId eId1, auto &checkEvents) {
        auto [cX, cY, cW, cH] = checkEvents.peek(&CollisionCheckEvent::bounds).box();

        ecm.getAll<PositionComponent>().each([&](EId eId2, auto &positionComps) {
            if (eId1 == eId2)
                return;

            auto [pX, pY, pW, pH] = positionComps.peek(&PositionComponent::bounds).box();
            bool isX = (cX >= pX && cX <= pW) || (cW >= pX && cW <= pW);
            bool isY = (cY >= pY && cY <= pH) || (cH >= pY && cH <= pH);
            if (!isX || !isY)
                return;

            ecm.add<DeathEvent>(eId1, eId2);
            ecm.add<DeathEvent>(eId2, eId1);
        });
    });
}

inline auto update(ECM &ecm)
{
    handleCollisions(ecm);

    return cleanup;
};
}; // namespace Systems::Collision
