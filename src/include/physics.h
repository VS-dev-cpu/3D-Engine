#pragma once

#include "functions.h"

using namespace std;

struct ray
{
    vec3d pos, dir;
    float len;

    vector<int> body;
    vector<vec3d> hit;
};

class physics
{
public:
    vector<mesh> colliders;

    bool collide(vec3d a, triangle b);
    bool collide(triangle a, triangle b);
    bool collide(triangle a, mesh b);
    bool collide(mesh a, mesh b);

    ray raycast(vec3d a, vec3d dir, float len);
    ray raycast(vec3d a, vec3d b);
};

// Point - Triangle Collision
bool physics::collide(vec3d a, triangle b)
{
    bool lower_x = false;
    bool higher_x = false;

    bool lower_y = false;
    bool higher_y = false;

    bool lower_z = false;
    bool higher_z = false;

    for (int i = 0; i < 3; i++)
    {
        if (b.p[i].x <= a.x)
            lower_x = true;
        if (b.p[i].x >= a.x)
            higher_x = true;

        if (b.p[i].y <= a.y)
            lower_y = true;
        if (b.p[i].y >= a.y)
            higher_y = true;

        if (b.p[i].z <= a.z)
            lower_z = true;
        if (b.p[i].z >= a.z)
            higher_z = true;
    }

    if (lower_x and higher_x and lower_y and higher_y and lower_z and higher_z)
        return true;
    return false;
}

// Triangle - Triangle Collision
bool physics::collide(triangle a, triangle b)
{
    bool lower_x = false;
    bool higher_x = false;

    bool lower_y = false;
    bool higher_y = false;

    bool lower_z = false;
    bool higher_z = false;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (a.p[i].x <= b.p[j].x)
                lower_x = true;
            if (a.p[i].x >= b.p[j].x)
                higher_x = true;

            if (a.p[i].y <= b.p[j].y)
                lower_y = true;
            if (a.p[i].y >= b.p[j].y)
                higher_y = true;

            if (a.p[i].z <= b.p[j].z)
                lower_z = true;
            if (a.p[i].z >= b.p[j].z)
                higher_z = true;
        }
    }

    if (lower_x and higher_x and lower_y and higher_y and lower_z and higher_z)
        return true;
    return false;
}

// Triangle - Mesh Collision
bool physics::collide(triangle a, mesh b)
{
    for (int i = 0; i < b.tris.size(); i++)
    {
        if (collide(a, b.tris[i]))
            return true;
    }
}

// Mesh - Mesh Collision
bool physics::collide(mesh a, mesh b)
{
    for (int i = 0; i < a.tris.size(); i++)
    {
        if (collide(a.tris[i], b))
            return true;
    }
}

//  Shoot a Ray from point 'a' to direction "dir" for a distance of "len"
ray physics::raycast(vec3d a, vec3d dir, float len)
{
    ray r;
    r.pos = a;
    r.len = len;

    bool l = true;
    while (l)
    {
        l = (dir.x >= 1 or dir.x <= -1) or (dir.y >= 1 or dir.y <= -1) or (dir.z >= 1 or dir.z <= -1);

        if (dir.x >= 1 or dir.x <= -1)
            dir.x /= 10;

        if (dir.y >= 1 or dir.y <= -1)
            dir.y /= 10;

        if (dir.z >= 1 or dir.z <= -1)
            dir.z /= 10;
    }

    r.dir = dir;

    vec3d p = a;
    triangle t;
    while (distance(a, p) <= len)
    {
        p.x += r.dir.x;
        p.y += r.dir.y;
        p.z += r.dir.z;
        t.p[0] = p;

        p.x += r.dir.x;
        p.y += r.dir.y;
        p.z += r.dir.z;
        t.p[1] = p;

        p.x += r.dir.x;
        p.y += r.dir.y;
        p.z += r.dir.z;
        t.p[2] = p;

        for (int i = 0; i < colliders.size(); i++)
        {
            if (collide(t, colliders[i]))
            {
                r.body.push_back(i);
                r.hit.push_back(p);
            }
        }
    }

    return r;
}

// Shoot a Ray from point 'a' to point 'b'
ray physics::raycast(vec3d a, vec3d b)
{
    vec3d dir;
    dir.x = abs(a.x - b.x);
    dir.y = abs(a.y - b.y);
    dir.z = abs(a.z - b.z);
    return raycast(a, dir, distance(a, b));
}