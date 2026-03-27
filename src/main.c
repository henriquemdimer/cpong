#include "./defs.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>
#include <stdio.h>

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
    int x;
    int y;
    int vel;
    struct PlayerKeybind keybind;
};

struct Player player_create(enum PlayerSide side)
{
    int x = 0;
    struct PlayerKeybind keybind = {};
    if (side == LEFT)
    {
        keybind.move_up_key = SDL_SCANCODE_W;
        keybind.move_down_key = SDL_SCANCODE_S;
        x = 10;
    }
    else if (side == RIGHT)
    {
        keybind.move_up_key = SDL_SCANCODE_UP;
        keybind.move_down_key = SDL_SCANCODE_DOWN;
        x = WINDOW_WIDTH - 10 - PLAYER_WIDTH;
    }

    struct Player p = {side, x, WINDOW_HEIGHT / 2 - (PLAYER_HEIGHT / 2), 0, keybind};
    return p;
}

void player_render(struct Player *player, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &(SDL_Rect){player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT});
}

void player_update(struct Player *player)
{
    player->y += player->vel;

    if (player->y >= WINDOW_HEIGHT - PLAYER_HEIGHT)
    {
        player->y = WINDOW_HEIGHT - PLAYER_HEIGHT;
    }

    if (player->y <= 0)
    {
        player->y = 0;
    }
}

void player_handle_input(struct Player *player, const Uint8 *state)
{
    player->vel = 0;
    if(state[player->keybind.move_up_key]) player->vel = -PLAYER_SPEED;
    if(state[player->keybind.move_down_key]) player->vel = PLAYER_SPEED;
}

struct Ball
{
    int x;
    int y;
    int velx;
    int vely;
};

struct Ball ball_create(int x, int y)
{
    struct Ball ball = {x, y, BALL_SPEED, BALL_SPEED};
    return ball;
}

void ball_render(struct Ball *ball, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &(SDL_Rect){ball->x, ball->y, BALL_SIZE, BALL_SIZE});
}

void ball_update(struct Ball *ball)
{
    ball->x += ball->velx;
    ball->y += ball->vely;
    if (ball->x <= 0 || ball->x >= WINDOW_WIDTH - BALL_SIZE)
        ball->velx *= -1;
    if (ball->y <= 0 || ball->y >= WINDOW_HEIGHT - BALL_SIZE)
        ball->vely *= -1;
}

int main()
{
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
    struct Ball ball = ball_create(WINDOW_WIDTH / 2 - BALL_SIZE, WINDOW_HEIGHT / 2 - BALL_SIZE);

    SDL_Event event;
    int run = 1;
    while (run)
    {
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

        ball_update(&ball);

        const Uint8 *keyboard = SDL_GetKeyboardState(NULL);
        player_handle_input(&player_one, keyboard);
        player_handle_input(&player_two, keyboard);
        
        player_update(&player_one);
        player_update(&player_two);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        ball_render(&ball, renderer);
        player_render(&player_one, renderer);
        player_render(&player_two, renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
