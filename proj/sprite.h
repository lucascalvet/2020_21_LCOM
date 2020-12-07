#ifndef SPRITE_H
#define SPRITE_H

#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  int x, y;                     // current position
  uint32_t transparency_color;  // transparency_color
  enum xpm_image_type xpm_type; //xpm image type
  int width, height;            // dimensions
  int xspeed, yspeed;           // current speed
  uint8_t *map;                 // the pixmap
} Sprite;

//creates a Sprite
Sprite *create_sprite(xpm_map_t xpm, int x, int y); //speed set to zero

//deletes Sprite pointed from sp
void(delete_sprite)(Sprite *sp);

//changes the sprite coordinates to x and y
void(change_sprite_coords)(Sprite *sp, int x, int y);

//draws Sprite in screen
void(draw_sprite)(Sprite *sp);

//erases Sprite from screen
void(erase_sprite)(Sprite *sp);

//moves Sprite in screen
void(move_sprite)(Sprite *sp, int final_x, int final_y, int xspeed, int yspeed);

//check collisions of sprite against a certain color in vram
bool(check_sprite_collision_by_color)(Sprite *sp, uint32_t color);

#endif //SPRITE_H
