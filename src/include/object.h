#pragma once

#include "functions.h"

struct object
{
    vec3d position;
    vec3d velocity;
    rot3d rotation;

    mesh meshData;
    mesh translated;

    void calculate_normals()
    {
        // Calculate Normals
        for (int i = 0; i < this->meshData.tris.size(); i++)
        {
            this->meshData.tris[i].normal = Triangle_CalculateNormal(this->meshData.tris[i]);
        }
    }

    void set_color(int r, int g, int b)
    {
        for (int i = 0; i < this->meshData.tris.size(); i++)
        {
            this->meshData.tris[i].color.r = r;
            this->meshData.tris[i].color.g = g;
            this->meshData.tris[i].color.b = b;
        }
    }

    void set_color(color3i color)
    {
        set_color(color.r, color.g, color.b);
    }

    void update()
    {
        this->translated = this->meshData;

        mat4x4 matRotX = Matrix_MakeRotationX(this->rotation.pitch * M_RAD);
        mat4x4 matRotY = Matrix_MakeRotationY(this->rotation.yaw * M_RAD);
        mat4x4 matRotZ = Matrix_MakeRotationZ(this->rotation.roll * M_RAD);
        mat4x4 rotation = Matrix_MakeIdentity();
        rotation = Matrix_MultiplyMatrix(rotation, matRotX);
        rotation = Matrix_MultiplyMatrix(rotation, matRotY);
        rotation = Matrix_MultiplyMatrix(rotation, matRotZ);

        for (int t = 0; t < this->translated.tris.size(); t++)
        {
            for (int i = 0; i < 3; i++)
            {
                this->translated.tris[t].p[i] = Matrix_MultiplyVector(matRotX, this->translated.tris[t].p[i]);
                this->translated.tris[t].p[i] = Matrix_MultiplyVector(matRotY, this->translated.tris[t].p[i]);
                this->translated.tris[t].p[i] = Matrix_MultiplyVector(matRotZ, this->translated.tris[t].p[i]);

                this->translated.tris[t].p[i].x += this->position.x;
                this->translated.tris[t].p[i].y += this->position.y;
                this->translated.tris[t].p[i].z += this->position.z;
            }
        }
    }
};