#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <stdbool.h>
#include <lcom/lab4.h>

//enum with the possible game states
enum game_state {
  MAIN_MENU, LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4
};

void (create_level)(enum game_state state);

void (draw_level)(enum game_state state);

void (handle_level)(enum game_state * state, bool keys_firemi[4], bool keys_waternix[4]);

void (tick_game_clock)();

void (update_game_cursor)(enum game_state * state, struct packet packet);

void (delete_level)(enum game_state state);

#endif