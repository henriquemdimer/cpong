#ifndef PARTICLE_H
#define PARTICLE_H

#include "defs.h"
#include "utils.h"

enum ParticleType
{
    UNDEFINED,
    IMPACT
};

struct Particle
{
    struct Vec2 pos;
    struct Vec2 vel;
    enum ParticleType type;
    float lifetime;
    float size;
    int color;
    float alpha;
    int active;
};

// TODO: add a active_particles field to improve loop performance
struct ParticleManager
{
    struct Particle particles[MAX_PARTICLES];
    int size;
};

struct Particle particle_create(struct Vec2 pos, struct Vec2 vel, enum ParticleType type, int lifetime, int size,
                                int color, int alpha);
struct ParticleManager particlemgr_create();
void particlemgr_render(struct ParticleManager *mgr, SDL_Renderer *renderer);
void particlemgr_spawn(struct ParticleManager *mgr, struct Vec2 pos, struct Vec2 vel, enum ParticleType type,
                       float size, int lifetime, int alpha);
void particlemgr_update(struct ParticleManager *mgr, float dt);

#endif
