#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "sprite.h"

//game reseved colors (BGR)
#define LAVA_RED 0x2f08f2
#define LAVA_BLUE 0xff3300
#define LAVA_PURPLE 0x95095b
#define GAME_BLACK 0x000000

//handle a character's movement
void (handle_characters_move)(Sprite * firemi, Sprite * waternix, Sprite *background, bool char1_keys[4], bool char2_keys[4], bool* game_over);

//checks if characters are in lava or not
bool (check_lava)(Sprite* firemi, Sprite* waternix);

#endif //GAME_ENGINE_H
