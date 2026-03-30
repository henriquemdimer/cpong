#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"
#include <SDL2/SDL_scancode.h>

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

struct Player player_create(enum PlayerSide side);
void player_render(struct Player *player, SDL_Renderer *renderer);
void player_update(struct Player *player, float dt);
void player_handle_input(struct Player *player, const Uint8 *state);

#endif
