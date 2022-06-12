#pragma once

#include <SDL2/SDL.h>
#include "types.h"

class graphics
{
public:
    SDL_Renderer *renderer;
    SDL_Window *window;
    int w, h;

    void init(char *name, int w, int h, Uint32 flags);
    void update();

    SDL_Texture *create_texture(SDL_Surface *surface, int w, int h);

    void image(SDL_Surface *surface);
    void draw_texture(SDL_Texture *tex, int x, int y, int w, int h);
    void draw_surface(SDL_Surface *surface, int x, int y, int w, int h);
    void draw_line(int x1, int y1, int x2, int y2, int r, int g, int b);
    void draw_rect(int x, int y, int w, int h, int r, int g, int b);
    void draw_all(int r, int g, int b);
    void draw_filled_triangle(triangle tri);

    void texture_pixel(SDL_Texture *tex, int x, int y, int r, int g, int b);
    void texture_line(SDL_Texture *tex, int x1, int y1, int x2, int y2, int r, int g, int b);
    void texture_rect(SDL_Texture *tex, int x, int y, int w, int h, int r, int g, int b);
    void texture_all(SDL_Texture *tex, int r, int g, int b);
    void texture_filled_triangle(SDL_Texture *tex, triangle tri);

    void surface_pixel(SDL_Surface *surface, int x, int y, int r, int g, int);
    void surface_line(SDL_Surface *surface, int x1, int y1, int x2, int y2, int r, int g, int b);
    void surface_rect(SDL_Surface *surface, int x, int y, int w, int h, int r, int g, int b);
    void surface_all(SDL_Surface *surface, int r, int g, int b);
    void surface_filled_triangle(SDL_Surface *surface, triangle tri);
};

void graphics::init(char *name, int w_w, int w_h, Uint32 flags)
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w_w, w_h, flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    w = w_w;
    h = w_h;
}

void graphics::update()
{
    SDL_RenderPresent(renderer);
}

SDL_Texture *graphics::create_texture(SDL_Surface *surface, int w, int h)
{
    if (surface == NULL)
        return SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
}

void graphics::image(SDL_Surface *surface)
{
    draw_surface(surface, 0, 0, w, h);
}

void graphics::draw_texture(SDL_Texture *tex, int x, int y, int w, int h)
{
    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = 0;
    src.h = 0;
    SDL_QueryTexture(tex, NULL, NULL, &src.w, &src.h);

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;

    SDL_RenderCopy(renderer, tex, &src, &dst);
}

void graphics::draw_surface(SDL_Surface *surface, int x, int y, int w, int h)
{
    SDL_Texture *tmp = SDL_CreateTextureFromSurface(renderer, surface);
    draw_texture(tmp, x, y, w, h);
    SDL_DestroyTexture(tmp);
}

void graphics::draw_line(int x1, int y1, int x2, int y2, int r, int g, int b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void graphics::draw_rect(int x, int y, int w, int h, int r, int g, int b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderFillRect(renderer, &rect);
}

void graphics::draw_all(int r, int g, int b)
{
    draw_rect(0, 0, w, h, r, g, b);
}

void graphics::draw_filled_triangle(triangle tri)
{
    int x1 = tri.p[0].x;
    int y1 = tri.p[0].y;
    int x2 = tri.p[1].x;
    int y2 = tri.p[1].y;
    int x3 = tri.p[2].x;
    int y3 = tri.p[2].y;

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1 > y2)
    {
        swap(y1, y2);
        swap(x1, x2);
    }
    if (y1 > y3)
    {
        swap(y1, y3);
        swap(x1, x3);
    }
    if (y2 > y3)
    {
        swap(y2, y3);
        swap(x2, x3);
    }

    t1x = t2x = x1;
    y = y1; // Starting points
    dx1 = (int)(x2 - x1);
    if (dx1 < 0)
    {
        dx1 = -dx1;
        signx1 = -1;
    }
    else
        signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1);
    if (dx2 < 0)
    {
        dx2 = -dx2;
        signx2 = -1;
    }
    else
        signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1)
    { // swap values
        swap(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2)
    { // swap values
        swap(dy2, dx2);
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2)
        goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;)
    {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else
        {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1)
        {
            i++;
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                    t1xp = signx1; // t1x += signx1;
                else
                    goto next1;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (1)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2; // t2x += signx2;
                else
                    goto next2;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
    next2:
        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;

        draw_line(minx, y, maxx, y, tri.color.r * tri.idensity.r, tri.color.g * tri.idensity.g, tri.color.b * tri.idensity.b);

        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2)
            break;
    }
next:
    // Second half
    dx1 = (int)(x3 - x2);
    if (dx1 < 0)
    {
        dx1 = -dx1;
        signx1 = -1;
    }
    else
        signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1)
    { // swap values
        swap(dy1, dx1);
        changed1 = true;
    }
    else
        changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++)
    {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else
        {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1)
        {
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                {
                    t1xp = signx1;
                    break;
                } // t1x += signx1;
                else
                    goto next3;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
            if (i < dx1)
                i++;
        }
    next3:
        // process second line until y value is about to change
        while (t2x != x3)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2;
                else
                    goto next4;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
    next4:

        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;

        draw_line(minx, y, maxx, y, tri.color.r * tri.idensity.r, tri.color.g * tri.idensity.g, tri.color.b * tri.idensity.b);

        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y > y3)
            return;
    }
}

void graphics::texture_pixel(SDL_Texture *tex, int x, int y, int r, int g, int b)
{
    Uint32 *pixels = nullptr;
    int pitch = 0;
    Uint32 format;

    // Get the size of the texture.
    int w, h;
    SDL_QueryTexture(tex, &format, NULL, &w, &h);

    if (x < 0 or x > w)
        return;
    if (y < 0 or y > h)
        return;

    // Now let's make our "pixels" pointer point to the texture data.
    if (SDL_LockTexture(tex, nullptr, (void **)&pixels, &pitch))
    {
        // If the locking fails, you might want to handle it somehow. SDL_GetError(); or something here.
        return;
    }
    SDL_PixelFormat *pixelFormat = new SDL_PixelFormat;
    pixelFormat->format = SDL_PIXELFORMAT_RGBA8888;
    Uint32 color = SDL_MapRGB(pixelFormat, r, g, b);

    // Before setting the color, we need to know where we have to place it.
    Uint32 pixelPosition = y * pitch + x;

    // Now we can set the pixel(s) we want.
    pixels[pixelPosition] = color;

    // Also don't forget to unlock your texture once you're done.
    SDL_UnlockTexture(tex);
}

void graphics::texture_line(SDL_Texture *tex, int x1, int y1, int x2, int y2, int r, int g, int b)
{
    // fast horizontal line
    if (y1 == y2)
    {
        int32_t start = min(x1, x2);
        int32_t end = max(x1, x2);

        for (int i = start; i < end; i++)
        {
            texture_pixel(tex, i, y1, r, g, b);
        }
        return;
    }

    // fast vertical line
    if (x1 == x2)
    {
        int32_t start = min(y1, y2);
        int32_t end = max(y1, y2);
        for (int i = start; i < end; i++)
        {
            texture_pixel(tex, x1, i, r, g, b);
        }
        return;
    }

    // general purpose line
    // lines are either "shallow" or "steep" based on whether the x delta
    // is greater than the y delta
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    bool shallow = abs(dx) > abs(dy);
    if (shallow)
    {
        // shallow version
        int32_t s = abs(dx);          // number of steps
        int32_t sx = dx < 0 ? -1 : 1; // x step value
        int32_t sy = (dy << 16) / s;  // y step value in fixed 16:16
        int32_t x = x1;
        int32_t y = y1 << 16;
        while (s--)
        {
            texture_pixel(tex, x, (y >> 16), r, g, b);
            y += sy;
            x += sx;
        }
    }
    else
    {
        // steep version
        int32_t s = abs(dy);          // number of steps
        int32_t sy = dy < 0 ? -1 : 1; // y step value
        int32_t sx = (dx << 16) / s;  // x step value in fixed 16:16
        int32_t y = y1;
        int32_t x = x1 << 16;
        while (s--)
        {
            texture_pixel(tex, (x >> 16), y, r, g, b);
            y += sy;
            x += sx;
        }
    }
}

void graphics::texture_rect(SDL_Texture *tex, int x, int y, int w, int h, int r, int g, int b)
{
    for (int ix = x; ix < w; ix++)
    {
        for (int iy = y; iy < h; iy++)
        {
            texture_pixel(tex, ix, iy, r, g, b);
        }
    }
}

void graphics::texture_all(SDL_Texture *tex, int r, int g, int b)
{
    // Get the size of the texture.
    int w, h;
    SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            printf("%i %i\n", x, y);
            texture_pixel(tex, x, y, r, g, b);
        }
    }
}

void graphics::texture_filled_triangle(SDL_Texture *tex, triangle tri)
{
    int x1 = tri.p[0].x;
    int y1 = tri.p[0].y;
    int x2 = tri.p[1].x;
    int y2 = tri.p[1].y;
    int x3 = tri.p[2].x;
    int y3 = tri.p[2].y;

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1 > y2)
    {
        swap(y1, y2);
        swap(x1, x2);
    }
    if (y1 > y3)
    {
        swap(y1, y3);
        swap(x1, x3);
    }
    if (y2 > y3)
    {
        swap(y2, y3);
        swap(x2, x3);
    }

    t1x = t2x = x1;
    y = y1; // Starting points
    dx1 = (int)(x2 - x1);
    if (dx1 < 0)
    {
        dx1 = -dx1;
        signx1 = -1;
    }
    else
        signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1);
    if (dx2 < 0)
    {
        dx2 = -dx2;
        signx2 = -1;
    }
    else
        signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1)
    { // swap values
        swap(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2)
    { // swap values
        swap(dy2, dx2);
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2)
        goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;)
    {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else
        {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1)
        {
            i++;
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                    t1xp = signx1; // t1x += signx1;
                else
                    goto next1;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (1)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2; // t2x += signx2;
                else
                    goto next2;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
    next2:
        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;

        texture_line(tex, minx, y, maxx, y, tri.color.r * tri.idensity.r, tri.color.g * tri.idensity.g, tri.color.b * tri.idensity.b);

        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2)
            break;
    }
next:
    // Second half
    dx1 = (int)(x3 - x2);
    if (dx1 < 0)
    {
        dx1 = -dx1;
        signx1 = -1;
    }
    else
        signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1)
    { // swap values
        swap(dy1, dx1);
        changed1 = true;
    }
    else
        changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++)
    {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else
        {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1)
        {
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                {
                    t1xp = signx1;
                    break;
                } // t1x += signx1;
                else
                    goto next3;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
            if (i < dx1)
                i++;
        }
    next3:
        // process second line until y value is about to change
        while (t2x != x3)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2;
                else
                    goto next4;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
    next4:

        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;

        texture_line(tex, minx, y, maxx, y, tri.color.r * tri.idensity.r, tri.color.g * tri.idensity.g, tri.color.b * tri.idensity.b);

        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y > y3)
            return;
    }
}

void graphics::surface_pixel(SDL_Surface *surface, int x, int y, int r, int g, int b)
{
    Uint32 pixel = SDL_MapRGB(surface->format, r, g, b);

    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[(y * surface->w) + x] = pixel;
}

void graphics::surface_line(SDL_Surface *surface, int x1, int y1, int x2, int y2, int r, int g, int b)
{
    // fast horizontal line
    if (y1 == y2)
    {
        int32_t start = min(x1, x2);
        int32_t end = max(x1, x2);

        for (int i = start; i < end; i++)
        {
            surface_pixel(surface, i, y1, r, g, b);
        }
        return;
    }

    // fast vertical line
    if (x1 == x2)
    {
        int32_t start = min(y1, y2);
        int32_t end = max(y1, y2);
        for (int i = start; i < end; i++)
        {
            surface_pixel(surface, x1, i, r, g, b);
        }
        return;
    }

    // general purpose line
    // lines are either "shallow" or "steep" based on whether the x delta
    // is greater than the y delta
    int32_t dx = x2 - x1;
    int32_t dy = y2 - y1;
    bool shallow = abs(dx) > abs(dy);
    if (shallow)
    {
        // shallow version
        int32_t s = abs(dx);          // number of steps
        int32_t sx = dx < 0 ? -1 : 1; // x step value
        int32_t sy = (dy << 16) / s;  // y step value in fixed 16:16
        int32_t x = x1;
        int32_t y = y1 << 16;
        while (s--)
        {
            surface_pixel(surface, x, (y >> 16), r, g, b);
            y += sy;
            x += sx;
        }
    }
    else
    {
        // steep version
        int32_t s = abs(dy);          // number of steps
        int32_t sy = dy < 0 ? -1 : 1; // y step value
        int32_t sx = (dx << 16) / s;  // x step value in fixed 16:16
        int32_t y = y1;
        int32_t x = x1 << 16;
        while (s--)
        {
            surface_pixel(surface, (x >> 16), y, r, g, b);
            y += sy;
            x += sx;
        }
    }
}

void graphics::surface_rect(SDL_Surface *surface, int x, int y, int w, int h, int r, int g, int b)
{
    for (int ix = x; ix < w; ix++)
    {
        for (int iy = y; iy < h; iy++)
        {
            surface_pixel(surface, ix, iy, r, g, b);
        }
    }
}

void graphics::surface_all(SDL_Surface *surface, int r, int g, int b)
{
    int w = surface->w;
    int h = surface->h;

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            surface_pixel(surface, x, y, r, g, b);
        }
    }
}

void graphics::surface_filled_triangle(SDL_Surface *surface, triangle tri)
{
    int x1 = tri.p[0].x;
    int y1 = tri.p[0].y;
    int x2 = tri.p[1].x;
    int y2 = tri.p[1].y;
    int x3 = tri.p[2].x;
    int y3 = tri.p[2].y;

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1 > y2)
    {
        swap(y1, y2);
        swap(x1, x2);
    }
    if (y1 > y3)
    {
        swap(y1, y3);
        swap(x1, x3);
    }
    if (y2 > y3)
    {
        swap(y2, y3);
        swap(x2, x3);
    }

    t1x = t2x = x1;
    y = y1; // Starting points
    dx1 = (int)(x2 - x1);
    if (dx1 < 0)
    {
        dx1 = -dx1;
        signx1 = -1;
    }
    else
        signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1);
    if (dx2 < 0)
    {
        dx2 = -dx2;
        signx2 = -1;
    }
    else
        signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1)
    { // swap values
        swap(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2)
    { // swap values
        swap(dy2, dx2);
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2)
        goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;)
    {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else
        {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1)
        {
            i++;
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                    t1xp = signx1; // t1x += signx1;
                else
                    goto next1;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (1)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2; // t2x += signx2;
                else
                    goto next2;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
    next2:
        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;

        surface_line(surface, minx, y, maxx, y, tri.color.r * tri.idensity.r, tri.color.g * tri.idensity.g, tri.color.b * tri.idensity.b);

        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2)
            break;
    }
next:
    // Second half
    dx1 = (int)(x3 - x2);
    if (dx1 < 0)
    {
        dx1 = -dx1;
        signx1 = -1;
    }
    else
        signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1)
    { // swap values
        swap(dy1, dx1);
        changed1 = true;
    }
    else
        changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++)
    {
        t1xp = 0;
        t2xp = 0;
        if (t1x < t2x)
        {
            minx = t1x;
            maxx = t2x;
        }
        else
        {
            minx = t2x;
            maxx = t1x;
        }
        // process first line until y value is about to change
        while (i < dx1)
        {
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if (changed1)
                {
                    t1xp = signx1;
                    break;
                } // t1x += signx1;
                else
                    goto next3;
            }
            if (changed1)
                break;
            else
                t1x += signx1;
            if (i < dx1)
                i++;
        }
    next3:
        // process second line until y value is about to change
        while (t2x != x3)
        {
            e2 += dy2;
            while (e2 >= dx2)
            {
                e2 -= dx2;
                if (changed2)
                    t2xp = signx2;
                else
                    goto next4;
            }
            if (changed2)
                break;
            else
                t2x += signx2;
        }
    next4:

        if (minx > t1x)
            minx = t1x;
        if (minx > t2x)
            minx = t2x;
        if (maxx < t1x)
            maxx = t1x;
        if (maxx < t2x)
            maxx = t2x;

        surface_line(surface, minx, y, maxx, y, tri.color.r * tri.idensity.r, tri.color.g * tri.idensity.g, tri.color.b * tri.idensity.b);

        if (!changed1)
            t1x += signx1;
        t1x += t1xp;
        if (!changed2)
            t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y > y3)
            return;
    }
}