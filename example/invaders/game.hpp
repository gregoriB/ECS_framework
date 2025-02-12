#pragma once

#include "core.hpp"
#include "renderer.hpp"
#include "update.hpp"
#include "utilities.hpp"
#include <stdexcept>

class Benchmark
{
  public:
    float average;
    int cycles;

    void printBenchmarks()
    {
        PRINT("average frame time:", average, "for", cycles, "frames\n", "  average FPS:", getFramerate());
    }

    void printBenchData()
    {
        PRINT("AVERAGE:", average, "CYCLES:", cycles, "FRAMERATE:", getFramerate())
    }

    void run(std::function<float()> fn)
    {
        auto start = std::chrono::high_resolution_clock::now();

        cycles = fn();
        if (cycles == 0)
            cycles = 1;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = end - start;
        average = duration.count() / cycles;
    };

    float getFramerate()
    {
        return cycles / (average * cycles);
    }
};

class Game
{
  public:
    Benchmark run(int cycles = 0)
    {
        if (!init())
            throw std::runtime_error("INIT FAILED");

        m_renderManager.startRender();
        Benchmark benchmark;
        benchmark.run([&]() -> int { return loop(cycles); });

        return benchmark;
    }

  private:
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

    int loop(int limit = 0)
    {
        int cycles{0};

        std::cout << "\n $$$$$ STARTING GAME $$$$$ \n";
        bool quit{false};
        float prevTime = m_renderManager.tick();

        while (!quit)
        {
            cycles++;
            if (cycles++ > limit && limit)
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
