#include "game_handler.h"
#include <math.h>

/**
 * @brief handle a character's movement
 * 
 * @param firemi one of the characters to move
 * @param waternix the other character to move
 * @param background the game's background (to be restored)
 * @param char1_keys the pressed keys for the movement of the first character
 * @param char2_keys the pressed keys for the movement of the second character
 */
void(handle_characters_move)(Sprite *firemi, Sprite *waternix, Sprite *background, bool char1_keys[4], bool char2_keys[4], bool *game_over, int *n_maps_f, int *n_maps_w, int *n_map_2_f, int *n_map_2_w, Sprite *level_collisions) {
  /*
  int prev_char1_x = firemi->x;
  int prev_char2_x = waternix->x;
  int prev_char1_y = firemi->y;
  int prev_char2_y = waternix->y;
  */

  bool change_char1 = sprite_keyboard_move(firemi, char1_keys, n_maps_f, n_map_2_f, 800, 600, level_collisions);
  bool change_char2 = sprite_keyboard_move(waternix, char2_keys, n_maps_w, n_map_2_w, 800, 600, level_collisions);

/*
  if (change_char1)
    restore_background(prev_char1_x, prev_char1_y, firemi->width, firemi->height, background);
  if (change_char2)
    restore_background(prev_char2_x, prev_char2_y, waternix->width, waternix->height, background);
*/
  if (change_char1 || change_char2) {
    *game_over = check_lava(firemi, waternix, level_collisions);

    draw_sprite(waternix);
    draw_sprite(firemi);
  }
}

void(handle_slider_move)(Sprite *slider, Sprite *background, Sprite *level_collisions) {
  //int prev_slider_x = slider->x;
  if (slider->xspeed == 0) {
    slider->xspeed = 2;
  }
  //restore_background(prev_slider_x, slider->y, slider->width, slider->height, background);
  slider->x += slider->xspeed;
  if (check_sprite_collision_by_color(slider, 0x0, level_collisions->map, false)) {
    slider->xspeed = -slider->xspeed;
    slider->x += slider->xspeed;
  }
  draw_sprite(slider);
}

/**
 * @brief Creates a minutes:seconds clock
 * @return the created clock
 */
Cursor *(create_cursor)(unsigned x, unsigned y) {
  Cursor *cursor = (Cursor *) malloc(sizeof(Cursor));

  xpm_image_t img;
  cursor->x = x;
  cursor->y = y;
  cursor->prev_x = x;
  cursor->prev_y = y;
  cursor->map = xpm_load(xpm_mouse, XPM_8_8_8, &img);
  cursor->height = img.height;
  cursor->transparency_color = xpm_transparency_color(img.type);
  cursor->width = img.width;

  return cursor;
}

/**
 * @brief Updates the cursor position
 * @param cursor a pointer to the cursor to be updated
 * @param packet a packet struct with the cursor movement information
 */
void(update_cursor)(Cursor *cursor, struct packet packet) {
  cursor->x += packet.delta_x;
  cursor->y -= packet.delta_y;
  if (cursor->x >= H_RES)
    cursor->x = H_RES - 1;
  if (cursor->y >= V_RES)
    cursor->y = V_RES - 1;
  if (cursor->x < 0)
    cursor->x = 0;
  if (cursor->y < 0)
    cursor->y = 0;
}

/**
 * @brief Draws the cursor
 * @param cursor the cursor to draw
 */
void(draw_cursor)(Cursor *cursor, Sprite *background) {
  //restore_background(cursor->prev_x, cursor->prev_y, cursor->width, cursor->height, background);
  cursor->prev_x = cursor->x;
  cursor->prev_y = cursor->y;

  int map_index = 0; //to keep track of map index

  uint32_t color;

  //draws cursor
  for (int row = cursor->y; row < cursor->y + (int) cursor->height; row++) {
    for (int col = cursor->x; col < cursor->x + (int) cursor->width; col++) {
      color = convert_BGR_to_RGB(color_assembler(cursor->map, &map_index));
      if (col < H_RES && row < V_RES) {
        if (color != cursor->transparency_color)
          draw_pixel(col, row, color);
      }
    }
  }
}

/**
 * @brief deletes a cursor object
 * @param sp pointer to cursor "object" to be deleted
 * @return none
 */
void(delete_cursor)(Cursor *cursor) {
  if (cursor == NULL)
    return;

  if (cursor->map)
    free(cursor->map);

  free(cursor);
  cursor = NULL; // XXX: pointer is passed by value
  // should do this @ the caller
}

/**
 * @brief Creates a minutes:seconds clock
 * @return the created clock
 */
Clock *(create_clock)(unsigned x, unsigned y) {
  Clock *clock = (Clock *) malloc(sizeof(Clock));

  xpm_image_t img;
  clock->x = x;
  clock->y = y;
  clock->map = xpm_load(xpm_numbers, XPM_8_8_8, &img);
  clock->height = img.height;
  clock->transparency_color = xpm_transparency_color(img.type);
  clock->width = CLOCK_WIDTH;
  clock->xpm_width = img.width;
  clock->count = 0;

  return clock;
}

/**
 * @brief Draws the clock
 * @param clock the clock to draw
 */
void(draw_clock)(Clock *clock) {
  int map_index = 0; //to keep track of map index

  uint32_t color;

  unsigned minutes = clock->count / 60;
  unsigned seconds = clock->count % 60;

  unsigned clock_digits[4] = {minutes / 10, minutes % 10, seconds / 10, seconds % 10};
  //printf("\nClock digits: {%d, %d, %d, %d}", clock_digits[0], clock_digits[1], clock_digits[2], clock_digits[3]);

  //draws pixmap
  for (int i = 0; i < 4; i++) {
    for (unsigned row = 0; row < clock->height; row++) {
      for (unsigned col = 0; col < XPM_NUMBERS_WIDTH; col++) {
        map_index = (row * clock->xpm_width + XPM_NUMBERS_STEP * clock_digits[i] + col) * bits_to_bytes();
        color = convert_BGR_to_RGB(color_assembler(clock->map, &map_index));
        if (color != clock->transparency_color) {
          if (i < 2)
            draw_pixel(col + clock->x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * i, row + clock->y, color);
          else
            draw_pixel(col + clock->x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * i + XPM_COLON_WIDTH + NUMBERS_SEP, row + clock->y, color);
        }
      }
    }
  }
  for (unsigned row = 0; row < clock->height; row++) {
    for (unsigned col = 0; col < XPM_COLON_WIDTH; col++) {
      map_index = (row * clock->xpm_width + XPM_NUMBERS_STEP * 10 + col) * bits_to_bytes();
      color = convert_BGR_to_RGB(color_assembler(clock->map, &map_index));
      if (color != clock->transparency_color) {
        draw_pixel(col + clock->x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * 2, row + clock->y - 3, color);
      }
    }
  }
}

void (draw_date)(rtc_time time, int x, int y, uint8_t *map, xpm_image_t img) {
  int map_index = 0;
  unsigned date_digits[12] = {time.hours / 10, time.hours % 10,
                              time.minutes / 10, time.minutes % 10, 
                              time.seconds / 10, time.seconds % 10, 
                              time.month_day / 10, time.month_day % 10, 
                              time.month / 10, time.month % 10, 
                              time.year / 10, time.year % 10};
  uint32_t color;
  uint32_t transparency_color = xpm_transparency_color(img.type);
  for (int i = 0; i < 12; i++) {
    for (unsigned row = 0; row < 30; row++) {
      for (unsigned col = 0; col < XPM_NUMBERS_WIDTH; col++) {
        map_index = (row * 320 + XPM_NUMBERS_STEP * date_digits[i] + col) * bits_to_bytes();
        color = convert_BGR_to_RGB(color_assembler(map, &map_index));
        if (color != transparency_color) {
          if (i < 2)
            draw_pixel(col + x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * i, row + y, color);
          else if (i < 4)
            draw_pixel(col + x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * i + XPM_COLON_WIDTH + NUMBERS_SEP, row + y, color);
          else if (i < 6)
            draw_pixel(col + x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * i + 2 * (XPM_COLON_WIDTH + NUMBERS_SEP), row + y, color);
          else if (i < 8)
            draw_pixel(col + x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * (i - 6), row + 30 + NUMBERS_SEP + y, color);
          else if (i < 10)
            draw_pixel(col + x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * (i - 6) + XPM_COLON_WIDTH + NUMBERS_SEP, row + 30 + NUMBERS_SEP + y, color);
          else
            draw_pixel(col + x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * (i - 6) + 2 * (XPM_COLON_WIDTH + NUMBERS_SEP), row + 30 + NUMBERS_SEP + y, color);
        }
      }
    }
  }
  for (unsigned row = 0; row < 30; row++) {
    for (unsigned col = 0; col < XPM_COLON_WIDTH; col++) {
      map_index = (row * 320 + XPM_NUMBERS_STEP * 10 + col) * bits_to_bytes();
      color = convert_BGR_to_RGB(color_assembler(map, &map_index));
      if (color != transparency_color) {
        draw_pixel(col + x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * 2, row + y - 3, color);
        draw_pixel(col + x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * 4 + NUMBERS_SEP + XPM_COLON_WIDTH, row + y - 3, color);
      }
    }
  }
  for (unsigned row = 0; row < 30; row++) {
    for (unsigned col = 0; col < XPM_SLASH_WIDTH; col++) {
      map_index = (row * 320 + XPM_NUMBERS_STEP * 10 + XPM_COLON_WIDTH + 2 * NUMBERS_SEP + col) * bits_to_bytes();
      color = convert_BGR_to_RGB(color_assembler(map, &map_index));
      if (color != transparency_color) {
        draw_pixel(col + x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * 2 - 5, row + y + 30 + NUMBERS_SEP, color);
        draw_pixel(col + x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP) * 4 - 5 + NUMBERS_SEP + XPM_COLON_WIDTH, row + y + 30 + NUMBERS_SEP, color);
      }
    }
  }
}

/**
 * @brief Adds one second to the clock
 * @param clock the clock to tick
 * @param background the background to be restored
 */
void(tick_clock)(Clock *clock, Sprite *background) {
  clock->count++;
  //restore_background(clock->x, clock->y, clock->width, clock->height, background);
  draw_clock(clock);
}

/**
 * @brief deletes a clock object
 * @param sp pointer to clock "object" to be deleted
 * @return none
 */
void(delete_clock)(Clock *clock) {
  if (clock == NULL)
    return;

  if (clock->map)
    free(clock->map);

  free(clock);
  clock = NULL; // XXX: pointer is passed by value
  // should do this @ the caller
}

/**
 * @brief checks if characters are in lava or not
 * @param firemi pointer to firemi sprite object
 * @param waternix pointer to waternix sprite object
 * @return true if they are in lava, false otherwise
 */
bool(check_lava)(Sprite *firemi, Sprite *waternix, Sprite *level_collisions) {
  if (check_sprite_collision_by_color(firemi, LAVA_BLUE, level_collisions->map, false))
    return true;
  if (check_sprite_collision_by_color(waternix, LAVA_RED, level_collisions->map, false))
    return true;
  if (check_sprite_collision_by_color(firemi, LAVA_PURPLE, level_collisions->map, false))
    return true;
  if (check_sprite_collision_by_color(waternix, LAVA_PURPLE, level_collisions->map, false))
    return true;
  return false;
}

/**
 * @brief creates a button sprite for the game board
 * @param xpm_button xpm map to be used
 * @param x x coordinate to initiate the sprite with
 * @param y y coordinate to initiate the sprite with
 * @param orientation_of_button represents the button orientation to know where to print it when pressed
 * @return pointer to game_button sprite
 */
Game_button *(create_game_button)(const xpm_row_t *xpm_button, uint16_t x, uint16_t y, enum orientation orientation_of_button) {
  Game_button *bup = (Game_button *) malloc(sizeof(Game_button));

  xpm_map_t xpm[1] = {xpm_button};

  if (bup == NULL)
    return NULL;

  Sprite *sp = create_sprite(xpm, x, y, 1);

  bup->initx = x;
  bup->inity = y;
  bup->button_sprite = sp;
  bup->pressed = false;
  bup->south_pressed = false;
  bup->orientation_of_button = orientation_of_button;

  if (bup->orientation_of_button == NORTH) {
    bup->finalx = bup->button_sprite->x;
    bup->finaly = bup->button_sprite->y + bup->button_sprite->height;
  }
  if (bup->orientation_of_button == SOUTH) {
    bup->finalx = bup->button_sprite->x;
    bup->finaly = bup->button_sprite->y - bup->button_sprite->height + 1;
  }
  if (bup->orientation_of_button == EAST) {
    bup->finalx = bup->button_sprite->x - bup->button_sprite->width + 1;
    bup->finaly = bup->button_sprite->y;
  }
  if (bup->orientation_of_button == WEST) {
    bup->finalx = bup->button_sprite->x + bup->button_sprite->width - 1;
    bup->finaly = bup->button_sprite->y;
  }
  return bup;
}

/**
 * @brief creates a bar sprite for the game board
 * @param xpm_bar xpm map to be used
 * @param x x coordinate to initiate the sprite with
 * @param y y coordinate to initiate the sprite with
 * @param finalx final x coordinate of the sprite with
 * @param finaly final y coordinate of the sprite with
 * @param init_angle the initial angle for the bar
 * @param final_angle the final angle after the bar moves
 * @param angular_speed the angular speed for the bar movement
 * @param game_button pointer to the game_button that triggers the bar to move
 * @return pointer to a game_bar sprite
 * 
 */
Game_bar *(create_game_bar)(const xpm_row_t *xpm_bar, uint16_t x, uint16_t y, uint16_t finalx, uint16_t finaly, int init_angle, int final_angle, int angular_speed, Game_button *bups[], int n_bups) {
  Game_bar *bap = (Game_bar *) malloc(sizeof(Game_bar));

  xpm_map_t xpm[1] = {xpm_bar};

  if (bap == NULL)
    return NULL;

  Sprite *sp = create_sprite(xpm, x, y, 1);

  bap->finalx = finalx;
  bap->finaly = finaly;
  bap->initx = x;
  bap->inity = y;
  bap->bar_sprite = sp;
  bap->final_angle = final_angle;
  bap->angular_speed = angular_speed;
  bap->angle = init_angle;
  bap->n_bups = n_bups;

  for (int i = 0; i < n_bups; i++) {
    bap->game_buttons[i] = bups[i];
  }

  return bap;
}

/**
 * @brief Creates a lever
 * @param xpm_lever a xpm of the lever
 * @param xpm_level_base a xpm of the lever's base
 * @param x the x coordinate of the lever
 * @param y the y coordinate of the lever
 * @return pointer to the created Game_lever
 */
Game_lever *(create_game_lever)(const xpm_row_t *xpm_lever, const xpm_row_t *xpm_lever_red, const xpm_row_t *xpm_lever_base, uint16_t x, uint16_t y) {
  Game_lever *lever = (Game_lever *) malloc(sizeof(Game_lever));

  xpm_map_t xpm_lever_array[2] = {xpm_lever, xpm_lever_red};
  xpm_map_t xpm_lever_base_array[1] = {xpm_lever_base};

  if (lever == NULL)
    return NULL;

  Sprite *lever_sprite = create_sprite(xpm_lever_array, x, y, 2);
  Sprite *lever_base_sprite = create_sprite(xpm_lever_base_array, x, y, 1);

  lever->lever_sprite = lever_sprite;
  lever->lever_base_sprite = lever_base_sprite;
  lever->active = false;
  lever->clicked = false;
  lever->angle = 0;

  return lever;
}

/**
 * @brief deletes game_button sprite
 * @param sp pointer to game_button "object" to be deleted
 * @return none
 */
void(delete_game_button)(Game_button *bup) {
  if (bup == NULL)
    return;

  if (bup->button_sprite) {
    delete_sprite(bup->button_sprite);
  }

  free(bup);
  bup = NULL; // XXX: pointer is passed by value
  // should do this @ the caller
}

/**
 * @brief deletes game_bar sprite
 * @param sp pointer to game_bar "object" to be deleted
 * @return none
 */
void(delete_game_bar)(Game_bar *bap) {
  if (bap == NULL)
    return;

  if (bap->bar_sprite) {
    delete_sprite(bap->bar_sprite);
  }

  free(bap);
  bap = NULL; // XXX: pointer is passed by value
  // should do this @ the caller
}

/**
 * @brief deletes a Game_lever object
 * @param lever pointer to the lever to be deleted
 */
void(delete_game_lever)(Game_lever *lever) {
  if (lever == NULL)
    return;
  if (lever->lever_sprite)
    delete_sprite(lever->lever_sprite);
  if (lever->lever_base_sprite)
    delete_sprite(lever->lever_base_sprite);
  free(lever);
  lever = NULL;
}

/**
 * @brief handles the movement the game button mechanics
 * @param bup pointer to game_button sprite "object"
 * @param background pointer to the background sprite
 * @param n_objs number of objects of objs array
 * @param objs sprites arrays containing objects that can trigger the button and collides with it
 * @return none
 */
void(handle_game_button)(Game_button *bup, Sprite *background, uint16_t n_objs, Sprite *objs[]) {
  //int prev_button_x = bup->button_sprite->x;
  //int prev_button_y = bup->button_sprite->y;

  //restoring the button background after it moves
  //restore_background(prev_button_x, prev_button_y, bup->button_sprite->width, bup->button_sprite->height, background);

  uint16_t rect_y = 0, rect_x = 0;
  int rect_width = 0, rect_height = 0;
  int speed = 1;

  if (bup->orientation_of_button == NORTH) {
    rect_x = bup->button_sprite->x - 2;
    rect_y = bup->button_sprite->y - 2;
    rect_width = bup->button_sprite->width + 2;
    rect_height = bup->button_sprite->height;
  }
  if (bup->orientation_of_button == SOUTH) {
    rect_x = bup->button_sprite->x - 1;
    rect_y = bup->button_sprite->y;
    rect_width = bup->button_sprite->width + 1;
    rect_height = bup->button_sprite->height + 1;
  }
  if (bup->orientation_of_button == EAST) {
    rect_x = bup->button_sprite->x + bup->button_sprite->width + 1;
    rect_y = bup->button_sprite->y;
    rect_width = 1;
    rect_height = bup->button_sprite->height;
  }
  if (bup->orientation_of_button == WEST) {
    rect_x = bup->button_sprite->x - 1;
    rect_y = bup->button_sprite->y;
    rect_width = 1;
    rect_height = bup->button_sprite->height;
  }

  //checking collision of characters with imaginary rectangle representing the button, according its orientation
  bup->pressed = false;
  for (int i = 0; i < n_objs; i++) {
    bup->pressed |= collision_one_rect(objs[i], rect_x, rect_y, rect_width, rect_height);
  }

  if (bup->pressed) {
    if (bup->orientation_of_button == NORTH) {
      if (bup->button_sprite->y != bup->finaly)
        bup->button_sprite->y += speed;
    }
    if (bup->orientation_of_button == SOUTH) {
      if (bup->south_pressed)
        bup->south_pressed = false;
      else
        bup->south_pressed = true;
    }
    if (bup->orientation_of_button == EAST) {
      if (bup->button_sprite->x != bup->finalx)
        bup->button_sprite->x -= speed;
    }
    if (bup->orientation_of_button == WEST) {
      if (bup->button_sprite->x != bup->finalx)
        bup->button_sprite->x += speed;
    }
  }
  else {
    if (bup->orientation_of_button != SOUTH) {
      bup->button_sprite->x = bup->initx;
      bup->button_sprite->y = bup->inity;
    }
  }

  if (bup->orientation_of_button == SOUTH) {
    if (bup->south_pressed) {
      if (bup->button_sprite->y != bup->finaly) {
        bup->button_sprite->y -= speed;
      }
    }
    else {
      if (bup->button_sprite->y != bup->inity) {
        bup->button_sprite->y += speed;
      }
    }
  }

  draw_sprite(bup->button_sprite);
}

/**
 * @brief handles the movement of a game bar object, it can be either angular or linear
 * @param bap pointer to game bar object
 * @param background pointer to the background sprite object
 * @param objects_to_collide objects that stop the bar from moving
 * @param n_objs number of objects to collide
 * @return none
 */
void(handle_game_bar)(Game_bar *bap, Sprite *background, Sprite *objects_to_collide[], int n_objs) {
  //x axis speed when not in agular movement
  int speed = 2;
  bool moving = false;
  bool colliding = false;

  //see if the bar has to move or no
  for (int i = 0; i < bap->n_bups; i++) {
    if (bap->game_buttons[i]->orientation_of_button == SOUTH)
      moving |= bap->game_buttons[i]->south_pressed;
    else
      moving |= bap->game_buttons[i]->pressed;
  }

  //angular movement
  if (bap->angular_speed != 0) {
    if (moving) {
      if (bap->angle != bap->final_angle) {
        //check bar collision with objects
        for (int i = 0; i < n_objs; i++) {
          if (check_collision_sprite_at_angle(bap->bar_sprite, bap->angle + bap->angular_speed, objects_to_collide[i]->x, objects_to_collide[i]->y, objects_to_collide[i]->width, objects_to_collide[i]->height))
            colliding = true;
        }
        if (!colliding)
          bap->angle += bap->angular_speed;
        else {
          if (bap->angle != 0)
            bap->angle -= bap->angular_speed;
        }
      }
    }
    else {
      if (bap->angle != 0) {
        //check bar collision with objects
        for (int i = 0; i < n_objs; i++) {
          if (check_collision_sprite_at_angle(bap->bar_sprite, bap->angle - bap->angular_speed, objects_to_collide[i]->x, objects_to_collide[i]->y, objects_to_collide[i]->width, objects_to_collide[i]->height))
            colliding = true;
        }
        if (!colliding)
          bap->angle -= bap->angular_speed;
        else if (bap->angle != bap->final_angle)
          bap->angle += bap->angular_speed;
      }
    }

    draw_sprite_at_angle(bap->bar_sprite, bap->angle, 0, 0);
  }
  else if (bap->finaly == bap->inity) {
    //horizontal movement
    if (moving) {

      if (bap->bar_sprite->x > bap->finalx && (bap->finalx - bap->initx) < 0) {
        //check bar collision with objects
        for (int i = 0; i < n_objs; i++) {
          if (collision_one_rect(bap->bar_sprite, objects_to_collide[i]->x + 1, objects_to_collide[i]->y - 1, objects_to_collide[i]->width - 1, objects_to_collide[i]->height - 1))
            colliding = true;
        }
        if (!colliding)
          bap->bar_sprite->x -= speed;
      }
      if (bap->bar_sprite->x < bap->finalx && (bap->finalx - bap->initx) > 0) {
        //check bar collision with objects
        for (int i = 0; i < n_objs; i++) {
          if (collision_one_rect(bap->bar_sprite, objects_to_collide[i]->x + 1, objects_to_collide[i]->y - 1, objects_to_collide[i]->width - 1, objects_to_collide[i]->height - 1))
            colliding = true;
        }
        if (!colliding)
          bap->bar_sprite->x += speed;
      }
    }
    else {

      if (bap->bar_sprite->x != bap->initx) {

        if ((bap->finalx - bap->initx) > 0) {
          //check bar collision with objects
          for (int i = 0; i < n_objs; i++) {
            if (collision_one_rect(bap->bar_sprite, objects_to_collide[i]->x + 1, objects_to_collide[i]->y - 1, objects_to_collide[i]->width - 1, objects_to_collide[i]->height - 1))
              colliding = true;
          }
          if (!colliding)
            bap->bar_sprite->x -= speed;
        }

        if ((bap->finalx - bap->initx) < 0) {
          //check bar collision with objects
          for (int i = 0; i < n_objs; i++) {
            if (collision_one_rect(bap->bar_sprite, objects_to_collide[i]->x + 1, objects_to_collide[i]->y - 1, objects_to_collide[i]->width - 1, objects_to_collide[i]->height - 1))
              colliding = true;
          }
          if (!colliding)
            bap->bar_sprite->x += speed;
        }
      }
    }
    draw_sprite(bap->bar_sprite);
  }
  //vertical movement
  else if (bap->finalx == bap->initx) {
    if (moving) {
      if (bap->bar_sprite->y < bap->finaly && (bap->finaly - bap->inity) > 0) {
        //check bar collision with objects
        for (int i = 0; i < n_objs; i++) {
          if (collision_one_rect(bap->bar_sprite, objects_to_collide[i]->x + 1, objects_to_collide[i]->y - 1, objects_to_collide[i]->width - 1, objects_to_collide[i]->height - 1))
            colliding = true;
        }
        if (!colliding)
          bap->bar_sprite->y += speed;
      }
      if (bap->bar_sprite->y > bap->finaly && (bap->finaly - bap->inity) < 0) {
        //check bar collision with objects
        for (int i = 0; i < n_objs; i++) {
          if (collision_one_rect(bap->bar_sprite, objects_to_collide[i]->x + 1, objects_to_collide[i]->y - 1, objects_to_collide[i]->width - 1, objects_to_collide[i]->height - 1))
            colliding = true;
        }
        if (!colliding) {
          bap->bar_sprite->y -= speed;
          bool on_top = false;

          //check if is anyone on top
          for (int i = 0; i < n_objs; i++) {
            if (collision_one_rect(objects_to_collide[i], bap->bar_sprite->x, bap->bar_sprite->y - 1, bap->bar_sprite->width, 2))
              on_top = true;
          }

          if (on_top) {
            //it subs to all off themm because the handle characters is done after is they are actually on top they go down
            for (int i = 0; i < n_objs; i++) {
              objects_to_collide[i]->y -= speed;
            }
          }
        }
      }
    }
    else {
      if (bap->bar_sprite->y != bap->inity) {

        if ((bap->finaly - bap->inity) > 0)
          //check bar collision with objects
          for (int i = 0; i < n_objs; i++) {
            if (collision_one_rect(bap->bar_sprite, objects_to_collide[i]->x + 1, objects_to_collide[i]->y - 1, objects_to_collide[i]->width - 1, objects_to_collide[i]->height - 1))
              colliding = true;
          }
        if (!colliding)
          bap->bar_sprite->y -= speed;

        if ((bap->finaly - bap->inity) < 0)
          //check bar collision with objects
          for (int i = 0; i < n_objs; i++) {
            if (collision_one_rect(bap->bar_sprite, objects_to_collide[i]->x + 1, objects_to_collide[i]->y - 1, objects_to_collide[i]->width - 1, objects_to_collide[i]->height - 1))
              colliding = true;
          }
        if (!colliding)
          bap->bar_sprite->y += speed;
      }
    }
    draw_sprite(bap->bar_sprite);
  }
}

/**
 * @brief Handles the lever game object
 * @param lever pointer to the lever to be handled
 * @param mouse_packet the last received mouse packet
 * @param cursor a pointer to the game cursor
 * @param firemi pointer to the firemi character (for allowing interaction with the lever)
 * @param waternix pointer to the waternix character (for allowing interaction with the lever)
 */
void(handle_game_lever)(Game_lever *lever, struct packet mouse_packet, Cursor * cursor, Sprite * firemi, Sprite * waternix) {
  int cursor_x = cursor->x;
  int cursor_y = cursor->y;
  int lever_x = lever->lever_sprite->x;
  int lever_y = lever->lever_sprite->y;
  if ((firemi->x + firemi->width > lever_x - 30 && firemi->x < lever_x + lever->lever_sprite->width + 30 && firemi->y + firemi->height < lever_y + lever->lever_sprite->height + 3 && firemi->y + 20 > lever_y - 10) || (waternix->x + waternix->width > lever_x - 30 && waternix->x < lever_x + lever->lever_sprite->width + 30 && waternix->y + waternix->height < lever_y + lever->lever_sprite->height + 3 && waternix->y + 20 > lever_y - 10)) {
    lever->lever_sprite->map = lever->lever_sprite->xpms[0];
    if (mouse_packet.lb) {
      if (!lever->active && cursor_x >= lever_x && cursor_x <= lever_x + 10 && cursor_y >= lever_y + 7 && cursor_y <= lever_y + 14)
        lever->clicked = true;
      if (lever->active && cursor_x >= lever_x + 26 && cursor_x <= lever_x + 36 && cursor_y >= lever_y + 7 && cursor_y <= lever_y + 14)
        lever->clicked = true;
    }
  }
  else {
    lever->lever_sprite->map = lever->lever_sprite->xpms[1];
  }
  if (!mouse_packet.lb) {
    if (lever->clicked) {
      if (lever->angle == 40) {
        lever->active = true;
      }
      else {
        lever->active = false;
      }
    }
    lever->clicked = false;
  }
  if (lever->clicked) {
    if (cursor_y >= lever_y + 23) {
      lever->clicked = false;
    }
    if (cursor_x == lever_x + 18) lever->angle = 90;
    else {
      lever->angle = atan((double) (lever_y + 23 - cursor_y) / (double) (cursor_x - (lever_x + 18)))*180 / M_PI;
      if (lever->angle < 0) lever->angle += 180;
      if (lever->angle > 140) lever->angle = 140;
      else if (lever->angle < 40) lever->angle = 40;
    }
  }
  else {
    if (lever->active) lever->angle = 40;
    else lever->angle = 140;
  }
  draw_sprite_at_angle(lever->lever_sprite, lever->angle - 90, 18, 23);
  draw_sprite(lever->lever_base_sprite);
  //printf("Active: %d", lever->active);
}

/**
 * @brief draws a random snow pattern
 * @param min_size the snow flake minimum diameter size
 * @param max_size the snow flake max diameter size
 * @param width the width of the screen
 * @param heigth the height of the screen
 * @param vertical_quantity_size average vertical amount of flakes
 * 
 */
void(draw_snow)(int min_size, int max_size, int width, int height, int vertical_quantity_size) {
  srand(time(0));

  int divisions_h = width / max_size;
  int divisions_v = height / vertical_quantity_size;
  uint16_t x = 0, y = 0, radius = 0;

  uint16_t positions[divisions_v][divisions_h][3]; //to store the snow flakes positions

  //creating random snow
  for (int j = 0; j < divisions_v; j++) {
    for (int i = 0; i < divisions_h; i++) {
      x = i * (rand() % max_size) + i * max_size;
      y = (j + 1) * (rand() % vertical_quantity_size) + j * vertical_quantity_size;
      radius = rand() % max_size + min_size;
      positions[j][i][0] = x;
      positions[j][i][1] = y;
      positions[j][i][2] = radius;
    }
  }

  //drawing the snow
  for (int k = 0; k < 10; k++) {
    for (int j = 0; j < divisions_v; j++) {
      for (int i = 0; i < divisions_h; i++) {
        vg_draw_circle(positions[j][i][0], positions[j][i][1], positions[j][i][2], 0xFFFFFF);
      }
    }
  }
}

/**
 * @brief handles a game box movement, which are pushed by the characters
 * @param firemi the firemi character sprite
 * @param waternix the waternix character sprite
 * @param game_box the box beeing handled
 * @param background the sprite of the backgroung to be restored in movement
 * @param level_collisions the level mask for collisions
 * @return none
 */
void(handle_game_box)(Sprite *firemi, Sprite *waternix, Sprite *game_box, Sprite *background, Sprite *level_collisions) {

  Sprite *chars[2] = {firemi, waternix};
  bool slope = false;

  int speed = 1;
  int gravity = 3;

  //restoring the background
  //restore_background(game_box->x, game_box->y, game_box->width, game_box->height, background);

  for (int i = 0; i < 2; i++) {
    //left movement
    if (collision_one_rect(chars[i], game_box->x + game_box->width + 1, game_box->y, 1, game_box->height)) {

      game_box->x -= speed;

      //if collides restores x
      if (check_sprite_collision_by_color(game_box, 0x00, level_collisions->map, false))
        game_box->x += speed;

      //checks for slope
      if (pixmap_get_color_by_coordinates(game_box->x - 1, game_box->y + game_box->height - 1, level_collisions->map, level_collisions->width) == 0x00) {
        game_box->y -= 1;
        slope = true;
        //if collides goes back
        if (check_sprite_collision_by_color(game_box, 0x00, level_collisions->map, false)) {
          game_box->y += 1;
          slope = false;
        }
      }
    }

    //right movement
    if (collision_one_rect(chars[i], game_box->x - 1, game_box->y, 1, game_box->height)) {

      game_box->x += speed;

      //if collides restores x
      if (check_sprite_collision_by_color(game_box, 0x00, level_collisions->map, false)) {
        game_box->x -= speed;
      }

      //checks for slope
      if (pixmap_get_color_by_coordinates(game_box->x + game_box->width + 1, game_box->y + game_box->height - 1, level_collisions->map, level_collisions->width) == 0x00) {
        slope = true;
        game_box->y -= 1;
        if (check_sprite_collision_by_color(game_box, 0x00, level_collisions->map, false)) {
          game_box->y += 1;
          slope = false;
        }
      }
    }

    //gravity
    if (!slope) {
      if (!check_sprite_collision_by_color(game_box, 0x00, level_collisions->map, false)) {
        game_box->y += gravity;
        //not passing through the floor
        while (check_sprite_collision_by_color(game_box, 0x00, level_collisions->map, false)) {
          game_box->y -= 1;
        }
      }
    }
  }

  draw_sprite(game_box);
}

/**
 * @brief handles when user wins a level
 * @param firemi the firemi character
 * @param waternix the waternix character
 * @param level_completed the title of the level
 * @return none
 */
bool(handle_win)(Sprite *firemi, Sprite *waternix, Sprite *level_completed, int xf, int yf, int xw, int yw, int width, int height) {
  if (collision_one_rect(waternix, xw, yw, width, height) && collision_one_rect(firemi, xf, yf, width, height)) {
    draw_sprite(level_completed);
    copy_buffer_to_vram();
    sleep(2);
    return true;
  }
  return false;
}

/**
 * @brief handles when user loses a level
 * 
 */
void(handle_lost)() {
  xpm_map_t game_over_title_xpm_array[1] = {game_over_title_xpm};
  Sprite *game_over_title = create_sprite(game_over_title_xpm_array, 0, 0, 1);
  draw_sprite(game_over_title);
  copy_buffer_to_vram();
  sleep(2);
  delete_sprite(game_over_title);
}

/**
 * @brief handles lava objects
 * @param lava sprite of the lava object to handle
 * @param background
 * @return none
 */
void(handle_lava)(Sprite *lava, Sprite *background, int initx, bool *change, int width) {

  restore_background(lava->x, lava->y, lava->width, lava->height, background);

  draw_sprite(lava);

  if (initx - width >= 0) {
    restore_background(initx - (width), lava->y, width, lava->height, background);
    restore_background(initx + width, lava->y, (lava->width), lava->height, background);
  }
  else {
    restore_background(0, lava->y, initx, lava->height, background);
    restore_background(initx + width, lava->y, (lava->width), lava->height, background);
  }

  if (!(*change)) {
    if ((abs(lava->x - initx) >= width) || lava->x <= 0) {
      (*change) = !(*change);
    }
  }
  else {
    if (lava->x >= initx)
      (*change) = !(*change);
  }

  if ((*change)) {
    lava->x += LAVA_SPEED;
    lava->x = initx;
  }
  else {
    lava->x -= LAVA_SPEED;
  }
}

/**
 * @brief handles wind object
 * @param wind the wind sprite object
 * @param x the x of the rectangle the defines the action space of the wind
 * @param y the where the wind starts
 * @param width the width of the rectangle the defines the action space of the wind
 * @param max_y the maximum that characters can reache with wind
 */
void(handle_wind)(Sprite *wind, int init_y, int max_y, Sprite *firemi, Sprite *waternix, int *map_wind, int *speed, int *speed2, Sprite *level_collisions) {
  bool in_wind_f = false;
  bool in_wind_w = false;

  int y_draw_max = wind->y;
  int wind_acelaration = 5;

  for (int i = firemi->x; i < firemi->x + firemi->width; i++) {
    if (i > wind->x + 10 && i < wind->x + wind->width) {
      in_wind_f = true;
      break;
    }
  }

  for (int i = waternix->x; i < waternix->x + waternix->width; i++) {
    if (i > wind->x + 10 && i < wind->x + wind->width) {
      in_wind_w = true;
      break;
    }
  }

  if (in_wind_f && firemi->y >= max_y && firemi->y <= init_y) {
    firemi->y -= (*speed);
    if ((*speed) < 50) {
      (*speed) += wind_acelaration;
    }
    /*
    if((*speed) > 20 && check_sprite_collision_by_color(firemi, 0x00, level_collisions->map, false) == 0x00){
      firemi->y += (*speed);
      (*speed) = 0;
    }
    */
  }
  else {
    (*speed) = 0;
  }

  if (in_wind_w && waternix->y >= max_y && waternix->y <= init_y) {
    waternix->y -= (*speed2);
    if ((*speed2) < 50) {
      (*speed2) += wind_acelaration;
    }
    /*
    if((*speed2) > 20 && check_sprite_collision_by_color(waternix, 0x00, level_collisions->map, false) == 0x00){
      waternix->y += (*speed2);
      (*speed2) = 0;
    }
    */
  }
  else {
    (*speed2) = 0;
  }

  if (((in_wind_f && firemi->y > max_y && firemi->y <= init_y) || (in_wind_w && waternix->y > max_y && waternix->y <= init_y)) && (firemi->y > y_draw_max || waternix->y > y_draw_max)) {
    switch ((*map_wind)) {
      case 0:
        wind->map = wind->xpms[0];
        break;
      case 1:
        wind->map = wind->xpms[1];
        break;
      case 2:
        wind->map = wind->xpms[2];
        break;
      case 3:
        wind->map = wind->xpms[3];
        break;
      case 4:
        wind->map = wind->xpms[4];
        break;
      case 5:
        wind->map = wind->xpms[5];
        break;
      default:
        break;
    }
    (*map_wind) = ((*map_wind) + 1) % 6;

    draw_sprite(wind);
  }
}
