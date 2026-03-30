#include "defs.h"
#include "ball.h"
#include "particle.h"
#include "player.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

int check_win(struct Ball *ball)
{
    if (ball->pos.x <= 0)
        return 2;
    if (ball->pos.x >= WINDOW_WIDTH - BALL_SIZE)
        return 1;
    return 0;
}

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
