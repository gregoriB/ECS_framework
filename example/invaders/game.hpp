#pragma once

#include "core.hpp"
#include "renderer.hpp"
#include "update.hpp"
#include "utilities.hpp"

class Game
{
  public:
    void run()
    {
        if (!init())
            return;

        m_renderManager.startRender();
        withBenchmarks([&]() -> int { return loop(); });
    }

  private:
    inline void withBenchmarks(std::function<float()> fn)
    {
        auto start = std::chrono::high_resolution_clock::now();

        int cycles = fn();
        if (cycles == 0)
            cycles = 1;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = end - start;
        auto avg = duration.count() / cycles;
        auto framerate = cycles / (avg * cycles);

        PRINT("average frame time:", avg, "for", cycles, "frames\n", "  average FPS:", framerate);
    };

    bool init()
    {
        try
        {
            if (!m_renderManager.init())
                throw 12345;

            Utilties::setup(m_entityComponentManager, m_screenConfig);
        }
        catch (int code)
        {
            PRINT("Error from init()", code)
            return false;
        }

        return true;
    }

    int loop()
    {
        int cycles{0};
        int limit{20000000};

        std::cout << "\n $$$$$ STARTING GAME $$$$$ \n";
        bool quit{false};
        float prevTime = m_renderManager.tick();

        while (!quit)
        {
            if (cycles++ > limit)
                break;

            /* PRINT("\n ~~~ CYCLE:", cycles, "~~~\n") */

            float startTime = m_renderManager.tick();
            auto inputs = m_renderManager.pollInputs();

            Utilties::registerPlayerInputs(m_entityComponentManager, inputs);

            if (!Update::run(m_entityComponentManager))
            {
                PRINT("!! QUIT COMMAND ISSUED !!")
                quit = true;
                continue;
            };

            m_renderManager.clear();
            auto renders = Utilties::getRenderableElements(m_entityComponentManager);
            m_renderManager.render(renders);

            int endTime = m_renderManager.tick();
            int timeDiff = endTime - startTime;
            if (timeDiff < SCREEN_TICKS_PER_FRAME)
                m_renderManager.wait(SCREEN_TICKS_PER_FRAME - timeDiff);

            float delta = (startTime - prevTime) / 1000.0f;
            setDeltaTime(delta);
            prevTime = startTime;
        }

        std::cout << "\n $$$$$ GAME OVER $$$$$ \n\n";

        m_renderManager.exit();

        return cycles;
    }

    void setDeltaTime(float delta)
    {
        Utilties::updateDeltaTime(m_entityComponentManager, delta);
    }

  private:
    EntityComponentManager<EntityId> m_entityComponentManager{};
    ScreenConfig m_screenConfig{};
    Renderer::Manager<EntityId> m_renderManager{m_screenConfig};
};
