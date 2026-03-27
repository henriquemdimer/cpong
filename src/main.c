#include "./defs.h"
#include <SDL2/SDL.h>
#include <stdio.h>

struct Vec2
{
    int x;
    int y;
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

void player_update(struct Player *player)
{
    player->pos.y += player->vel;

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

struct Ball
{
    struct Vec2 pos;
    struct Vec2 vel;
    int cooldown;
};

struct Ball ball_create(int x, int y)
{
    struct Ball ball = {(struct Vec2){x, y}, (struct Vec2){BALL_SPEED, BALL_SPEED}, BALL_COOLDOWN};
    return ball;
}

void ball_render(struct Ball *ball, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &(SDL_Rect){ball->pos.x, ball->pos.y, BALL_SIZE, BALL_SIZE});
}

int check_aabb_collision(struct Vec2 pos1, struct Vec2 pos2, int w1, int h1, int w2, int h2)
{
    if (pos1.x < pos2.x + w2 && pos1.x + w1 > pos2.x && pos1.y < pos2.y + h2 && pos1.y + h1 > pos2.y)
        return 1;
    return 0;
}

void ball_update(struct Ball *ball, const struct Vec2 collidables[], size_t size)
{
    ball->cooldown--;
    if (ball->cooldown > 0)
        return;

    ball->pos.x += ball->vel.x;
    ball->pos.y += ball->vel.y;

    for (size_t i = 0; i < size; i++)
    {
        if (check_aabb_collision(ball->pos, collidables[i], BALL_SIZE, BALL_SIZE, PLAYER_WIDTH, PLAYER_HEIGHT))
            ball->vel.x *= -1;
    }

    if (ball->pos.x <= 0)
    {
        ball->pos.x = 0;
        ball->vel.x *= -1;
    }

    if (ball->pos.x >= WINDOW_WIDTH - BALL_SIZE)
    {
        ball->pos.x = WINDOW_WIDTH - BALL_SIZE;
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

int check_win(struct Ball *ball)
{
    if (ball->pos.x <= 0)
        return 2;
    if (ball->pos.x >= WINDOW_WIDTH - BALL_SIZE)
        return 1;
    return 0;
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
        
        ball_update(&ball, (struct Vec2[2]){player_one.pos, player_two.pos}, 2);

        int winner = check_win(&ball);
        if (winner != 0)
        {
            ball = ball_create(WINDOW_WIDTH / 2 - BALL_SIZE, WINDOW_HEIGHT / 2 - BALL_SIZE);

            if (winner == 1)
                player_one.score++;
            else if (winner == 2)
                player_two.score++;
        }


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
