#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "sprite.h"
#include "xpm_numbers.h"

//game reseved colors (BGR)
#define LAVA_RED 0x2f08f2
#define LAVA_BLUE 0xff3300
#define LAVA_PURPLE 0x95095b
#define GAME_BLACK 0x000000
#define NUMBERS_SEP 5
#define CLOCK_WIDTH ((XPM_NUMBERS_WIDTH + NUMBERS_SEP)*4 + XPM_COLON_WIDTH)
//enum with all the directions
enum orientation{
  NORTH, SOUTH, EAST, WEST
};

typedef struct {
uint8_t * map; //TODO: Should it be here? Make a clock module or struct?
unsigned x;
unsigned y;
unsigned height;
unsigned width;
unsigned xpm_width;
unsigned count;
uint32_t transparency_color;
} Clock;

//game board buttons
typedef struct {
  Sprite *button_sprite;                  //pointer to the correspondant sprite object
  enum orientation orientation_of_button; //the orientation of the button is pointing to
  bool pressed;                           //state of the button
  uint8_t initx;                          //default x coord for button
  uint8_t inity;                          //default y coord for button
  uint8_t finalx;                         //final x coord of button when pressed
  uint8_t finaly;                         //final y coord of button when pressed
} Game_button;

//game board bar's
typedef struct {
  Sprite *bar_sprite;                     //pointer to the correspondant sprite object
  uint8_t init_angle;                     //the initial angle that the bar does with the ground
  uint8_t final_angle;                    //final angle that the bar does with the ground
  uint8_t angular_speed;                  //the bar angular speed
  Game_button *game_button;               //pointer to the button that triggers the bar movement
} Game_bar;

//handle a character's movement
void(handle_characters_move)(Sprite *firemi, Sprite *waternix, Sprite *background, bool char1_keys[4], bool char2_keys[4], bool *game_over, Game_button *bup);

void (handle_slider_move)(Sprite * slider, Sprite *background);

//checks if characters are in lava or not
bool(check_lava)(Sprite *firemi, Sprite *waternix);

Clock * (create_clock)(unsigned x, unsigned y);

void (draw_clock)(Clock * clock);

void (tick_clock)(Clock * clock, Sprite * background);

void(delete_clock)(Clock * clock);

//creates a button sprite for the game board
Game_button *(create_game_button)(xpm_map_t xpm[], int x, int y, int n_xpms, enum orientation orientation_of_button);

//creates a bar sprite for the game board
Game_bar *(create_game_bar)(xpm_map_t xpm[], int x, int y, int n_xpms,  uint8_t init_angle, uint8_t final_angle, uint8_t angular_speed, Game_button *bup);

//deletes game_button sprite "object"
void(delete_game_button)(Game_button *bup);

//deletes game_bar sprite "object"
void(delete_game_bar)(Game_bar *bap);

//draws game button in screen
void(draw_game_button)(Game_button *bup);

#endif //GAME_ENGINE_H
