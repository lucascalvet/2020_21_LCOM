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

  //draws pixmap
  for (int row = sp->y; row < sp->y + sp->height; row++) {
    for (int col = sp->x; col < sp->x + sp->width; col++) {
      draw_pixel(col, row, convert_BGR_to_RGB(color_assembler(sp->map, &map_index)));
    }
  }
}

/**
 * @brief erases sprite from screen according to xpm type transparency color
 * @param sp pointer to Sprite "object" to be erased
 * @return none
 */
void(erase_sprite)(Sprite *sp) {
  uint32_t transparency_color = xpm_transparency_color(sp->xpm_type); //gets the transparency color for xpm image type

  for (int row = sp->y; row < sp->y + sp->height; row++) {
    for (int col = sp->x; col < sp->x + sp->width; col++) {
      draw_pixel(col, row, transparency_color);
    }
  }
}

/**
 * @brief moves sprite in screen
 * @param sp pointer to Sprite "object" to be moved
 * @param final_x the destination x coordinate of Sprite
 * @param final_y the destination y coordinate of Sprite
 * @param xspeed displacement in pixels between consecutive frames, in the x axis
 * @param yspeed displacement in pixels between consecutive frames, in the y axis
 * @return none
 */
void(move_sprite)(Sprite *sp, int final_x, int final_y, int xspeed, int yspeed) { //TODO: frame rate not yet implemented
  while (sp->y != final_y && sp->x != final_x) {
    sp->x += xspeed;
    sp->y += yspeed;

    draw_sprite(sp);
    //sleep(1);
  }
}
