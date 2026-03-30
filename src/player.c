#include "player.h"
#include "defs.h"
#include <SDL2/SDL.h>

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
