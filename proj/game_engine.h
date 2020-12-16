#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "sprite.h"

//game reseved colors (BGR)
#define LAVA_RED 0x2f08f2
#define LAVA_BLUE 0xff3300
#define LAVA_PURPLE 0x95095b
#define GAME_BLACK 0x000000

//enum with all the directions
enum orientation{
  NORTH, SOUTH, EAST, WEST
};

//game board buttons
typedef struct {
  Sprite *button_sprite;                  //pointer to the correspondant sprite object
  enum orientation orientation_of_button; //the orientation of the button is pointing to
  bool pressed;                           //state of the button
  uint16_t initx;                          //default x coord for button
  uint16_t inity;                          //default y coord for button
  uint16_t finalx;                         //final x coord of button when pressed
  uint16_t finaly;                         //final y coord of button when pressed
} Game_button;

//game board bars
typedef struct {
  Sprite *bar_sprite;                     //pointer to the correspondant sprite object
  uint16_t final_angle;                          //the angle that the bar does when moved
  uint16_t angular_speed;                  //the bar angular speed
  Game_button *game_button;               //pointer to the button that triggers the bar movement
} Game_bar;

//handle a character's movement
void(handle_characters_move)(Sprite *firemi, Sprite *waternix, Sprite *background, bool char1_keys[4], bool char2_keys[4], bool *game_over);

//checks if characters are in lava or not
bool(check_lava)(Sprite *firemi, Sprite *waternix);

//creates a game_button sprite for the game board
Game_button *(create_game_button)(xpm_map_t xpm[], uint16_t x, uint16_t y, int n_xpms, enum orientation orientation_of_button);

//creates a game_bar sprite for the game board
Game_bar *(create_game_bar)(xpm_map_t xpm[], uint16_t x, uint16_t y, int n_xpms,  uint16_t angle, uint16_t angular_speed, Game_button *bup);

//deletes game_button sprite "object"
void(delete_game_button)(Game_button *bup);

//deletes game_bar sprite "object"
void(delete_game_bar)(Game_bar *bap);

//
void(handle_game_button)(Game_button *bup, Sprite *background, Sprite* firemi, Sprite* waternix);

//
void(handle_game_bar)(Game_bar *bap, Sprite *background);

#endif //GAME_ENGINE_H
