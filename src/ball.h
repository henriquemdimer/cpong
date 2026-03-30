#ifndef BALL_H
#define BALL_H

#include "utils.h"

struct Ball
{
    struct Vec2 pos;
    struct Vec2 vel;
    float cooldown;
};

struct Ball ball_create(int x, int y);
void ball_render(struct Ball *ball, SDL_Renderer *renderer);
void ball_update(struct Ball *ball, float dt, const struct Vec2 collidables[], size_t size);

#endif
