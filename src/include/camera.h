#pragma once

#include "functions.h"

struct camera
{
    SDL_Surface *image;
    color3i infill;

    vec3d position;
    rot3d rotation;

    vec3d vLookDir;
    vec3d vUp;
    mat4x4 matView;

    mat4x4 matProj;
    float fNearCutoff = 1.0f;

    int w, h;
    float *pDepthBuffer = nullptr;

    bool out_of_screen(int x, int y)
    {
        if (x < 0 or x > this->w - 1)
            return true;
        if (y < 0 or y > this->h - 1)
            return true;

        return false;
    }

    void set_pixel(SDL_Surface *surface, int x, int y, int r, int g, int b)
    {
        if (out_of_screen(x, y))
            return;

        Uint32 pixel = SDL_MapRGB(surface->format, r, g, b);

        Uint32 *pixels = (Uint32 *)surface->pixels;
        pixels[(y * surface->w) + x] = pixel;
    }

    color3i get_pixel(SDL_Surface *surface, int x, int y)
    {
        color3i color;
        if (out_of_screen(x, y))
            return color;

        int bpp = surface->format->BytesPerPixel;
        /* Here p is the address to the pixel we want to retrieve */
        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
        Uint32 pd;

        switch (bpp)
        {
        case 1:
            pd = *p;
            break;

        case 2:
            pd = *(Uint16 *)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                pd = p[0] << 16 | p[1] << 8 | p[2];
            else
                pd = p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            pd = *(Uint32 *)p;
            break;

        default:
            pd = 0; /* shouldn't happen, but avoids warnings */
        }
        Uint8 r, g, b;

        SDL_GetRGB(pd, surface->format, &r, &g, &b);
        color.r = r;
        color.g = g;
        color.b = b;
        return color;
    }

    void set_buffer(int x, int y, float data)
    {
        if (out_of_screen(x, y))
            return;

        pDepthBuffer[y * this->w + x] = data;
    }

    float get_buffer(int x, int y)
    {
        if (out_of_screen(x, y))
            return 0;

        return pDepthBuffer[y * this->w + x];
    }

    void draw_triangle(triangle tri)
    {
        if (tri.p[1].y < tri.p[0].y)
        {
            swap(tri.p[0].y, tri.p[1].y);
            swap(tri.p[0].x, tri.p[1].x);
            swap(tri.t[0].u, tri.t[1].u);
            swap(tri.t[0].v, tri.t[1].v);
            swap(tri.t[0].w, tri.t[1].w);
        }

        if (tri.p[2].y < tri.p[0].y)
        {
            swap(tri.p[0].y, tri.p[2].y);
            swap(tri.p[0].x, tri.p[2].x);
            swap(tri.t[0].u, tri.t[2].u);
            swap(tri.t[0].v, tri.t[2].v);
            swap(tri.t[0].w, tri.t[2].w);
        }

        if (tri.p[2].y < tri.p[1].y)
        {
            swap(tri.p[1].y, tri.p[2].y);
            swap(tri.p[1].x, tri.p[2].x);
            swap(tri.t[1].u, tri.t[2].u);
            swap(tri.t[1].v, tri.t[2].v);
            swap(tri.t[1].w, tri.t[2].w);
        }

        int dy1 = tri.p[1].y - tri.p[0].y;
        int dx1 = tri.p[1].x - tri.p[0].x;
        float dv1 = tri.t[1].v - tri.t[0].v;
        float du1 = tri.t[1].u - tri.t[0].u;
        float dw1 = tri.t[1].w - tri.t[0].w;

        int dy2 = tri.p[2].y - tri.p[0].y;
        int dx2 = tri.p[2].x - tri.p[0].x;
        float dv2 = tri.t[2].v - tri.t[0].v;
        float du2 = tri.t[2].u - tri.t[0].u;
        float dw2 = tri.t[2].w - tri.t[0].w;

        float tex_u, tex_v, tex_w;

        float dax_step = 0, dbx_step = 0,
              du1_step = 0, dv1_step = 0,
              du2_step = 0, dv2_step = 0,
              dw1_step = 0, dw2_step = 0;

        if (dy1)
            dax_step = dx1 / (float)abs(dy1);
        if (dy2)
            dbx_step = dx2 / (float)abs(dy2);

        if (dy1)
            du1_step = du1 / (float)abs(dy1);
        if (dy1)
            dv1_step = dv1 / (float)abs(dy1);
        if (dy1)
            dw1_step = dw1 / (float)abs(dy1);

        if (dy2)
            du2_step = du2 / (float)abs(dy2);
        if (dy2)
            dv2_step = dv2 / (float)abs(dy2);
        if (dy2)
            dw2_step = dw2 / (float)abs(dy2);

        if (dy1)
        {
            for (int i = tri.p[0].y; i <= tri.p[1].y; i++)
            {
                int ax = tri.p[0].x + (float)(i - tri.p[0].y) * dax_step;
                int bx = tri.p[0].x + (float)(i - tri.p[0].y) * dbx_step;

                float tex_su = tri.t[0].u + (float)(i - tri.p[0].y) * du1_step;
                float tex_sv = tri.t[0].v + (float)(i - tri.p[0].y) * dv1_step;
                float tex_sw = tri.t[0].w + (float)(i - tri.p[0].y) * dw1_step;

                float tex_eu = tri.t[0].u + (float)(i - tri.p[0].y) * du2_step;
                float tex_ev = tri.t[0].v + (float)(i - tri.p[0].y) * dv2_step;
                float tex_ew = tri.t[0].w + (float)(i - tri.p[0].y) * dw2_step;

                if (ax > bx)
                {
                    swap(ax, bx);
                    swap(tex_su, tex_eu);
                    swap(tex_sv, tex_ev);
                    swap(tex_sw, tex_ew);
                }

                tex_u = tex_su;
                tex_v = tex_sv;

                tex_w = tex_sw;

                float tstep = 1.0f / ((float)(bx - ax));
                float t = 0.0f;

                for (int j = ax; j < bx; j++)
                {
                    tex_u = (1.0f - t) * tex_su + t * tex_eu;
                    tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                    tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                    if (tex_w > get_buffer(j, i))
                    {
                        if (tri.isTextured)
                        {
                            color3i color = get_pixel(&tri.texture, tex_u, tex_v);
                            set_pixel(this->image, j, i, tri.idensity.r * color.r, tri.idensity.g * color.g, tri.idensity.b * color.b);
                        }
                        else
                        {
                            set_pixel(this->image, j, i, tri.color.r * tri.idensity.r, tri.color.g * tri.idensity.g, tri.color.b * tri.idensity.b);
                        }
                        set_buffer(j, i, tex_w);
                    }
                    t += tstep;
                }
            }
        }

        dy1 = tri.p[2].y - tri.p[1].y;
        dx1 = tri.p[2].x - tri.p[1].x;
        dv1 = tri.t[2].v - tri.t[1].v;
        du1 = tri.t[2].u - tri.t[1].u;
        dw1 = tri.t[2].w - tri.t[1].w;

        if (dy1)
            dax_step = dx1 / (float)abs(dy1);
        if (dy2)
            dbx_step = dx2 / (float)abs(dy2);

        du1_step = 0, dv1_step = 0;
        if (dy1)
            du1_step = du1 / (float)abs(dy1);
        if (dy1)
            dv1_step = dv1 / (float)abs(dy1);
        if (dy1)
            dw1_step = dw1 / (float)abs(dy1);

        if (dy1)
        {
            for (int i = tri.p[1].y; i <= tri.p[2].y; i++)
            {
                int ax = tri.p[1].x + (float)(i - tri.p[1].y) * dax_step;
                int bx = tri.p[0].x + (float)(i - tri.p[0].y) * dbx_step;

                float tex_su = tri.t[1].u + (float)(i - tri.p[1].y) * du1_step;
                float tex_sv = tri.t[1].v + (float)(i - tri.p[1].y) * dv1_step;
                float tex_sw = tri.t[1].w + (float)(i - tri.p[1].y) * dw1_step;

                float tex_eu = tri.t[0].u + (float)(i - tri.p[0].y) * du2_step;
                float tex_ev = tri.t[0].v + (float)(i - tri.p[0].y) * dv2_step;
                float tex_ew = tri.t[0].w + (float)(i - tri.p[0].y) * dw2_step;

                if (ax > bx)
                {
                    swap(ax, bx);
                    swap(tex_su, tex_eu);
                    swap(tex_sv, tex_ev);
                    swap(tex_sw, tex_ew);
                }

                tex_u = tex_su;
                tex_v = tex_sv;
                tex_w = tex_sw;

                float tstep = 1.0f / ((float)(bx - ax));
                float t = 0.0f;

                for (int j = ax; j < bx; j++)
                {
                    tex_u = (1.0f - t) * tex_su + t * tex_eu;
                    tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                    tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                    if (tex_w > get_buffer(j, i))
                    {
                        if (tri.isTextured)
                        {
                            color3i color = get_pixel(&tri.texture, tex_u, tex_v);
                            set_pixel(this->image, j, i, tri.idensity.r * color.r, tri.idensity.g * color.g, tri.idensity.b * color.b);
                        }
                        else
                        {
                            set_pixel(this->image, j, i, tri.color.r * tri.idensity.r, tri.color.g * tri.idensity.g, tri.color.b * tri.idensity.b);
                        }
                        set_buffer(j, i, tex_w);
                    }
                    t += tstep;
                }
            }
        }
    }

    void fill_triangle(triangle tri)
    {
        set_pixel(this->image, tri.p[0].x, tri.p[0].y, 255, 255, 255);
        set_pixel(this->image, tri.p[1].x, tri.p[1].y, 255, 255, 255);
        set_pixel(this->image, tri.p[2].x, tri.p[2].y, 255, 255, 255);
    }

    /**
     *    Init The Camera
     *
     *   \param FOV Field Of View [DEFAULT: 90.0f]
     *   \param w Width of the camera's output [DEFAULT: The Window's Width]
     *   \param h Heigth of the camera's output [DEFAULT: The Window's Heigth]
     *   \param near The nearest object to render [DEFAULT: 0.1f]
     *   \param far The furthest object to render [DEFAULT: 1000.0f]
     *   \param near_cutoff The nearest distance to NOT cut off an object [DEFAULT: 1.0f]
     */
    void init(float FOV, int w, int h, float near, float far, float near_cutoff)
    {
        this->matProj = Matrix_MakeProjection(FOV, (float)h / (float)w, near, far);
        this->fNearCutoff = near_cutoff;

        this->w = w;
        this->h = h;

        this->pDepthBuffer = new float[this->w * this->h];
        this->image = SDL_CreateRGBSurface(0, this->w, this->h, 32, 0, 0, 0, 0);
    }

    void clear()
    {
        for (int x = 0; x < this->w; x++)
        {
            for (int y = 0; y < this->h; y++)
            {
                set_pixel(this->image, x, y, this->infill.r, this->infill.g, this->infill.b);
            }
        }
    }

    void update()
    {
        vec3d vUp;
        vUp.y = -1;

        vec3d vTarget;
        vTarget.z = 1;
        mat4x4 matCameraRotX = Matrix_MakeRotationX(this->rotation.pitch * M_RAD);
        mat4x4 matCameraRotY = Matrix_MakeRotationY(this->rotation.yaw * M_RAD);
        mat4x4 matCameraRotZ = Matrix_MakeRotationZ(this->rotation.roll * M_RAD);
        this->vLookDir = Matrix_MultiplyVector(matCameraRotY, vTarget);
        vTarget = Vector_Add(this->position, this->vLookDir);

        mat4x4 matCamera = Matrix_PointAt(this->position, vTarget, vUp);
        this->matView = Matrix_QuickInverse(matCamera);
        this->matView = Matrix_MultiplyMatrix(this->matView, matCameraRotX);
        this->matView = Matrix_MultiplyMatrix(this->matView, matCameraRotZ);

        for (int i = 0; i < this->w * this->h; i++)
        {
            this->pDepthBuffer[i] = 0.0f;
        }
    }

    void render(triangle tri)
    {
        vec3d vCameraRay = Vector_Sub(tri.p[0], this->position);

        if (Vector_DotProduct(tri.normal, vCameraRay) < 0.0f)
        {
            triangle triViewed, triProjected, triToRaster;

            // Convert World Space --> View Space
            triViewed.p[0] = Matrix_MultiplyVector(this->matView, tri.p[0]);
            triViewed.p[1] = Matrix_MultiplyVector(this->matView, tri.p[1]);
            triViewed.p[2] = Matrix_MultiplyVector(this->matView, tri.p[2]);
            triViewed.color = tri.color;
            triViewed.idensity = tri.idensity;
            triViewed.isTextured = tri.isTextured;
            triViewed.texture = tri.texture;
            triViewed.t[0] = tri.t[0];
            triViewed.t[1] = tri.t[1];
            triViewed.t[2] = tri.t[2];

            int nClippedTriangles = 0;
            triangle clipped[2];
            vec3d nearBorder;
            nearBorder.z = this->fNearCutoff;
            vec3d farBorder;
            farBorder.z = 1.0f;

            nClippedTriangles = Triangle_ClipAgainstPlane(nearBorder, farBorder, triViewed, clipped[0], clipped[1]);

            for (int n = 0; n < nClippedTriangles; n++)
            {
                // Project triangles from 3D --> 2D
                triProjected.p[0] = Matrix_MultiplyVector(this->matProj, clipped[n].p[0]);
                triProjected.p[1] = Matrix_MultiplyVector(this->matProj, clipped[n].p[1]);
                triProjected.p[2] = Matrix_MultiplyVector(this->matProj, clipped[n].p[2]);
                triProjected.color = clipped[n].color;
                triProjected.idensity = clipped[n].idensity;
                triProjected.isTextured = clipped[n].isTextured;
                triProjected.texture = clipped[n].texture;
                triProjected.t[0] = clipped[n].t[0];
                triProjected.t[1] = clipped[n].t[1];
                triProjected.t[2] = clipped[n].t[2];

                triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
                triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
                triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

                triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
                triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
                triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;

                triProjected.t[0].w = 1.0f / triProjected.p[0].w;
                triProjected.t[1].w = 1.0f / triProjected.p[1].w;
                triProjected.t[2].w = 1.0f / triProjected.p[2].w;

                triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
                triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
                triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

                // Scale into view
                vec3d vOffsetView;
                vOffsetView.x = 1.0f;
                vOffsetView.y = 1.0f;
                vOffsetView.z = 0.0f;

                triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
                triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
                triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);

                triProjected.p[0].x *= 0.5 * (float)this->w;
                triProjected.p[0].y *= 0.5 * (float)this->h;
                triProjected.p[1].x *= 0.5 * (float)this->w;
                triProjected.p[1].y *= 0.5 * (float)this->h;
                triProjected.p[2].x *= 0.5 * (float)this->w;
                triProjected.p[2].y *= 0.5 * (float)this->h;

                // Clipping Works Right Here

                triToRaster = triProjected;

                triangle clipped[2];
                vector<triangle> listTriangles;
                listTriangles.push_back(triToRaster);
                int nNewTriangles = 1;

                for (int p = 0; p < 4; p++)
                {
                    int nTrisToAdd = 0;
                    while (nNewTriangles > 0)
                    {
                        triangle test = listTriangles.front();
                        listTriangles.erase(listTriangles.begin());
                        nNewTriangles--;

                        vec3d plane_n;
                        vec3d plane_p;
                        plane_n.w = 1.0f;
                        plane_p.w = 1.0f;

                        switch (p)
                        {
                        case 0:
                            plane_n.x = 0.0f;
                            plane_n.y = 0.0f;
                            plane_n.z = 0.0f;

                            plane_p.x = 0.0f;
                            plane_p.y = 1.0f;
                            plane_p.z = 0.0f;

                            nTrisToAdd = Triangle_ClipAgainstPlane(plane_n, plane_p, test, clipped[0], clipped[1]);
                            break;

                        case 1:
                            plane_n.x = 0.0f;
                            plane_n.y = (float)this->h - 1.0f;
                            plane_n.z = 0.0f;

                            plane_p.x = 0.0f;
                            plane_p.y = -1.0f;
                            plane_p.z = 0.0f;

                            nTrisToAdd = Triangle_ClipAgainstPlane(plane_n, plane_p, test, clipped[0], clipped[1]);
                            break;

                        case 2:
                            plane_n.x = 0.0f;
                            plane_n.y = 0.0f;
                            plane_n.z = 0.0f;

                            plane_p.x = 1.0f;
                            plane_p.y = 0.0f;
                            plane_p.z = 0.0f;

                            nTrisToAdd = Triangle_ClipAgainstPlane(plane_n, plane_p, test, clipped[0], clipped[1]);
                            break;

                        case 3:
                            plane_n.x = (float)this->w - 1.0f;
                            plane_n.y = 0.0f;
                            plane_n.z = 0.0f;

                            plane_p.x = -1.0f;
                            plane_p.y = 0.0f;
                            plane_p.z = 0.0f;

                            nTrisToAdd = Triangle_ClipAgainstPlane(plane_n, plane_p, test, clipped[0], clipped[1]);
                            break;
                        }

                        for (int w = 0; w < nTrisToAdd; w++)
                            listTriangles.push_back(clipped[w]);
                    }
                    nNewTriangles = listTriangles.size();
                }

                for (int i = 0; i < listTriangles.size(); i++)
                {
                    draw_triangle(listTriangles[i]);
                }
            }
        }
    }

    void move_right(float x) // Right
    {
        vec3d vUp;
        vUp.y = -1;
        vec3d vForward = Vector_Mul(this->vLookDir, x);
        vec3d vRight = Vector_CrossProduct(vForward, vUp);
        this->position = Vector_Sub(this->position, vRight);
    }

    void move_forward(float z) // Forward
    {
        vec3d vForward = Vector_Mul(this->vLookDir, z);
        this->position = Vector_Add(this->position, vForward);
    }
};