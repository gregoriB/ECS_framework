#pragma once

#include <chrono>

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
        stopped = false;
        start = std::chrono::steady_clock::now();
    }

    float getDuration() const
    {
        return duration;
    }

    void stop()
    {
        stopped = true;
    }

  private:
    float duration{0};
    bool stopped{};
    std::chrono::steady_clock::time_point start;
};
