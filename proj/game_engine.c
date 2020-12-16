#include "game_engine.h"

/**
 * @brief handle a character's movement
 * 
 * @param firemi one of the characters to move
 * @param waternix the other character to move
 * @param background the game's background (to be restored)
 * @param char1_keys the pressed keys for the movement of the first character
 * @param char2_keys the pressed keys for the movement of the second character
 */
void(handle_characters_move)(Sprite *firemi, Sprite *waternix, Sprite *background, bool char1_keys[4], bool char2_keys[4], bool *game_over) {
  int prev_char1_x = firemi->x;
  int prev_char2_x = waternix->x;
  int prev_char1_y = firemi->y;
  int prev_char2_y = waternix->y;
  bool change_char1 = sprite_keyboard_move(firemi, char1_keys);
  bool change_char2 = sprite_keyboard_move(waternix, char2_keys);
  if (change_char1)
    restore_background(prev_char1_x, prev_char1_y, firemi->width, firemi->height, background);
  if (change_char2)
    restore_background(prev_char2_x, prev_char2_y, waternix->width, waternix->height, background);

  if (change_char1 || change_char2) {
    *game_over = check_lava(firemi, waternix);

    draw_sprite(waternix);
    draw_sprite(firemi);
  }
}

void (handle_slider_move)(Sprite * slider, Sprite *background){
  int prev_slider_x = slider->x;
  if (slider->xspeed == 0) {
    slider->xspeed = 2;
  }
  restore_background(prev_slider_x, slider->y, slider->width, slider->height, background);
  slider->x += slider->xspeed;
  if (check_sprite_collision_by_color(slider, 0x0)){
    slider->xspeed = -slider->xspeed;
    slider->x += slider->xspeed;
  }
  draw_sprite(slider);
}

/**
 * @brief Creates a minutes:seconds clock
 * @return the created clock
 */
Clock * (create_clock)(unsigned x, unsigned y){
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
void (draw_clock)(Clock * clock){
  int map_index = 0; //to keep track of map index

  uint32_t color;

  unsigned minutes = clock->count/60;
  unsigned seconds = clock->count%60;

  unsigned clock_digits[4] = {minutes/10, minutes%10, seconds/10, seconds%10};
  printf("\nClock digits: {%d, %d, %d, %d}", clock_digits[0], clock_digits[1], clock_digits[2], clock_digits[3]);

  //draws pixmap
  for (int i = 0; i < 4; i++){
    for (unsigned row = 0; row < clock->height; row++) {
      for (unsigned col = 0; col < XPM_NUMBERS_WIDTH; col++) {
        map_index = (row * clock->xpm_width + XPM_NUMBERS_STEP * clock_digits[i] + col)*bits_to_bytes();
        color = convert_BGR_to_RGB(color_assembler(clock->map, &map_index));
        if (color != clock->transparency_color){
          if (i < 2)
            draw_pixel(col + clock->x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP)*i, row + clock->y, color);
          else
            draw_pixel(col + clock->x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP)*i + XPM_COLON_WIDTH + NUMBERS_SEP, row + clock->y, color);
        }
      }
    }
  }
  for (unsigned row = 0; row < clock->height; row++) {
      for (unsigned col = 0; col < XPM_COLON_WIDTH; col++) {
        map_index = (row * clock->xpm_width + XPM_NUMBERS_STEP * 10 + col)*bits_to_bytes();
        color = convert_BGR_to_RGB(color_assembler(clock->map, &map_index));
        if (color != clock->transparency_color){
            draw_pixel(col + clock->x + (XPM_NUMBERS_WIDTH + NUMBERS_SEP)*2, row + clock->y - 3, color);
        }
      }
    }
}

/**
 * @brief Adds one second to the clock
 * @param clock the clock to tick
 * @param background the background to be restored
 */
void (tick_clock)(Clock * clock, Sprite * background){
  clock->count++;
  restore_background(clock->x, clock->y, clock->width, clock->height, background);
  draw_clock(clock);
}

/**
 * @brief deletes a clock object
 * @param sp pointer to clock "object" to be deleted
 * @return none
 */
void(delete_clock)(Clock * clock) {
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
bool(check_lava)(Sprite *firemi, Sprite *waternix) {
  if (check_sprite_collision_by_color(firemi, LAVA_BLUE))
    return true;
  if (check_sprite_collision_by_color(waternix, LAVA_RED))
    return true;
  if (check_sprite_collision_by_color(firemi, LAVA_PURPLE))
    return true;
  if (check_sprite_collision_by_color(waternix, LAVA_PURPLE))
    return true;
  return false;
}

/**
 * @brief creates a button sprite for the game board
 * @param xpm xpm array with the xpm's to be used
 * @param x x coordinate to initiate the sprite with
 * @param y y coordinate to initiate the sprite with
 * @param n_xpms number os xpm maps that form the sprite
 * @param orientation_of_button represents the button orientation to know where to print it when pressed
 * @return pointer to game_button sprite
 */
Game_button *(create_game_button)(xpm_map_t xpm[], uint16_t x, uint16_t y, int n_xpms, enum orientation orientation_of_button) {
  Game_button *bup = (Game_button *) malloc(sizeof(Game_button));

  if (bup == NULL)
    return NULL;

  Sprite *sp = create_sprite(xpm, x, y, n_xpms);

  bup->initx = x;
  bup->inity = y;
  bup->button_sprite = sp;
  bup->pressed = false;
  bup->orientation_of_button = orientation_of_button;

  if (bup->orientation_of_button == NORTH) {
    bup->finalx = bup->button_sprite->x;
    bup->finaly = bup->button_sprite->y + bup->button_sprite->height / 2 + bup->button_sprite->height / 4; //goes until 3/4 of its height
  }
  if (bup->orientation_of_button == SOUTH) {
    bup->finalx = bup->button_sprite->x;
    bup->finaly = bup->button_sprite->y - bup->button_sprite->height / 2 - bup->button_sprite->height / 4;
  }
  if (bup->orientation_of_button == EAST) {
    bup->finalx = bup->button_sprite->x - bup->button_sprite->width / 2 - bup->button_sprite->width / 4;
    bup->finaly = bup->button_sprite->y;
  }
  if (bup->orientation_of_button == WEST) {
    bup->finalx = bup->button_sprite->x + bup->button_sprite->width / 2 + bup->button_sprite->width / 4;
    bup->finaly = bup->button_sprite->y;
  }

  return bup;
}

/**
 * @brief creates a bar sprite for the game board
 * @param xpm xpm array with the xpm's to be used
 * @param x x coordinate to initiate the sprite with
 * @param y y coordinate to initiate the sprite with
 * @param n_xpms number os xpm maps that form the sprite
 * @param init_angle the initial angle for the bar
 * @param final_angle the final angle after the bar moves
 * @param angular_speed the angular speed for the bar movement
 * @param game_button pointer to the game_button that triggers the bar to move
 * @return pointer to a game_bar sprite
 * 
 */
Game_bar *(create_game_bar)(xpm_map_t xpm[], uint16_t x, uint16_t y, int n_xpms, uint16_t final_angle, uint16_t angular_speed, Game_button *bup) {
  Game_bar *bap = (Game_bar *) malloc(sizeof(Game_bar));

  if (bap == NULL)
    return NULL;

  Sprite *sp = create_sprite(xpm, x, y, n_xpms);

  bap->bar_sprite = sp;
  bap->final_angle = final_angle;
  bap->angular_speed = angular_speed;
  bap->game_button = bup;

  return bap;
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
 * @brief handles the movement the game button mechanics
 * @param bup pointer to game_button sprite "object"
 * @param background pointer to the background sprite
 * @param firemi pointer to firemi sprite character
 * @param waterix pointer to waternix sprite character
 * @return none
 */
void(handle_game_button)(Game_button *bup, Sprite *background, Sprite *firemi, Sprite *waternix) {
  int prev_button_x = bup->button_sprite->x;
  int prev_button_y = bup->button_sprite->y;

  //restoring the button background after it moves
  restore_background(prev_button_x, prev_button_y, bup->button_sprite->width, bup->button_sprite->height, background);

  uint16_t rect_y = 0, rect_x = 0;
  bool south_pressed = false;

  if (bup->orientation_of_button == NORTH) {
    rect_x = bup->button_sprite->x;
    rect_y = bup->button_sprite->y - 1;
  }
  if (bup->orientation_of_button == SOUTH) {
    rect_x = bup->button_sprite->x;
    rect_y = bup->button_sprite->y + bup->button_sprite->height + 1;
  }
  if (bup->orientation_of_button == EAST) {
    rect_x = bup->button_sprite->x + bup->button_sprite->width + 1;
    rect_y = bup->button_sprite->y;
  }
  if (bup->orientation_of_button == WEST) {
    rect_x = bup->button_sprite->x - 1;
    rect_y = bup->button_sprite->y;
  }

  //TODO: optimize this code to only run this when one os the characters is close
  //TODO: all workin besides south
  //checking collision of characters with imaginary rectangle on top of button, according to button orientation
  bup->pressed = collision_one_rect(firemi, rect_x, rect_y, bup->button_sprite->width, bup->button_sprite->height) || collision_one_rect(waternix, rect_x, rect_y, bup->button_sprite->width, bup->button_sprite->height);

  if (bup->pressed) {
    if (bup->orientation_of_button == NORTH) {
      if (bup->button_sprite->y != bup->finaly)
        bup->button_sprite->y += 1;
    }
    if (bup->orientation_of_button == SOUTH) {
      if (south_pressed)
        south_pressed = false;
      else
        south_pressed = true;
    }
    if (bup->orientation_of_button == EAST) {
      if (bup->button_sprite->x != bup->finalx)
        bup->button_sprite->x -= 1;
    }
    if (bup->orientation_of_button == WEST) {
      if (bup->button_sprite->x != bup->finalx)
        bup->button_sprite->x += 1;
    }
  }
  else {
    if (bup->orientation_of_button != SOUTH) {
      bup->button_sprite->x = bup->initx;
      bup->button_sprite->y = bup->inity;
    }
  }

  if (bup->orientation_of_button == SOUTH) {
    if (south_pressed) {
      if (bup->button_sprite->y != bup->finaly) {
        bup->button_sprite->y -= 1;
      }
    }
    else {
      bup->button_sprite->x = bup->initx;
      bup->button_sprite->y = bup->inity;
    }
  }

  draw_sprite(bup->button_sprite);
}

/**
 * @brief  
 * 
 */
void(handle_game_bar)(Game_bar *bap, Sprite *background) {
  //int prev_bar_x = bap->bar_sprite->x;
  //int prev_bar_y = bap->bar_sprite->y;
  uint16_t angle = 0;

  //restoring the button background after it moves
  //restore_background(prev_bar_x, prev_bar_y, bap->bar_sprite->width, bap->bar_sprite->height, background);

  //bar moves if it's respective button is pressed
  if(bap->game_button->pressed){
    if(angle != bap->final_angle){
      if(bap->final_angle > 0){
      angle += bap->angular_speed;
      }
      else{
         angle -= bap->angular_speed;
      }
    }
  }
  else{
    angle = 0;
  }

  draw_sprite_at_angle(bap->bar_sprite, angle);
}
