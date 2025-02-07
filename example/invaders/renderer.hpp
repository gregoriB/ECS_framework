#pragma once

#include "core.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <SDL_timer.h>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string_view>

namespace Renderer
{
struct RGBA
{
    uint8_t r, g, b, a;
    RGBA() : r(0), g(0), b(0), a(255)
    {
    }
    RGBA(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a)
    {
    }
};

struct RenderableElement
{
    float x, y, w, h;
    RGBA rgba;
    std::string_view text;

    RenderableElement(float _x, float _y, float _w, float _h, RGBA _rgba, std::string_view _text = "")
        : x(_x), y(_y), w(_w), h(_h), rgba(_rgba), text(_text)
    {
    }
};

template <typename EntityId> class Manager
{
  public:
    Manager(ScreenConfig _config) : m_screen(_config)
    {
    }

    ~Manager()
    {
        exit();
    }

    TTF_Font *m_font;
    bool init()
    {
        TTF_Init();
        m_font = TTF_OpenFont("/home/brandon/coding/cpp/ecs_library/example/invaders/assets/font.ttf", 24);
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        {
            printError("Could not initialize renderer");
            return false;
        }

        return true;
    }

    bool createRenderer()
    {
        if (!m_window)
            return false;

        m_renderer = SDL_CreateRenderer(m_window, false, SDL_RENDERER_ACCELERATED);

        return m_renderer ? true : false;
    }

    bool startRender()
    {
        if (!createWindow() || !createRenderer())
        {
            exit();
            return false;
        }
        SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        return true;
    }

    void exit()
    {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);

        IMG_Quit();
        SDL_Quit();

        m_window = nullptr;
        m_renderer = nullptr;
    }

    void printError(const std::string &message = "An error has occurred")
    {
        std::cout << message << " SDL_Error: " << SDL_GetError() << std::endl;
    }

    bool createWindow()
    {
        m_window = SDL_CreateWindow("SPACE INVADERS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    m_screen.width, m_screen.height, SDL_WINDOW_SHOWN);

        if (!m_window)
        {
            printError("Window could not be created");
            return false;
        }

        return true;
    }

    void clear()
    {
        SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(m_renderer);
    }

    void renderText(SDL_Renderer *renderer, const RenderableElement &re, const SDL_Rect &rect)
    {
        const auto &text = re.text;
        auto [r, g, b, a] = re.rgba;
        SDL_Color color = {r, g, b, a};

        if (text.empty() || !m_font)
            return;

        SDL_Surface *surface = TTF_RenderText_Solid(m_font, text.data(), color);
        if (!surface)
            return;

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            SDL_FreeSurface(surface);
            return;
        }

        SDL_Rect textRect = {rect.x, rect.y + (rect.h - surface->h) / 2, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    void renderTile(const RenderableElement &re)
    {
        auto tile = createRectangle(re.x, re.y, re.w, re.h);
        if (re.text.empty())
            setRenderColor(re.rgba);
        else
            setInvisibleRenderColor();

        renderText(m_renderer, re, tile);
        renderSolidRect(tile);
    }

    void renderSolidRect(SDL_Rect &tile)
    {
        SDL_RenderFillRect(m_renderer, &tile);
    }

    void setRenderColor(const RGBA &rgba)
    {
        auto [r, g, b, a] = rgba;
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    }

    void setInvisibleRenderColor()
    {
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
    }

    SDL_Rect createRectangle(int x, int y, int w, int h)
    {
        return SDL_Rect{x, y, w, h};
    }

    void render(std::vector<RenderableElement> &renderElements)
    {
        for (const auto &element : renderElements)
            renderTile(element);

        SDL_RenderPresent(m_renderer);
    }

    int tick()
    {
        return SDL_GetTicks64();
    }

    void wait(int time)
    {
        SDL_Delay(time);
    }

    std::vector<Inputs> pollInputs()
    {
        std::vector<Inputs> inputs;
        while (SDL_PollEvent(&m_event))
        {
            if (m_event.type == SDL_QUIT)
            {
                inputs.push_back(Inputs::QUIT);
                continue;
            }
        }

        const Uint8 *keyStates = SDL_GetKeyboardState(NULL);

        if (keyStates[SDL_SCANCODE_LEFT])
            inputs.push_back(Inputs::LEFT);
        if (keyStates[SDL_SCANCODE_RIGHT])
            inputs.push_back(Inputs::RIGHT);
        if (keyStates[SDL_SCANCODE_SPACE])
            inputs.push_back(Inputs::SHOOT);

        return inputs;
    }

  private:
    SDL_Window *m_window;
    SDL_Event m_event;
    ScreenConfig m_screen;
    SDL_Renderer *m_renderer;
};
} // namespace Renderer
