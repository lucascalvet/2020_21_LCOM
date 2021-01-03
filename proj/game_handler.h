#ifndef GAME_HANDLER_H
#define GAME_HANDLER_H

#include "rtc.h"
#include "sprite.h"
#include "xpm_numbers.h"
#include "xpm_titles.h"
#include "xpm_mouse.h"

//game reseved colors (BGR)
#define LAVA_RED 0x2f08f2
#define LAVA_BLUE 0xff3300
#define LAVA_PURPLE 0x95095b
#define GAME_BLACK 0x000000
#define NUMBERS_SEP 5
#define CLOCK_WIDTH ((XPM_NUMBERS_WIDTH + NUMBERS_SEP)*4 + XPM_COLON_WIDTH)
#define LAVA_SPEED 1
#define H_RES 800
#define V_RES 600
//enum with all the directions
enum orientation{
  NORTH, SOUTH, EAST, WEST
};

typedef struct {
  int x;
  int y;
  int prev_x;
  int prev_y;
  uint8_t *map;
  unsigned height;
  unsigned width;
  uint32_t transparency_color;
} Cursor;

typedef struct {
  uint8_t *map;
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
  bool south_pressed;
  uint16_t initx;                          //default x coord for button
  uint16_t inity;                          //default y coord for button
  uint16_t finalx;                         //final x coord of button when pressed
  uint16_t finaly;                         //final y coord of button when pressed
} Game_button;

//game board bars
typedef struct {
  Sprite *bar_sprite;                     //pointer to the correspondant sprite object
  uint16_t initx;                          //default x coord for button
  uint16_t inity;                          //default y coord for button
  uint16_t finalx;                         //final x coord of button when pressed
  uint16_t finaly;                         //final y coord of button when pressed
  int final_angle;                          //the angle that the bar does when moved
  int angle;
  int angular_speed;                  //the bar angular speed
  int n_bups;
  Game_button *game_buttons[3];               //pointer to the buttons that triggers the bar movement
} Game_bar;

//game levers
typedef struct {
  Sprite *lever_sprite;       //pointer to the correspondent lever
  Sprite *lever_base_sprite;  //pointer to the lever base sprite
  int angle;           //lever angle
  bool active;         //state of the lever
  bool clicked;
} Game_lever;

//handle a character's movement
void(handle_characters_move)(Sprite *firemi, Sprite *waternix, Sprite *background, bool char1_keys[4], bool char2_keys[4], bool *game_over, int *n_maps_f, int *n_maps_w, int *n_map_2_f, int *n_map_2_w, Sprite *level_collisions);

void (handle_slider_move)(Sprite * slider, Sprite *background, Sprite *level_collisions);

//checks if characters are in lava or not
bool(check_lava)(Sprite *firemi, Sprite *waternix, Sprite *level_collisions);

Cursor * (create_cursor)(unsigned x, unsigned y);

void (update_cursor)(Cursor *cursor, struct packet packet);

void (draw_cursor)(Cursor *cursor, Sprite * background);

void(delete_cursor)(Cursor *cursor);

Clock * (create_clock)(unsigned x, unsigned y);

void (draw_clock)(Clock * clock);

void (draw_date)(rtc_time time, int x, int y, uint8_t* map, xpm_image_t img);

void (tick_clock)(Clock * clock, Sprite * background);

void(delete_clock)(Clock * clock);

//creates a button sprite for the game board
Game_button *(create_game_button)(const xpm_row_t *xpm_button, uint16_t x, uint16_t y, enum orientation orientation_of_button);

//creates a game_bar sprite for the game board
Game_bar *(create_game_bar)(const xpm_row_t *xpm_bar, uint16_t x, uint16_t y, uint16_t finalx, uint16_t finaly, int init_angle, int final_angle, int angular_speed,  Game_button *bups[],  int n_bups);

//creates a game lever
Game_lever *(create_game_lever)(const xpm_row_t *xpm_lever, const xpm_row_t *xpm_lever_base, uint16_t x, uint16_t y);

//deletes game_button sprite "object"
void(delete_game_button)(Game_button *bup);

//deletes game_bar sprite "object"
void(delete_game_bar)(Game_bar *bap);

//deletes game_bar sprite "object"
void(delete_game_lever)(Game_lever *lever);

//handles de buttons dynamics
void(handle_game_button)(Game_button *bup, Sprite *background,  uint16_t n_objs, Sprite* objs[]);

//handles the bar's dynamics
void(handle_game_bar)(Game_bar *bap, Sprite *background, Sprite *objects_to_collide[], int n_objs);

//handles the lever's dynamics
void(handle_game_lever)(Game_lever *lever, struct packet mouse_packet, Cursor * cursor, Sprite * firemi, Sprite * waternix);

//creates random 2d array of line of snow
void(draw_snow)(int min_size, int max_size, int width, int height, int vertical_quantity);

//handles de boxes dynamics
void(handle_game_box)(Sprite *firemi, Sprite *waternix, Sprite * game_box, Sprite *background, Sprite* level_collisions);

//handles the wins
bool(handle_win)(Sprite* firemi, Sprite* waternix, Sprite* level_completed,  int xf, int yf, int xw, int yw, int width, int height);

//handles the losts
void(handle_lost)();

//handles the lava objects
void(handle_lava)(Sprite * lava, Sprite *background, int initx, bool *change, int width);

//handles wind object
void(handle_wind)(Sprite *wind, int init_y, int max_y, Sprite *firemi, Sprite *waternix, int *map_wind, int* speed,  int* speed2, Sprite* level_collisions);

#endif //GAME_HANDLER_H
