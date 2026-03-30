#include "particle.h"
#include "utils.h"
#include <SDL2/SDL.h>

struct Particle particle_create(struct Vec2 pos, struct Vec2 vel, enum ParticleType type, int lifetime, int size,
                                int color, int alpha)
{
    struct Particle particle = {pos, vel, type, lifetime, size, color, 0, alpha};
    return particle;
}

struct ParticleManager particlemgr_create()
{
    struct ParticleManager pmgr = {0};
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        pmgr.particles[i] = particle_create((struct Vec2){0, 0}, (struct Vec2){0, 0}, UNDEFINED, 0, 0, 0, 255);
    }

    return pmgr;
}

void particlemgr_render(struct ParticleManager *mgr, SDL_Renderer *renderer)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        struct Particle p = mgr->particles[i];
        if (p.active)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            switch (p.type)
            {
            case IMPACT:
                circle_render(renderer, p.pos, p.size / 2, p.alpha);
                break;

            case UNDEFINED:
            default:
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, p.alpha);
                SDL_RenderFillRect(renderer, &(SDL_Rect){p.pos.x, p.pos.y, 10, 10});
                break;
            }
        }
    }
}

// TODO: create a batch spawn function
void particlemgr_spawn(struct ParticleManager *mgr, struct Vec2 pos, struct Vec2 vel, enum ParticleType type,
                       float size, int lifetime, int alpha)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        struct Particle *p = &mgr->particles[i];
        if (!p->active)
        {
            p->pos = pos;
            p->vel = vel;
            p->size = size;
            p->lifetime = lifetime;
            p->active = 1;
            p->type = type;
            p->alpha = alpha;
            break;
        }
    }
}

void particlemgr_update(struct ParticleManager *mgr, float dt)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        struct Particle *p = &mgr->particles[i];
        if (p->active)
        {
            if (p->lifetime <= 0)
            {
                p->active = 0;
            }

            p->pos.x += p->vel.x * dt;
            p->pos.y += p->vel.y * dt;
            p->lifetime -= dt;

            switch (p->type)
            {
            case IMPACT:
                p->alpha -= 200 * dt;
                if (p->alpha < 0)
                    p->alpha = 0;
                p->size -= 1 * dt;
                break;

            case UNDEFINED:
            default:
                break;
            }
        }
    }
}
