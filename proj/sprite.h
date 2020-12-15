#ifndef SPRITE_H
#define SPRITE_H

#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include "video_gr.h"
#include <lcom/timer.h>

#define FPS 30
#define V_STEP 1
#define JUMP_STEP 27
#define GRAVITY 3
#define MAX_V 8
#define FRICTION 1

typedef struct {
  int x, y;                     // current position
  uint32_t transparency_color;  // transparency_color
  enum xpm_image_type xpm_type; //xpm image type
  int width, height;            // dimensions
  int xspeed, yspeed;           // current speed
  uint8_t *map;                 // current pixmap
  uint8_t *xpms[3];             //array of xpm's
} Sprite;

//creates a Sprite
Sprite *create_sprite(xpm_map_t xpm[], int x, int y, int n_xpms); //speed set to zero

//deletes Sprite pointed from sp
void(delete_sprite)(Sprite *sp);

//changes the sprite coordinates to x and y
void(change_sprite_coords)(Sprite *sp, int x, int y);

//draws Sprite in screen
void(draw_sprite)(Sprite *sp);

//erases Sprite from screen
void(erase_sprite)(Sprite *sp);

bool (handle_move)(Sprite *sp, bool keys[4]);

void (handle_characters_move)(Sprite * char1, Sprite * char2, Sprite *background, bool char1_keys[4], bool char2_keys[4], bool* game_over);

//moves Sprite in screen
void(move_sprite)(Sprite *sp, int final_x, int final_y, int xspeed, int yspeed, Sprite *background);

//checks collision between two sprites (treating both as a rectangle)
bool (collision)(Sprite* sp1, Sprite *sp2);

//check collisions of sprite against a certain color in vram
bool(check_sprite_collision_by_color)(Sprite *sp, uint32_t color);

#endif //SPRITE_H
