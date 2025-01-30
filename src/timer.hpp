#pragma once

#include <chrono>

class Timer
{
  public:
    Timer(float _seconds) : duration(_seconds)
    {
        restart();
    }

    bool hasElapsed() const
    {
        return getElapsedTime() >= duration;
    }

    float getElapsedTime() const
    {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration<float>(now - start).count();
    }

    void update(float _seconds)
    {
        duration = _seconds;
        restart();
    }

    void restart()
    {
        start = std::chrono::steady_clock::now();
    }

    float getDuration() const
    {
        return duration;
    }

  private:
    float duration{0};
    std::chrono::steady_clock::time_point start;
};
