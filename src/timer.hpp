#pragma once

#include <chrono>

namespace ECS
{
namespace internal
{
namespace Utilities
{
/**
 * @deprecated This will be removed in a future version once custom tags are implemented
 * 
 * @brief Has no use within the library.  This is a purely game logic component
 */
class Timer
{
  public:
    Timer(float _seconds) : duration(_seconds)
    {
        restart();
    }

    bool isStopped() const
    {
        return stopped;
    }

    bool hasElapsed() const
    {
        return getElapsedTime() >= duration;
    }

    float getElapsedTime() const
    {
        if (isStopped())
            return std::chrono::duration<float>(stopTime - startTime).count();

        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration<float>(now - startTime).count();
    }

    void update(float _seconds)
    {
        duration = _seconds;
        restart();
    }

    void restart()
    {
        stopped = false;
        startTime = std::chrono::steady_clock::now();
    }

    float getDuration() const
    {
        return duration;
    }

    void stop()
    {
        stopped = true;
        stopTime = std::chrono::steady_clock::now();
    }

  private:
    float duration{0};
    bool stopped{};
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point stopTime;
};
}; // namespace Utilities
}; // namespace internal
}; // namespace ECS
