#include "game_engine.h"

static uint8_t * clock_map; //TODO: Should it be here? Make a clock module or struct?
static unsigned clock_height;
static unsigned clock_width = CLOCK_WIDTH;
static unsigned clock_count = 0;
static uint32_t clock_transparency_color;

/**
 * @brief handle a character's movement
 * 
 * @param firemi one of the characters to move
 * @param waternix the other character to move
 * @param background the game's background (to be restored)
 * @param char1_keys the pressed keys for the movement of the first character
 * @param char2_keys the pressed keys for the movement of the second character
 */
void (handle_characters_move)(Sprite * firemi, Sprite * waternix, Sprite *background, bool char1_keys[4], bool char2_keys[4], bool* game_over){
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

void (create_clock)(){
  xpm_image_t img;
  clock_map = xpm_load(xpm_numbers, XPM_8_8_8, &img);
  clock_height = img.height;
  clock_transparency_color = xpm_transparency_color(img.type);
  draw_clock();
}

void (draw_clock)(){
  int map_index = 0; //to keep track of map index

  uint32_t color;

  unsigned minutes = clock_count/60;
  unsigned seconds = clock_count%60;

  unsigned clock_digits[4] = {minutes/10, minutes%10, seconds/10, seconds%10};

  //draws pixmap
  for (int i = 0; i < 4; i++){
    for (unsigned row = 0; row < clock_height; row++) {
      for (unsigned col = 0; col < XPM_NUMBERS_WIDTH; col++) {
        map_index = row * 800 + XPM_NUMBERS_STEP * clock_digits[i] + col;
        color = convert_BGR_to_RGB(color_assembler(clock_map, &map_index));
        if (color != clock_transparency_color)
          draw_pixel(col + CLOCK_X, row + CLOCK_Y, color);
      }
    }
  }
}

void (tick_clock)(Sprite * background){
  clock_count++;
  restore_background(CLOCK_X, CLOCK_Y, clock_width, clock_height, background);
  draw_clock();
}

/**
 * @brief checks if characters are in lava or not
 * @param firemi pointer to firemi sprite object
 * @param waternix pointer to waternix sprite object
 * @return true if they are in lava, false otherwise
 */
bool (check_lava)(Sprite* firemi, Sprite* waternix){
  if(check_sprite_collision_by_color(firemi, LAVA_BLUE)) return true;
  if(check_sprite_collision_by_color(waternix, LAVA_RED)) return true;
  if(check_sprite_collision_by_color(firemi, LAVA_PURPLE)) return true;
  if(check_sprite_collision_by_color(waternix, LAVA_PURPLE)) return true;
  return false;
}
