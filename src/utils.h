#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL_render.h>

struct Vec2
{
    float x;
    float y;
};

int check_aabb_collision(struct Vec2 pos1, struct Vec2 pos2, int w1, int h1, int w2, int h2);
void circle_render(SDL_Renderer *renderer, struct Vec2 pos, int radius, int alpha);
void print_metrics(float dt);
int irandom_range(int min, int max);

#endif
