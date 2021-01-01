
#include "game_engine.h"
#include "game_handler.h"
#include "sprite.h"

#include "xpm_main_menu.h"
#include "xpm_firemi.h"
#include "xpm_game_elements.h"
#include "xpm_levels.h"
#include "xpm_slider.h"
#include "xpm_waternix.h"

//creating the clock element
Clock *game_clock = NULL;

//creating the cursor
Cursor *cursor;

Sprite *level;

//creating menu buttons
Sprite *play_button;
Sprite *play_letters;
Sprite *rules_button;
Sprite *rules_letters;

//creating main character sprites
Sprite *firemi;
Sprite *waternix;

//creating game board sprite elements for levels
Sprite *slider;
Sprite *box1;
Sprite *box2;
Game_button *game_button1;
Game_button *buttons1[1];
Game_bar *game_bar1;
Game_button *game_button2;
Game_button *buttons2[1];
Game_bar *game_bar2;
Game_button *game_button3;
Game_button *buttons3[1];
Game_bar *game_bar3;
Sprite *level1_completed;
Sprite *objs_to_collide[4];

//to handle the change of xpm to animate sprite when running
int n_maps_f = 0, n_maps_w = 0;
int n_map_2_f = 0, n_map_2_w = 0;

void (create_level)(enum game_state state) {
  cursor = create_cursor(400, 300);

  xpm_map_t xpm_firemi_array[11] = {idle1_firemi_xpm, idle2_firemi_xpm, idle3_firemi_xpm, l1_firemi_xpm, l2_firemi_xpm, l3_firemi_xpm, r1_firemi_xpm, r2_firemi_xpm, r3_firemi_xpm, push_left_firemi_xpm, push_right_firemi_xpm};

  xpm_map_t xpm_waternix_array[11] = {idle1_waternix_xpm, idle2_waternix_xpm, idle3_waternix_xpm, l1_waternix_xpm, l2_waternix_xpm, l3_waternix_xpm, r1_waternix_xpm, r2_waternix_xpm, r3_waternix_xpm, push_left_waternix_xpm, push_right_waternix_xpm};

  if (state == MAIN_MENU) {
    xpm_map_t xpm_menu_array[1] = {xpm_main_menu};
    xpm_map_t xpm_play_button[2] = {xpm_menu_button, xpm_menu_button_blue};
    xpm_map_t xpm_rules_button[2] = {xpm_menu_button, xpm_menu_button_red};
    xpm_map_t xpm_play_letters[1] = {xpm_play_button_letters};
    xpm_map_t xpm_rules_letters[1] = {xpm_rules_button_letters};
    level = create_sprite(xpm_menu_array, 0, 0, 1);
    play_button = create_sprite(xpm_play_button, 40, 400, 2);
    rules_button = create_sprite(xpm_rules_button, 40, 460, 2);
    play_letters = create_sprite(xpm_play_letters, 40, 400, 1);
    rules_letters = create_sprite(xpm_rules_letters, 40, 460, 1);
  }
  if (state == LEVEL_1) {
    xpm_map_t xpm_slider_array[1] = {xpm_slider};
    xpm_map_t xpm_box1_array[1] = {xpm_box76x76};
    xpm_map_t xpm_box2_array[1] = {xpm_box50x50};
    xpm_map_t xpm_level1_array[1] = {xpm_level1_with_slope_red_lava};
    game_clock = create_clock(652, 20);
    level = create_sprite(xpm_level1_array, 0, 0, 1);
    firemi = create_sprite(xpm_firemi_array, 20, 510, 11);
    waternix = create_sprite(xpm_waternix_array, 50, 510, 11);
    slider = create_sprite(xpm_slider_array, 317, 90, 1);
    box1 = create_sprite(xpm_box1_array, 390, 509, 1);
    box2 = create_sprite(xpm_box2_array, 563, 354, 1);
    game_button1 = create_game_button(xpm_button_south, 66, 376, SOUTH);
    buttons1[0] = game_button1;
    game_bar1 = create_game_bar(xpm_bar_v_76h, 555, 510, 0, 0, 0, -90, -1, buttons1, 1);
    game_button2 = create_game_button(xpm_button_north, 705, 570, NORTH);
    buttons2[0] = game_button2;
    game_bar2 = create_game_bar(xpm_bar_h91, 195, 270, 104, 270, 0, 0, 0, buttons2, 1);
    game_button3 = create_game_button(xpm_button_north, 720, 255, NORTH);
    buttons3[0] = game_button3;
    game_bar3 = create_game_bar(xpm_bar_h91, 645, 90, 554, 90, 0, 0, 0, buttons3, 1);

    //title elements
    xpm_map_t level1_completed_title_xpm_array[1] = {level1_completed_title};
    level1_completed = create_sprite(level1_completed_title_xpm_array, 0, 0, 1);

    objs_to_collide[0] = firemi;
    objs_to_collide[1] = waternix;
    objs_to_collide[2] = box1;
    objs_to_collide[3] = box2;
  }
}

void (draw_level)(enum game_state state) {
  if (state == MAIN_MENU) {
    draw_sprite(level);
    draw_sprite(play_button);
    draw_sprite(rules_button);
    draw_sprite(play_letters);
    draw_sprite(rules_letters);
    draw_cursor(cursor, level);
  }
  if (state == LEVEL_1) {
    //drawing level1 elements
    draw_sprite(level);
    draw_clock(game_clock);
    draw_sprite(firemi);
    draw_sprite(waternix);
    draw_sprite(box1);
    draw_sprite(box2);
    draw_sprite(slider);
    draw_sprite(game_button1->button_sprite);
    draw_sprite(game_bar1->bar_sprite);
    draw_sprite(game_button2->button_sprite);
    draw_sprite(game_bar2->bar_sprite);
    draw_sprite(game_button3->button_sprite);
    draw_sprite(game_bar3->bar_sprite);
  }
}

void (handle_level)(enum game_state * state, bool keys_firemi[4], bool keys_waternix[4]) {
  bool game_over = false;
  if (*state == MAIN_MENU) {
    if (cursor->x >= play_button->x && cursor->x <= play_button->x + play_button->width && cursor->y >= play_button->y && cursor->y <= play_button->y + play_button->height)
      play_button->map = play_button->xpms[1];
      
    else
      play_button->map = play_button->xpms[0];
    
    if (cursor->x >= rules_button->x && cursor->x <= rules_button->x + rules_button->width && cursor->y >= rules_button->y && cursor->y <= rules_button->y + rules_button->height)
      rules_button->map = rules_button->xpms[1];
    else
      rules_button->map = rules_button->xpms[0];
    
    draw_sprite(play_button);
    draw_sprite(rules_button);
    draw_sprite(play_letters);
    draw_sprite(rules_letters);
    draw_cursor(cursor, level);
  }
  if (*state == LEVEL_1) {
    //level1 handlers
    handle_game_button(game_button1, level, 4, objs_to_collide);
    handle_game_bar(game_bar1, level);
    handle_game_button(game_button2, level, 4, objs_to_collide);
    handle_game_bar(game_bar2, level);
    handle_game_button(game_button3, level, 4, objs_to_collide);
    handle_game_bar(game_bar3, level);
    handle_slider_move(slider, level);
    handle_game_box(firemi, waternix, box1, level);
    handle_game_box(firemi, waternix, box2, level);
    draw_cursor(cursor, level);
    handle_win(firemi, waternix, level1_completed);
  }
  if (*state != MAIN_MENU) {
    handle_characters_move(firemi, waternix, level, keys_firemi, keys_waternix, &game_over, &n_maps_f, &n_maps_w, &n_map_2_f, &n_map_2_w);
    if (game_over) {
      handle_lost();
      delete_level(*state);
      *state = MAIN_MENU;
      create_level(*state);
      draw_level(*state);
    }
  }
}

void (tick_game_clock)(){
  if (game_clock != NULL) tick_clock(game_clock, level);
}

void (update_game_cursor)(enum game_state * state, struct packet packet) {
  update_cursor(cursor, packet);
  if (*state == MAIN_MENU && packet.lb){
    if (cursor->x >= play_button->x && cursor->x <= play_button->x + play_button->width && cursor->y >= play_button->y && cursor->y <= play_button->y + play_button->height){
      delete_level(*state);
      *state = LEVEL_1;
      create_level(*state);
      draw_level(*state);
    }
  }
}

void (delete_level)(enum game_state state) {
  delete_sprite(level);
  if (state == MAIN_MENU) {
    delete_cursor(cursor);
    delete_sprite(play_button);
    delete_sprite(rules_button);
    delete_sprite(play_letters);
    delete_sprite(rules_letters);
  }
  if (state == LEVEL_1) {
    delete_sprite(firemi);
    delete_sprite(waternix);
    delete_sprite(box1);
    delete_sprite(box2);

    delete_clock(game_clock);
    delete_cursor(cursor);

    delete_game_button(game_button1);
    delete_game_bar(game_bar1);
    delete_game_button(game_button2);
    delete_game_bar(game_bar2);
    delete_game_button(game_button3);
    delete_game_bar(game_bar3);

    delete_sprite(level1_completed);
  }
}
