#include "game_engine.h"

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
