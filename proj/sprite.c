#include "sprite.h"
#include <math.h>

/**
 * @brief creates a Sprite
 * @param xpm xpm array with the xpm's to be used
 * @param x x coordinate to initiate the sprite with
 * @param y y coordinate to initiate the sprite with
 * @param n_xpms number os xpm maps that form the sprite
 * @return pointer to Sprite "object" created, in fact to a struct of type Sprite
 */
Sprite *(create_sprite)(xpm_map_t xpm[], int x, int y, int n_xpms) {
  //allocates space for the "object"
  Sprite *sp = (Sprite *) malloc(sizeof(Sprite));

  if (sp == NULL)
    return NULL;

  xpm_image_t img;

  sp->map = xpm_load(xpm[0], XPM_8_8_8, &img);

  sp->xpms[0] = sp->map;

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

  //reads the sprite pixmaps if it has more than one
  for (int i = 1; i < n_xpms; i++) {
    sp->xpms[i] = xpm_load(xpm[i], XPM_8_8_8, &img);
  }

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
 * @brief draws Sprite "objects" in screen at their current coordinates
 * @param sp the sprite to be drawn
 * @return none
 */
void(draw_sprite)(Sprite *sp) {
  int map_index = 0; //to keep track of map index

  uint32_t color;

  //draws pixmap
  for (int row = sp->y; row < sp->y + sp->height; row++) {
    for (int col = sp->x; col < sp->x + sp->width; col++) {
      color = convert_BGR_to_RGB(color_assembler(sp->map, &map_index));
      if (color != sp->transparency_color)
        draw_pixel(col, row, color);
    }
  }
}

/**
 * @brief draw sprite cutted in width
 * @param sp the sprite to be drawn
 * @param width the width after cutted
 * @return none
 */
void(draw_sprite_cutted)(Sprite *sp, int width) {
  int map_index = 0; //to keep track of map index

  uint32_t color;

  //draws pixmap
  for (int row = sp->y; row < sp->y + sp->height; row++) {
    for (int col = sp->x; col < sp->x + width; col++) {
      color = convert_BGR_to_RGB(color_assembler(sp->map, &map_index));
      if (color != sp->transparency_color)
        draw_pixel(col, row, color);
    }
  }
}

/**
 * @brief draws Sprite "objects" in screen at an angle
 * @param sp the sprite to be drawn
 * @param angle the angle of the sprite
 * @param x0 the x coordinate of the rotation anchor
 * @param y0 the y coordinate of the rotation anchor
 * @return none
 */
void(draw_sprite_at_angle)(Sprite *sp, int angle, int x0, int y0) {
  int map_index = 0; //to keep track of map index

  uint32_t color;

  int transformed_x = 0;
  int transformed_y = 0;

  float teta = angle * (M_PI / 180); //current angle in randians

  //draws pixmaps
  for (int row = 0; row < sp->height; row++) {
    for (int col = 0; col < sp->width; col++) {
      color = convert_BGR_to_RGB(color_assembler(sp->map, &map_index));
      if (color != sp->transparency_color) {
        transformed_x = (int)((double) (col - x0) * cos(teta) + (double) (row - y0) * sin(teta) + x0);
        transformed_y = (int)((double) (row - y0) * cos(teta) - (double) (col - x0) * sin(teta) + y0);
        if (transformed_x + sp->x >= 0 && transformed_y + sp->y >= 0)
          draw_pixel(transformed_x + sp->x, transformed_y + sp->y, color);
      }
    }
  }
}

/**
 * @brief checks collisions of sprite at angle to a rectangle
 * @param sp the sprite to be checked
 * @param angle the angle where the sprite is drawn
 * @param x the x position of the rectangle
 * @param y the y position of the rectangle
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @return true if collides, false otherwise
 */
bool(check_collision_sprite_at_angle)(Sprite *sp, int angle, int x, int y, int width, int height) {
  int map_index = 0; //to keep track of map index

  uint32_t color;

  int transformed_x = 0;
  int transformed_y = 0;

  float teta = angle * (M_PI / 180); //current angle in randians

  //draws pixmap
  for (int row = 0; row < sp->height; row++) {
    for (int col = 0; col < sp->width; col++) {
      color = convert_BGR_to_RGB(color_assembler(sp->map, &map_index));
      if (color != sp->transparency_color) {
        transformed_x = (double) col * cos(teta) + (double) row * sin(teta);
        transformed_y = (double) row * cos(teta) - (double) col * sin(teta);

        if (transformed_x + sp->x >= 0 && transformed_y + sp->y >= 0)
          if (transformed_x + sp->x <= x + width && transformed_x + sp->x >= x && transformed_y + sp->y >= y && transformed_y + sp->y <= y + height) {
            return true;
          }
      }
    }
  }
  return false;
}

/**
 * @brief draws a Sprite resized to new width and new height Not working yet tentative of doing this algorithm: Nearest Neighbor Image Scaling
 * @param sp the sprite to be resized
 * @param new_width the sprite's new width
 * @param new_height the sprite's new height
 */
void(draw_resized_sprite)(Sprite *sp, int new_width, int new_height) {
  uint32_t color;

  double x_ratio = (double) sp->width / new_width;
  double y_ratio = (double) sp->height / new_height;

  uint16_t px, py;

  for (int row = sp->y; row < sp->y + new_height; row++) {
    for (int col = sp->x; col < sp->x + new_width; col++) {
      px = (uint16_t) floor(col * x_ratio);
      py = (uint16_t) floor(row * y_ratio);

      //printf("VAluess: %d  %d", px, py);

      color = convert_BGR_to_RGB(pixmap_get_color_by_coordinates(px, py, sp->map, sp->width));

      if (color != sp->transparency_color)
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
 * @param x x coordinate of the area to be restored
 * @param y y coordinate of the area to be restored
 * @param width width of that same area
 * @param height height of that same area
 * @param background the background to be restored
 * @return none
 */
void(restore_background)(uint16_t x, uint16_t y, int width, int height, Sprite *background) {
  int map_index = 0; //to keep track of map index

  if (x < 800 && y < 600) {
    for (int row = y; row < y + height; row++) {
      for (int col = x; col < x + width; col++) {
        map_index = (col + 800 * row) * 3;
        draw_pixel(col, row, convert_BGR_to_RGB(color_assembler(background->map, &map_index)));
      }
    }
  }
}

/**
 * @brief handles a sprite movement using keyboard and a set of 4 keys
 * @param sp the sprite to handle the movement
 * @param keys the array of 4 keys to be used
 * @param n_map control the xpm animation
 * @param n_map_2 control the xpm animation when idle
 * @param screen_width the screen's width
 * @param 
 * @return true if the sprite has changed state
 */
bool(sprite_keyboard_move)(Sprite *sp, bool keys[4], int *n_map, int *n_map_2, int screen_width, int screen_height, Sprite *level_collisisions) {
  int prev_x = sp->x;
  int prev_y = sp->y;
  bool changed = false;
  bool slope_condition = false; //an horizontal line has to be black
  bool on_limit = false;
  //bool object_collision = false;
  if (keys[0]) {
    sp->y += 1; //checking if it is on the ground
    if (check_sprite_xpm0_collision_by_color(sp, 0x0, level_collisisions->map, false))
      sp->yspeed -= JUMP_STEP;
    sp->y -= 1; //restoring y value
  }
  if (keys[1]) {

    //checking for limits to not disturb the gameplay because of animated sprites may collide to walls or objects when they are side to side with them, this prevents it

    //right limit

    if (sp->x + sp->width > screen_width - 15)
      on_limit = true;

    if (on_limit)
      sp->x -= ON_LIMIT_HELP_SPEED;

    on_limit = false;

    //checking for slope
    for (int i = sp->y + sp->height - 12; i < sp->y + sp->height - 1; i++) {
      if (pixmap_get_color_by_coordinates(sp->x - 1, i, level_collisisions->map, level_collisisions->width) == 0x00) {
        slope_condition = true;
      }
    }

    if (slope_condition && sp->x > 15 + 1 && sp->x + sp->width < screen_width - 15 - 1) {
      sp->y -= SLOPE_STEP;
      //if actually collides gets back to normal
      if (check_sprite_collision_by_color(sp, 0x0, level_collisisions->map, false)) {
        sp->y += SLOPE_STEP;
      }
    }

    sp->xspeed -= V_STEP + FRICTION;
    if (sp->yspeed == 0) {
      *n_map_2 = 0;

      switch (*n_map) {
        case 0:
          sp->map = sp->xpms[3];
          break;
        case 1:
          sp->map = sp->xpms[4];
          break;
        case 2:
          sp->map = sp->xpms[5];
          break;
        default:
          break;
      }
      *n_map = (*n_map + 1) % 3;
    }
    else {
      sp->map = sp->xpms[5];
    }
    changed = true;
  }

  if (keys[3]) {
    //left limit

    if (sp->x < 15)
      on_limit = true;

    if (on_limit)
      sp->x += ON_LIMIT_HELP_SPEED;

    on_limit = false;

    //checking for slope
    for (int i = sp->y + sp->height - 12; i < sp->y + sp->height - 1; i++) {
      if (pixmap_get_color_by_coordinates(sp->x + sp->width + 1, i, level_collisisions->map, level_collisisions->width) == 0x00) {
        slope_condition = true;
      }
    }

    if (slope_condition && sp->x > 15 + 1 && sp->x + sp->width < screen_width - 15 - 1) {
      sp->y -= SLOPE_STEP;
      //if actually collides gets back to normal
      if (check_sprite_collision_by_color(sp, 0x0, level_collisisions->map, false)) {
        sp->y += SLOPE_STEP;
      }
    }

    sp->xspeed += V_STEP + FRICTION;
    if (sp->yspeed == 0) {
      *n_map_2 = 0;

      switch (*n_map) { //we can enlarge it to how many sprites we need to put it running
        case 0:
          sp->map = sp->xpms[6];
          break;
        case 1:
          sp->map = sp->xpms[7];
          break;
        case 2:
          sp->map = sp->xpms[8];
          break;
        default:
          break;
      }
      *n_map = (*n_map + 1) % 3;
    }
    else {
      sp->map = sp->xpms[8];
    }
    changed = true;
  }

  if (sp->xspeed > 0) {
    sp->xspeed -= FRICTION;
    if (sp->xspeed < 0)
      sp->xspeed = 0;
  }
  else if (sp->xspeed < 0) {
    sp->xspeed += FRICTION;
    if (sp->xspeed > 0)
      sp->xspeed = 0;
  }
  sp->yspeed += GRAVITY;

  if (sp->xspeed > MAX_V)
    sp->xspeed = MAX_V;
  if (sp->xspeed < -MAX_V)
    sp->xspeed = -MAX_V;

  if (sp->xspeed == 0) {

    if (sp->x + sp->width > screen_width - 7) {
      sp->x = screen_width - 7 - sp->width;
    }

    if (sp->x < 7) {
      sp->x = 7;
    }

    *n_map = 0;
    sp->map = sp->xpms[0];

    //to change between idles
    /*
    switch(*n_map_2){  
        case 0:  
          sp->map = sp->xpms[0];
          break;
        case 1:
          sp->map = sp->xpms[1];
          break;
        case 2:
          sp->map = sp->xpms[2];
          break;
        default:
          break;
      }
       *n_map_2 = (*n_map_2 + 1) % 3;
    */
    changed = true;
  }

  if (sp->yspeed != 0) {
    sp->y += sp->yspeed;
    if (sp->y < 0) sp->y = 0;
    if (check_sprite_xpm0_collision_by_color(sp, 0x0, level_collisisions->map, false)) {
      sp->y = prev_y;
      if (sp->yspeed > 0) {
        for (int step = sp->yspeed - 1; step > 0; step--) {
          sp->y += step;
          if (sp->y < 0) sp->y = 0;
          if (!check_sprite_xpm0_collision_by_color(sp, 0x0, level_collisisions->map, false)) {
            changed = true;
            break;
          }
          sp->y = prev_y;
        }
      }
      else if (sp->yspeed < 0) {
        for (int step = sp->yspeed + 1; step < 0; step++) {
          sp->y += step;
          if (sp->y < 0) sp->y = 0;
          if (!check_sprite_xpm0_collision_by_color(sp, 0x0, level_collisisions->map, false)) {
            changed = true;
            break;
          }
          sp->y = prev_y;
        }
      }
      sp->yspeed = 0;
    }
    else
      changed = true;
  }

  if (sp->xspeed != 0) {
    sp->x += sp->xspeed;
    if (check_sprite_xpm0_collision_by_color(sp, 0x0, level_collisisions->map, false)) {
      sp->x = prev_x;
      if (sp->xspeed > 0) {
        for (int step = sp->xspeed - 1; step > 0; step--) {
          sp->x += step;
          if (!check_sprite_xpm0_collision_by_color(sp, 0x0, level_collisisions->map, false)) {
            changed = true;
            break;
          }
          sp->x = prev_x;
        }
      }
      else if (sp->xspeed < 0) {
        for (int step = sp->xspeed + 1; step < 0; step++) {
          sp->x += step;
          if (!check_sprite_xpm0_collision_by_color(sp, 0x0, level_collisisions->map, false)) {
            changed = true;
            break;
          }
          sp->x = prev_x;
        }
      }
      sp->xspeed = 0;
    }
    else
      changed = true;
  }

  return changed;
  /*
  if (sp->x == prev_x && sp->y == prev_y) return;

  for(int i = 0; i < n_collision_objects; i++){
    if(collision(sp, collision_sprites[i])){
      draw_sprite(collision_sprites[i]); //draws again the sprite that is colliding with
    }
  }
  */
}

//#################### COLLISION FUNCTIONS #################### //

/**
 * @brief checks collision between two sprites (note: this algorithm treats each sprite as a rectangle)
 * @param sp1 the first sprite to check collisions
 * @param sp2 the second sprite to check collisions with the first
 * @return true if collision occurs, false otherwise
 */
bool(collision_two_rects)(Sprite *sp1, Sprite *sp2) {
  if (sp1->x + sp1->width >= sp2->x &&  //sp1 right edge past sp2 left edge
      sp1->x <= sp2->x + sp2->width &&  //sp1 left edge past sp2 right edge
      sp1->y <= sp2->y + sp2->height && //sp1 top edge past sp2 bottom edge
      sp1->y + sp1->height >= sp2->y) { //sp1 bottom edge past sp2 top edge
    return true;
  }
  return false;
}

/**
 * @brief checks collision between a sprite and rectangle
 * @param sp the first sprite to check collisions
 * @param x x coordinate of the rectangle
 * @param y y coordinate of the rectangle
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @return true if collision occurs, false otherwise
 */
bool(collision_one_rect)(Sprite *sp, uint16_t x, uint16_t y, int width, int height) {
  for (int row = sp->y; row < sp->y + sp->height; row++) {
    for (int col = sp->x; col < sp->x + sp->width; col++) {
      if (convert_BGR_to_RGB(pixmap_get_color_by_coordinates(col - sp->x, row - sp->y, sp->xpms[0], sp->width)) != sp->transparency_color) {
        if (col <= x + width && col >= x && row >= y && row <= y + height) {
          return true;
        }
      }
    }
  }
  return false;
}

/**
 * @brief check collisions of sprite against a certain color in the frame buffer and/or a pixmap
 * @param sp sprite "object" to check for collision
 * @param color color to check against with
 * @param pixmap pixmap to check for collision
 * @param vram check only on frame buffer
 * @return true if collision and false otherwise
 */
bool(check_sprite_collision_by_color)(Sprite *sp, uint32_t color, uint8_t *pixmap, bool vram) { //note: it must be used before the sprite is printed and the color is changed
  for (int row = sp->y; row < sp->y + sp->height; row++) {
    for (int col = sp->x; col < sp->x + sp->width; col++) {
      if (convert_BGR_to_RGB(pixmap_get_color_by_coordinates(col - sp->x, row - sp->y, sp->map, sp->width)) != sp->transparency_color) {
        if (vram) {
          if (vram_get_color_by_coordinates(col, row) == color)
            return true;
        }
        else {
          if (pixmap_get_color_by_coordinates(col, row, pixmap, 800) == color)
            return true;
          if (vram_get_color_by_coordinates(col, row) == color)
            return true;
        }
      }
    }
  }
  return false;
}

/**
 * @brief check collisions of sprite's first xpm against a certain color in the frame buffer and/or a pixmap
 * @param sp sprite "object" to check for collision
 * @param color color to check against with
 * @param pixmap pixmap to check for collision
 * @param vram check only on vram
 * @return true if collision and false otherwise
 */
bool(check_sprite_xpm0_collision_by_color)(Sprite *sp, uint32_t color, uint8_t *pixmap, bool vram) { //note: it must be used before the sprite is printed and the color is changed
  for (int row = sp->y; row < sp->y + sp->height; row++) {
    for (int col = sp->x; col < sp->x + sp->width; col++) {
      if (convert_BGR_to_RGB(pixmap_get_color_by_coordinates(col - sp->x, row - sp->y, sp->xpms[0], sp->width)) != sp->transparency_color) {
        if (vram) {
          if (vram_get_color_by_coordinates(col, row) == color)
            return true;
        }
        else {
          if (pixmap_get_color_by_coordinates(col, row, pixmap, 800) == color)
            return true;
          if (vram_get_color_by_coordinates(col, row) == color)
            return true;
        }
      }
    }
  }
  return false;
}

//******* POSSIBLY DEPRECATED ********//
/**
 * @brief moves sprite in screen
 * @param sp pointer to Sprite "object" to be moved
 * @param final_x the destination x coordinate of Sprite
 * @param final_y the destination y coordinate of Sprite
 * @param xspeed displacement in pixels between consecutive frames, in the x axis
 * @param yspeed displacement in pixels between consecutive frames, in the y axis
 * @param background the background to be restored
 * @return none
 */
void(move_sprite)(Sprite *sp, int final_x, int final_y, int xspeed, int yspeed, Sprite *background) {
  while (sp->y != final_y || sp->x != final_x) {
    sp->x += xspeed;
    sp->y += yspeed;

    //just for testing purpose
    if (check_sprite_collision_by_color(sp, 0x0, background->map, false))
      break;

    //restore_background(sp->x - xspeed, sp->y - yspeed, sp->width, sp->height, background);

    draw_sprite(sp);

    tickdelay(micros_to_ticks(20000));
  }
}
