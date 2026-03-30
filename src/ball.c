#include "ball.h"
#include "defs.h"
#include "utils.h"
#include <SDL2/SDL.h>

struct Ball ball_create(int x, int y)
{
    struct Ball ball = {(struct Vec2){x, y}, (struct Vec2){BALL_SPEED, BALL_SPEED}, BALL_COOLDOWN};
    return ball;
}

void ball_render(struct Ball *ball, SDL_Renderer *renderer)
{
    // =========== Render HITBOX ==============
    // SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    // SDL_RenderFillRect(renderer, &(SDL_Rect){ball->pos.x, ball->pos.y, BALL_SIZE, BALL_SIZE});
    circle_render(renderer, (struct Vec2){ball->pos.x + BALL_SIZE / 2, ball->pos.y + BALL_SIZE / 2}, BALL_SIZE / 2,
                  255);
}

void ball_update(struct Ball *ball, float dt, const struct Vec2 collidables[], size_t size)
{
    ball->cooldown -= dt;
    if (ball->cooldown > 0)
        return;

    // collision is broken at high delta time, so i just limited it
    if (dt > 0.032)
        dt = 0.032;

    ball->pos.x += ball->vel.x * dt;
    ball->pos.y += ball->vel.y * dt;

    for (size_t i = 0; i < size; i++)
    {
        if (check_aabb_collision(ball->pos, collidables[i], BALL_SIZE, BALL_SIZE, PLAYER_WIDTH, PLAYER_HEIGHT))
            ball->vel.x *= -1;
    }

    if (ball->pos.y <= 0)
    {
        ball->pos.y = 0;
        ball->vel.y *= -1;
    }

    if (ball->pos.y >= WINDOW_HEIGHT - BALL_SIZE)
    {
        ball->pos.y = WINDOW_HEIGHT - BALL_SIZE;
        ball->vel.y *= -1;
    }
}
