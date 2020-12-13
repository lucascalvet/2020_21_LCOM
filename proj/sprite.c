#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include "sprite.h"
#include "video_gr.h"
#include <lcom/timer.h>

/**
 * @brief creates a Sprite
 * @param xpm pointer to xpm to be used as image
 * @return pointer to Sprite "object" created, in fact to a structer of type Sprite
 */
Sprite *(create_sprite)(xpm_map_t xpm, int x, int y) {
  //allocates space for the "object"
  Sprite *sp = (Sprite *) malloc(sizeof(Sprite));

  if (sp == NULL)
    return NULL;

  xpm_image_t img;

  //reads the sprite pixmap
  sp->map = xpm_load(xpm, XPM_8_8_8, &img);

  if (sp->map == NULL) {
    free(sp);
    return NULL;
  }

  //assigning the Sprite "atributes"
  sp->x = x;
  sp->y = y;
  sp->width = img.width;
  sp->height = img.height;
  sp->xspeed = 0;
  sp->yspeed = 0;
  sp->xpm_type = img.type;
  sp->transparency_color = xpm_transparency_color(sp->xpm_type); //gets the transparency color for xpm image type

  return sp;
}

/**
 * @brief deletes Sprite pointed from sp
 * @param sp pointer to Sprite "object" to be deleted
 * @return none
 */
void(delete_sprite)(Sprite *sp) {
  if (sp == NULL)
    return;

  if (sp->map)
    free(sp->map);

  free(sp);
  sp = NULL; // XXX: pointer is passed by value
  // should do this @ the caller
}

/**
 * @brief changes the sprite coordinates to x and y
 * @param sp pointer to Sprite "object" to be altered
 * @param x x coordinate of upper left corner
 * @param y y coordinate of upper left corner
 * @return none
 */
void(change_sprite_coords)(Sprite *sp, int x, int y) {
  sp->x = x;
  sp->y = y;
}

/**
 * @brief draws Sprite "object" in screen at coordinates indicated
 * @param x the x coordinate of Sprite
 * @param y the y coordinate of Sprite
 * @return none
 */
void(draw_sprite)(Sprite *sp) {
  int map_index = 0; //to keep track of map index

  uint32_t color;

  //draws pixmap
  for (int row = sp->y; row < sp->y + sp->height; row++) {
    for (int col = sp->x; col < sp->x + sp->width; col++) {
      color = convert_BGR_to_RGB(color_assembler(sp->map, &map_index));
      if(color != sp->transparency_color)
        draw_pixel(col, row, color);
    }
  }
}

/**
 * @brief erases sprite from screen according to xpm type transparency color
 * @param sp pointer to Sprite "object" to be erased
 * @return none
 */
void(erase_sprite)(Sprite *sp) {

  for (int row = sp->y; row < sp->y + sp->height; row++) {
    for (int col = sp->x; col < sp->x + sp->width; col++) {
      draw_pixel(col, row, sp->transparency_color);
    }
  }
}

/**
 * @brief restores the background at a given rectangle to avoid overprocessing
 * @param
 * @param
 * @param
 * @param
 * @return none
 */
void(restore_background)(uint16_t x, uint16_t y, int width, int height, Sprite *background){
    int map_index = 0; //to keep track of map index

    for (int row = y; row < y + height; row++) {
      for (int col = x; col < x + width; col++) {
      map_index = (col + 800 * row) * 3; 
      draw_pixel(col, row, convert_BGR_to_RGB(color_assembler(background->map, &map_index)));
    }
  }
}

/**
 * @brief handle a sprite movement using keyboard and a set of 4 keys
 * @param sp the sprite to handle the movement
 * @param collision_sprites pointer arrays to objects that can collide to that so that each object 
 * doesn't deleat each other while restoring background
 * @param n_collision_objects the size of the collision sprite array
 * @return none
 */

//TODO: temporary for testing. Frame rate handled in proj.c. Later merging with move_sprite
void(handle_move)(Sprite *sp, int xspeed, int yspeed, Sprite *background, bool keys[4], Sprite* collision_sprites[], int n_collision_objects) {
  int prev_x = sp->x;
  int prev_y = sp->y;

  if(keys[0]) {
    sp->y += 1; //checking if it is on the ground TODO: not very pretty, but it works...
    if(check_sprite_collision_by_color(sp, 0x0))
      sp->yspeed -= JUMP_STEP;
    sp->y -= 1; //restoring y value
  }
  if(keys[1]) sp->xspeed -= V_STEP;
  if(keys[2]) sp->yspeed += V_STEP;
  if(keys[3]) sp->xspeed += V_STEP;

  if (sp->xspeed > 0) {
    sp->xspeed -= FRICTION;
    if (sp->xspeed < 0) sp->xspeed = 0;
  }
  else if (sp->xspeed < 0) {
    sp->xspeed += FRICTION;
    if (sp->xspeed > 0) sp->xspeed = 0;
  }
  sp->yspeed += GRAVITY;

  if (sp->xspeed > MAX_V) sp->xspeed = MAX_V;
  if (sp->xspeed < -MAX_V) sp->xspeed = -MAX_V;

  sp->y += sp->yspeed;
  if (check_sprite_collision_by_color(sp, 0x0)) {
    sp->y = prev_y;
    if (sp->yspeed > 0){
      for (int step = sp->yspeed - 1; step > 0; step--) {
        sp->y += step;
        if (!check_sprite_collision_by_color(sp, 0x0)) break;
        sp->y = prev_y;
      }
    }
    else if (sp->yspeed < 0){
      for (int step = sp->yspeed + 1; step < 0; step++) {
        sp->y += step;
        if (!check_sprite_collision_by_color(sp, 0x0)) break;
        sp->y = prev_y;
      }
    }
    sp->yspeed = 0;
  }

  sp->x += sp->xspeed;
  if (check_sprite_collision_by_color(sp, 0x0)) {
    sp->x = prev_x;
    if (sp->xspeed > 0){
      for (int step = sp->xspeed - 1; step > 0; step--) {
        sp->x += step;
        if (!check_sprite_collision_by_color(sp, 0x0)) break;
        sp->x = prev_x;
      }
    }
    else if (sp->xspeed < 0){
      for (int step = sp->xspeed + 1; step < 0; step++) {
        sp->x += step;
        if (!check_sprite_collision_by_color(sp, 0x0)) break;
        sp->x = prev_x;
      }
    }
    sp->xspeed = 0;
  }

  if(check_sprite_collision_by_color(sp, 0x0)){
    sp->x = prev_x;
    sp->y = prev_y;
    return;
  }

  if (sp->x == prev_x && sp->y == prev_y) return;

  for(int i = 0; i < n_collision_objects; i++){
    if(collision(sp, collision_sprites[i])){
      draw_sprite(collision_sprites[i]); //draws again the sprite that is colliding with
    }
  }
  
  restore_background(prev_x, prev_y, sp->width, sp->height, background);

  draw_sprite(sp);
}

//******* KINDA DEPRECATED ********//
/**
 * @brief moves sprite in screen
 * @param sp pointer to Sprite "object" to be moved
 * @param final_x the destination x coordinate of Sprite
 * @param final_y the destination y coordinate of Sprite
 * @param xspeed displacement in pixels between consecutive frames, in the x axis
 * @param yspeed displacement in pixels between consecutive frames, in the y axis
 * @return none
 */
void(move_sprite)(Sprite *sp, int final_x, int final_y, int xspeed, int yspeed, Sprite *background) { //TODO: frame rate not yet implemented
  while (sp->y != final_y || sp->x != final_x) {
    sp->x += xspeed;
    sp->y += yspeed;

    //just for testing purpose
    if(check_sprite_collision_by_color(sp, 0x0))
      break;
    
    restore_background(sp->x-xspeed, sp->y-yspeed, sp->width, sp->height, background);

    draw_sprite(sp);

    tickdelay(micros_to_ticks(20000)); 
  }
}

//#################### COLLISION FUNCTIONS #################### //

/**
 * @brief checks collision between two sprites (note: this algorithm treats each sprite as a rectangle)
 * @param sp1 the first sprite to check collisions
 * @param sp2 the second sprite to check collisions with the first
 * @return true if collision occurs, false otherwise
 */
bool (collision)(Sprite* sp1, Sprite *sp2){
  if (sp1->x + sp1->width >= sp2->x &&       //sp1 right edge past sp2 left edge
      sp1->x <= sp2->x + sp2->width &&      //sp1 left edge past sp2 right edge
      sp1->y <= sp2->y + sp2->height &&     //sp1 top edge past sp2 bottom edge
      sp1->y + sp1->height >= sp2->y) {  //sp1 bottom edge past sp2 top edge
        return true;
  }
  return false;
}

/**
 * @brief check collisions of sprite against a certain color in vram
 * @param sp sprite "object" to check for collision
 * @param color color to check against with
 * @return true if collision and false otherwise
 */
bool(check_sprite_collision_by_color)(Sprite *sp, uint32_t color) { //note: it must be used before the sprite is printed and the color is changed
  for (int row = sp->y; row < sp->y + sp->height; row++) {
    for (int col = sp->x; col < sp->x + sp->width; col++) {
      if(convert_BGR_to_RGB(pixmap_get_color_by_coordinates(col - sp->x, row - sp->y, sp->map, sp->width)) != sp->transparency_color){
        if (vram_get_color_by_coordinates(col, row) == color)
          return true;
      }
    }
  }
  return false;
}

