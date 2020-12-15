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
