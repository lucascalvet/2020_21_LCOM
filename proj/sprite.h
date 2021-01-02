#ifndef SPRITE_H
#define SPRITE_H

//LCOM header files
#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <lcom/timer.h>

//c header files
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "video_gr.h"

//environment constants
#define FPS 30
#define V_STEP 1
#define JUMP_STEP 27
#define GRAVITY 3
#define MAX_V 8
#define FRICTION 1
#define SLOPE_STEP 8
#define ON_LIMIT_HELP_SPEED 7

typedef struct {
  int x, y;                     //current position
  uint32_t transparency_color;  //transparency_color
  enum xpm_image_type xpm_type; //xpm image type
  int width, height;            //dimensions
  int xspeed, yspeed;           //current speed
  uint8_t *map;                 //current pixmap
  uint8_t *xpms[11];             //array of xpm's
} Sprite;

//creates a Sprite
Sprite *create_sprite(xpm_map_t xpm[], int x, int y, int n_xpms); //speed set to zero

//deletes Sprite pointed from sp
void(delete_sprite)(Sprite *sp);

//changes the sprite coordinates to x and y
void(change_sprite_coords)(Sprite *sp, int x, int y);

//draws Sprite "objects" in screen at their current coordinates
void(draw_sprite)(Sprite *sp);

//draws a sprite cutted in his width
void(draw_sprite_cutted)(Sprite *sp, int width);

//draws Sprite "objects" in screen at a certain angle
void(draw_sprite_at_angle)(Sprite *sp, int angle, unsigned x, unsigned y);

//checks collisions of sprite at angle to a rectangle
bool(check_collision_sprite_at_angle)(Sprite *sp, int angle, int x, int y, int width, int height);

//draws a Sprite resized to new width and new height
void(draw_resized_sprite)(Sprite *sp, int new_width, int new_height);

//erases Sprite from screen
void(erase_sprite)(Sprite *sp);

//restores the background at a given rectangle to avoid overprocessing
void(restore_background)(uint16_t x, uint16_t y, int width, int height, Sprite *background);

//sprite movement controlled with keyboard using a set of keys
bool(sprite_keyboard_move)(Sprite *sp, bool keys[4],  int *n_map,  int *n_map_2, int screen_width, int screen_height, Sprite *level_collisisions);

//checks collision between two sprites (treating both as a rectangle)
bool(collision_two_rects)(Sprite *sp1, Sprite *sp2);

//checks collision between a sprite and rectangle
bool(collision_one_rect)(Sprite *sp1, uint16_t x, uint16_t y, int width, int height);

//check collisions of sprite against a certain color in vram
bool(check_sprite_collision_by_color)(Sprite *sp, uint32_t color, uint8_t *pixmap, bool vram);

//moves Sprite in screen POSSIBILY DEPRECATED
void(move_sprite)(Sprite *sp, int final_x, int final_y, int xspeed, int yspeed, Sprite *background);

#endif //SPRITE_H
