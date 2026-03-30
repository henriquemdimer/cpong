#include "utils.h"
#include <SDL2/SDL.h>

// TODO: improve circle rendering algorithm
// this implementation is too inefficient
void circle_render(SDL_Renderer *renderer, struct Vec2 pos, int radius, int alpha)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
    for (int x = pos.x - radius; x < pos.x + radius * 2; x++)
    {
        for (int y = pos.y - radius; y < pos.y + radius * 2; y++)
        {
            int dx = x - pos.x;
            int dy = y - pos.y;
            double distance = (dx * dx) + (dy * dy);
            if (distance <= radius * radius)
            {
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

int check_aabb_collision(struct Vec2 pos1, struct Vec2 pos2, int w1, int h1, int w2, int h2)
{
    if (pos1.x < pos2.x + w2 && pos1.x + w1 > pos2.x && pos1.y < pos2.y + h2 && pos1.y + h1 > pos2.y)
        return 1;
    return 0;
}

float metric_cooldown = 1;
void print_metrics(float dt)
{
    metric_cooldown -= dt;
    if (metric_cooldown > 0)
        return;

    metric_cooldown = 1;
    printf("FPS: %.2f\n%.2f ms\nDelta: %f\n", 1 / dt, dt * 1000, dt);
}

int irandom_range(int min, int max)
{
    return min + rand() % (max - min + 1);
}
