#pragma once

#include "functions.h"

enum lightType
{
    point,
    direction,
    spot
};

struct light
{
    vec3d position;
    vec3d direction;

    vec3d spread;

    lightType type = point;

    color3f color;

    float strength = 0.5f;

    void calculate_color(triangle *t)
    {
        vec3d triA = Vector_Avarage(t->p[0], t->p[1], t->p[2]);
        t->idensity.r = 0;
        t->idensity.g = 0;
        t->idensity.b = 0;
        switch (this->type)
        {
        case 0: // point
            t->idensity.r += map(distance(triA, this->position), 0.0f, this->strength, 1.0f, 0.0f);
            t->idensity.g += map(distance(triA, this->position), 0.0f, this->strength, 1.0f, 0.0f);
            t->idensity.b += map(distance(triA, this->position), 0.0f, this->strength, 1.0f, 0.0f);

            t->idensity.r *= this->color.r;
            t->idensity.g *= this->color.g;
            t->idensity.b *= this->color.b;
            break;

        case 1: // direction
            t->idensity.r += max(0.1f, Vector_DotProduct(this->direction, t->normal)) * this->strength;
            t->idensity.g += max(0.1f, Vector_DotProduct(this->direction, t->normal)) * this->strength;
            t->idensity.b += max(0.1f, Vector_DotProduct(this->direction, t->normal)) * this->strength;

            t->idensity.r *= this->color.r;
            t->idensity.g *= this->color.g;
            t->idensity.b *= this->color.b;
            break;

        case 2: // spot
            for (int i = 0; i < 3; i++)
            {
                if (this->direction.x > 0 && t->p[i].x < this->position.x - this->spread.x)
                    break;
                if (this->direction.x < 0 && t->p[i].x > this->position.x + this->spread.x)
                    break;

                if (this->direction.y > 0 && t->p[i].y < this->position.y - this->spread.y)
                    break;
                if (this->direction.y < 0 && t->p[i].y > this->position.y + this->spread.y)
                    break;

                if (this->direction.z > 0 && t->p[i].z < this->position.z - this->spread.z)
                    break;
                if (this->direction.z < 0 && t->p[i].z > this->position.z + this->spread.z)
                    break;
            }

            t->idensity.r += map(distance(triA, this->position), 0.0f, this->strength, 1.0f, 0.0f);
            t->idensity.g += map(distance(triA, this->position), 0.0f, this->strength, 1.0f, 0.0f);
            t->idensity.b += map(distance(triA, this->position), 0.0f, this->strength, 1.0f, 0.0f);

            t->idensity.r *= this->color.r;
            t->idensity.g *= this->color.g;
            t->idensity.b *= this->color.b;
            break;
        }
    }
};