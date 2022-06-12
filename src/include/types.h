#pragma once

#include <vector>
#include <list>
#include <SDL2/SDL.h>

using namespace std;

struct vec2d
{
    float u = 0.0f;
    float v = 0.0f;
    float w = 1.0f;
};

struct vec3d
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;
};

struct rot3d
{
    float pitch = 0;
    float yaw = 0;
    float roll = 0;
};

struct mat4x4
{
    float m[4][4] = {0};
};

struct color3i
{
    int r = 255;
    int g = 255;
    int b = 255;
};

struct color3f
{
    float r = 1;
    float g = 1;
    float b = 1;
};

struct triangle
{
    vec3d p[3];
    vec2d t[3];

    vec3d normal;

    bool isTextured = false;

    color3i color;
    color3f idensity;

    SDL_Surface texture;
};

struct mesh
{
    vector<triangle> tris;
};