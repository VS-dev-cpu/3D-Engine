#pragma once

#include "graphics.h"

#include "camera.h"
#include "object.h"
#include "light.h"
#include "physics.h"
#include "clock.h"

#include <algorithm>
#include <list>
#include <fstream>
#include <strstream>

class engine
{
public:
    graphics render_class;

    SDL_Texture *tex;
    SDL_Surface *surf;

    vector<int> rendering_cameras;

    vector<camera> cams;

    vector<object> objs;
    vector<light> lights;

    float fTheta = 0.0f;
    float fDeltaTime = 1.0f;

    void init(char *name, int w, int h, Uint32 flags, int cameras);
    void update(float fElapsedTime);
    int add(char *path);
    int add(lightType type, color3f c, float strength, vec3d position, vec3d direction);

    int create();
    int tri(int obj, triangle tri);
};

float fov = 0;

void engine::init(char *name, int w, int h, Uint32 flags, int cameras)
{
    render_class.init(name, w, h, flags);

    surf = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);

    camera cam;
    for (int i = 0; i < cameras; i++)
    {
        cams.push_back(cam);
        cams[i].infill.r = 150;
        cams[i].infill.g = 200;
        cams[i].infill.b = 250;
        cams[i].init(90, w, h, 0.1, 1000, 1);
    }
}

void engine::update(float fElapsedTime)
{
    fDeltaTime = fElapsedTime;

    // Clear The Previous Camera Image
    for (int i = 0; i < cams.size(); i++)
    {
        SDL_LockSurface(cams[i].image);
        cams[i].clear();
    }

    mat4x4 matRotZ, matRotX;

    // Translate World Into it's position
    matRotX = Matrix_MakeRotationX(fTheta);
    matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);

    mat4x4 matTrans;
    matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 0.0f);

    mat4x4 matWorld;
    matWorld = Matrix_MakeIdentity();
    matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX);
    matWorld = Matrix_MultiplyMatrix(matWorld, matTrans);

    // Update Cameras
    for (int i = 0; i < cams.size(); i++)
        cams[i].update();

    for (int obj_i = 0; obj_i < objs.size(); obj_i++)
    {
        object obj = objs[obj_i];

        mesh m = obj.translated;

        for (int tri_i = 0; tri_i < m.tris.size(); tri_i++)
        {
            triangle tri = m.tris[tri_i];

            for (int i = 0; i < lights.size(); i++)
            {
                lights[i].calculate_color(&tri);
            }

            triangle triProjected, triTransformed, triViewed;

            triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
            triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
            triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);
            triTransformed.color = tri.color;
            triTransformed.idensity = tri.idensity;
            triTransformed.isTextured = tri.isTextured;
            triTransformed.texture = tri.texture;
            triTransformed.normal = tri.normal;
            triTransformed.t[0] = tri.t[0];
            triTransformed.t[1] = tri.t[1];
            triTransformed.t[2] = tri.t[2];

            // Render The Triangle
            for (int i = 0; i < cams.size(); i++)
                cams[i].render(triTransformed);
        }
    }
    for (int i = 0; i < cams.size(); i++)
        SDL_UnlockSurface(cams[i].image);
}

int engine::add(char *path)
{
    ifstream f(path);
    if (!f.is_open())
        return -1;

    // Local cache of verts
    vector<vec3d> verts;
    vector<vec2d> texs;

    object obj;

    while (!f.eof())
    {
        char line[128];
        f.getline(line, 128);

        strstream s;
        s << line;

        char junk;

        if (line[0] == 'v')
        {
            if (line[1] == ' ')
            {
                vec3d v;
                s >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }
            else if (line[1] == 't')
            {
                vec2d v;
                s >> junk >> junk >> v.u >> v.v;
                texs.push_back(v);
            }
            else if (line[1] == 'n')
            {
                // DOSOMETHING
            }
        }
        if (line[0] == 'f')
        {
            string str;
            str = s.str();
            vector<string> parts;
            parts.push_back(split(str, " ", 1));
            parts.push_back(split(str, " ", 2));
            parts.push_back(split(str, " ", 3));

            triangle t;
            t.p[0] = verts[isplit(parts[0], "/", 0) - 1];
            t.p[1] = verts[isplit(parts[1], "/", 0) - 1];
            t.p[2] = verts[isplit(parts[2], "/", 0) - 1];
            obj.meshData.tris.push_back(t);
        }
    }

    // Calculate Normals
    obj.calculate_normals();
    obj.update();
    objs.push_back(obj);

    return objs.size() - 1;
}

int engine::add(lightType type, color3f c, float strength, vec3d position, vec3d direction)
{
    light l;
    l.type = type;
    l.color = c;
    l.strength = strength;
    l.position = position;
    l.direction = direction;
    lights.push_back(l);

    return lights.size() - 1;
}

int engine::create()
{
    object obj;
    objs.push_back(obj);

    return objs.size() - 1;
}

int engine::tri(int obj, triangle t)
{
    objs[obj].meshData.tris.push_back(t);

    return objs[obj].meshData.tris.size() - 1;
}
