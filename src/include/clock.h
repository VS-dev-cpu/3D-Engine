#pragma once

#include <SDL2/SDL.h>

struct game_clock
{
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    float deltaTime = 0;

    float start = 0;
    float current = 0;

    void update()
    {
        this->LAST = this->NOW;
        this->NOW = SDL_GetPerformanceCounter();
        this->deltaTime = ((float)((this->NOW - this->LAST) * 1000 / (float)SDL_GetPerformanceFrequency())) / 1000;

        this->current = SDL_GetTicks() - this->start;
    }

    void reset()
    {
        this->start = SDL_GetTicks();
    }
};