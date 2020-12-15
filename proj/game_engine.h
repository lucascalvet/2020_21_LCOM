#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "sprite.h"
#include "xpm_numbers.h"

//game reseved colors (BGR)
#define LAVA_RED 0x2f08f2
#define LAVA_BLUE 0xff3300
#define LAVA_PURPLE 0x95095b
#define GAME_BLACK 0x000000

#define CLOCK_X 50
#define CLOCK_Y 50
#define NUMBERS_SEP 8
#define CLOCK_WIDTH ((XPM_NUMBERS_WIDTH + NUMBERS_SEP)*4 + XPM_COLON_WIDTH)

//handle a character's movement
void (handle_characters_move)(Sprite * firemi, Sprite * waternix, Sprite *background, bool char1_keys[4], bool char2_keys[4], bool* game_over);

void (handle_slider_move)(Sprite * slider, Sprite *background);

void (create_clock)();

void (draw_clock)();

void (tick_clock)();

//checks if characters are in lava or not
bool (check_lava)(Sprite* firemi, Sprite* waternix);

#endif //GAME_ENGINE_H
