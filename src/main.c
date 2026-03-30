#include "./defs.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
    ==================================================
                        STRUCTS
    ==================================================
*/

struct Vec2
{
    float x;
    float y;
};

struct PlayerKeybind
{
    SDL_Scancode move_up_key;
    SDL_Scancode move_down_key;
};

enum PlayerSide
{
    RIGHT,
    LEFT
};

struct Player
{
    enum PlayerSide side;
    struct Vec2 pos;
    int score;
    float vel;
    struct PlayerKeybind keybind;
};

struct Ball
{
    struct Vec2 pos;
    struct Vec2 vel;
    float cooldown;
};

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
    size_t size;
};

/*
    ==================================================
                        UTILS
    ==================================================
*/

int check_win(struct Ball *ball)
{
    if (ball->pos.x <= 0)
        return 2;
    if (ball->pos.x >= WINDOW_WIDTH - BALL_SIZE)
        return 1;
    return 0;
}

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

/*
    ==================================================
                        PLAYER
    ==================================================
*/

struct Player player_create(enum PlayerSide side)
{
    int x = 0;
    struct PlayerKeybind keybind = {};
    if (side == LEFT)
    {
        keybind.move_up_key = SDL_SCANCODE_W;
        keybind.move_down_key = SDL_SCANCODE_S;
    }
    else if (side == RIGHT)
    {
        keybind.move_up_key = SDL_SCANCODE_UP;
        keybind.move_down_key = SDL_SCANCODE_DOWN;
        x = WINDOW_WIDTH - PLAYER_WIDTH;
    }

    struct Player p = {side, (struct Vec2){x, WINDOW_HEIGHT / 2 - (PLAYER_HEIGHT / 2)}, 0, 0, keybind};
    return p;
}

void player_render(struct Player *player, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &(SDL_Rect){player->pos.x, player->pos.y, PLAYER_WIDTH, PLAYER_HEIGHT});
}

void player_update(struct Player *player, float dt)
{
    player->pos.y += player->vel * dt;

    if (player->pos.y >= WINDOW_HEIGHT - PLAYER_HEIGHT)
        player->pos.y = WINDOW_HEIGHT - PLAYER_HEIGHT;

    if (player->pos.y <= 0)
        player->pos.y = 0;
}

void player_handle_input(struct Player *player, const Uint8 *state)
{
    player->vel = 0;
    if (state[player->keybind.move_up_key])
        player->vel = -PLAYER_SPEED;
    if (state[player->keybind.move_down_key])
        player->vel = PLAYER_SPEED;
}

/*
    ==================================================
                        BALL
    ==================================================
*/

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

/*
    ==================================================
                        PARTICLE
    ==================================================
*/

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

/*
    ==================================================
                        MAIN
    ==================================================
*/

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int show_metrics = 0;
    int target_fps = -1;

    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "--show-metrics") == 0)
                show_metrics = 1;

            if (strcmp(argv[i], "--target-fps") == 0)
            {
                if (argc <= i + 1)
                {
                    printf("Usage: %s [--target-fps NUM|--show-metrics]\n", argv[0]);
                    return 1;
                }
                target_fps = atoi(argv[i + 1]);
            }
        }
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window =
        SDL_CreateWindow("C Ping Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window)
    {
        printf("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        printf("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    struct Player player_one = player_create(LEFT);
    struct Player player_two = player_create(RIGHT);
    struct Ball ball = ball_create(WINDOW_WIDTH / 2 - BALL_SIZE / 2, WINDOW_HEIGHT / 2 - BALL_SIZE / 2);
    struct ParticleManager pmgr = particlemgr_create();

    SDL_Event event;
    int run = 1;

    Uint64 frequency = SDL_GetPerformanceFrequency();
    Uint64 last_time = SDL_GetPerformanceCounter();
    float dt = 0;

    while (run)
    {
        Uint64 now = SDL_GetPerformanceCounter();
        dt = (float)(now - last_time) / frequency;

        if (target_fps > 0)
        {
            // TODO: stop loop and wait until the target delta time is reached
            if ((1.0 / target_fps) > dt)
                continue;
        }

        last_time = now;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                run = 0;
                break;

            default:
                break;
            }
        }

        int winner = check_win(&ball);
        if (winner != 0)
        {
            if (winner == 1)
                player_one.score++;
            else if (winner == 2)
                player_two.score++;

            for (int i = 0; i < 50; i++)
            {
                struct Vec2 pos = {ball.pos.x, ball.pos.y};
                struct Vec2 vel = {0, irandom_range(-100, 100)};
                if (winner == 2)
                {
                    vel.x = irandom_range(10, 100);
                }
                else if (winner == 1)
                {
                    pos.x += BALL_SIZE;
                    vel.x = irandom_range(-100, -10);
                }

                particlemgr_spawn(&pmgr, pos, vel, IMPACT, 10, 2, 255);
            }

            ball = ball_create(WINDOW_WIDTH / 2 - BALL_SIZE, WINDOW_HEIGHT / 2 - BALL_SIZE);
        }

        // INPUT
        const Uint8 *keyboard = SDL_GetKeyboardState(NULL);
        player_handle_input(&player_one, keyboard);
        player_handle_input(&player_two, keyboard);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // UPDATE
        particlemgr_update(&pmgr, dt);
        ball_update(&ball, dt, (struct Vec2[2]){player_one.pos, player_two.pos}, 2);
        player_update(&player_one, dt);
        player_update(&player_two, dt);

        // RENDER
        particlemgr_render(&pmgr, renderer);
        ball_render(&ball, renderer);
        player_render(&player_one, renderer);
        player_render(&player_two, renderer);

        SDL_RenderPresent(renderer);

        if (show_metrics)
            print_metrics(dt);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
