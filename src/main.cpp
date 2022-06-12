#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

#include "engine.h"
#else
#include "include/engine.h"
#endif

engine e;
game_clock clk;
physics p;

float pitch, yaw;
float sensitivity = 1.0f;

bool running = true;

color3f white;

color3i red;
color3i green;
color3i blue;

vec3d nullpos;
vec3d vpos;

bool init()
{
    e.init("Test", 512, 512, SDL_WINDOW_SHOWN, 4);

    // SDL_ShowCursor(0);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // Loading

    e.add("res/obj/thing.obj");
    e.add("res/obj/cube.obj");
    e.add("res/obj/target.obj");

    // Preparing
    red.r = 255;
    red.g = 0;
    red.b = 0;

    green.r = 0;
    green.g = 255;
    green.b = 0;

    blue.r = 0;
    blue.g = 0;
    blue.b = 255;

    vpos.x = 0;
    vpos.y = 10;
    vpos.z = -10;

    e.objs[0].position.x = 0;
    e.objs[0].position.y = 0;
    e.objs[0].position.z = 0;
    e.objs[0].update();

    p.colliders.push_back(e.objs[0].translated);

    e.objs[1].position.x = 0;
    e.objs[1].position.y = 10;
    e.objs[1].position.z = 0;
    e.objs[1].update();

    e.objs[1].set_color(green);
    e.objs[2].set_color(blue);

    e.add(point, white, 50, vpos, nullpos);

    e.cams[0].position.z = -20;
    e.cams[0].position.y = 20;
    e.cams[0].rotation.pitch = 45;

    e.cams[1].position.z = 0;
    e.cams[1].position.y = 20;
    e.cams[1].rotation.pitch = 90;

    e.cams[3].position.z = -30;

    e.cams[4].position.z = 0;
    e.cams[4].position.y = -20;
    e.cams[4].rotation.pitch = -90;

    e.lights[0].position;

    // mix.play_n("asdf", -1);

    return true;
}

bool load = init();

const Uint8 *key;
int cursor_x, cursor_y;

bool loop()
{
    key = SDL_GetKeyboardState(NULL);
    SDL_GetRelativeMouseState(&cursor_x, &cursor_y);

    /*int rot; // === (int)angle(e.cams[0].position, e.objs[0].position);
    int dist = distance(e.cams[0].position, e.objs[0].position);
    mix.setDirection(0, rot, dist, 5);*/

    vec3d d;
    d.y = -1;
    ray r;
    r = p.raycast(e.objs[1].position, d, 10);
    if (r.hit.size() > 0)
    {
        e.objs[2].position = r.hit[0];
    }

    float speed = 10.0f * clk.deltaTime;
    float gravity = 8.0f * clk.deltaTime;

    if (key[SDL_SCANCODE_LCTRL])
        speed = 20.0f * clk.deltaTime;

    if (key[SDL_SCANCODE_SPACE])
        e.objs[1].position.y += speed;

    if (key[SDL_SCANCODE_LSHIFT])
        e.objs[1].position.y -= speed;

    if (key[SDL_SCANCODE_W])
        e.objs[1].position.z += speed;
    if (key[SDL_SCANCODE_S])
        e.objs[1].position.z -= speed;

    if (key[SDL_SCANCODE_D])
        e.objs[1].position.x -= speed;
    if (key[SDL_SCANCODE_A])
        e.objs[1].position.x += speed;

    // e.objs[1].position.y -= gravity;

    if (p.collide(e.objs[0].translated, e.objs[1].translated))
    {
        e.objs[1].set_color(red);
        // e.objs[1].position.y += gravity;
    }
    else
    {
        e.objs[1].set_color(green);
    }

    e.objs[0].update();
    e.objs[1].update();
    e.objs[2].update();

    if (key[SDL_SCANCODE_ESCAPE])
        return false;

    clk.update();

    e.update(clk.deltaTime);

    e.render_class.draw_surface(e.cams[0].image, 0, 0, 256, 256);
    e.render_class.draw_surface(e.cams[1].image, 256, 0, 256, 256);
    e.render_class.draw_surface(e.cams[2].image, 0, 256, 256, 256);
    e.render_class.draw_surface(e.cams[3].image, 256, 256, 256, 256);
    // e.render_class.image(e.cams[0].image);
    e.render_class.update();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return false;
            break;
        }
    }
    return true;
}

#ifdef __EMSCRIPTEN__
void emLoop()
{
    loop();
}
#endif

int main()
{

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(emLoop, 0, 1);
#else
    while (running)
    {
        running = loop();
    }
#endif

    SDL_Quit();

    return 0;
}