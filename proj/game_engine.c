#include "game_engine.h"
#include "game_handler.h"
#include "sprite.h"

#include "xpm_firemi.h"
#include "xpm_game_elements.h"
#include "xpm_levels.h"
#include "xpm_menus.h"
#include "xpm_waternix.h"

//creating the clock element
static Clock *game_clock = NULL;

//creating the cursor
static Cursor *cursor;

static Sprite *level;
static Sprite *level_collisions;

//creating menu buttons
static Sprite *play_button;
static Sprite *play_letters;
static Sprite *rules_button;
static Sprite *rules_letters;
static Sprite *exit_button;

//creating main character sprites
static Sprite *firemi;
static Sprite *waternix;

//creating game board sprite elements for levels
static Sprite *wind;
static Sprite *wind2;
static Sprite *slider;
static Sprite *box1;
static Sprite *box2;
static Game_button *game_button1;
static Game_button *buttons1[1];
static Game_bar *game_bar1;
static Game_button *game_button2;
static Game_button *buttons2[2];
static Game_bar *game_bar2;
static Game_button *game_button3;
static Game_button *buttons3[3];
static Game_bar *game_bar3;
static Game_button *game_button5;
static Game_button *game_button4;
static Game_lever *lever;
static Sprite *level_completed;
static Sprite *objs_to_collide[4];
static Game_bar *game_bar4;
static Game_bar *game_bar5;
static Game_bar *game_bar6;
static Game_button *game_button6;
static Game_button *game_button7;

static Game_button *buttons4[2];
static Game_button *buttons5[1];
static Game_button *buttons6[1];

static Sprite *red_lava;
static Sprite *red_lava2;
static Sprite *blue_lava;
static Sprite *blue_lava2;
static Sprite *purple_lava;
static Sprite *purple_lava2;

static uint8_t *numbers_map = NULL;
static xpm_image_t img;

static bool red_lava_change = false;
static bool red_lava2_change = false;
static bool lava_change_blue = false;
static bool lava_change_blue2 = false;
static bool lava_change_purple = false;

static struct packet prev_mouse_packet;
static rtc_time prev_time;

//to handle the change of xpm to animate sprite when running
static int n_maps_f = 0, n_maps_w = 0;
static int n_map_2_f = 0, n_map_2_w = 0;
static int map_wind = 0, map_wind2 = 0;
static int wind_speed = 0, wind_speed2 = 0;
static int wind_speed_2 = 0, wind_speed2_2 = 0;

void(create_level)(enum game_state state) {
  cursor = create_cursor(400, 300);

  if (numbers_map == NULL) {
    numbers_map = xpm_load(xpm_numbers, XPM_8_8_8, &img);
  }

  xpm_map_t xpm_firemi_array[11] = {idle1_firemi_xpm, idle2_firemi_xpm, idle3_firemi_xpm, l1_firemi_xpm, l2_firemi_xpm, l3_firemi_xpm, r1_firemi_xpm, r2_firemi_xpm, r3_firemi_xpm, push_left_firemi_xpm, push_right_firemi_xpm};

  xpm_map_t xpm_waternix_array[11] = {idle1_waternix_xpm, idle2_waternix_xpm, idle3_waternix_xpm, l1_waternix_xpm, l2_waternix_xpm, l3_waternix_xpm, r1_waternix_xpm, r2_waternix_xpm, r3_waternix_xpm, push_left_waternix_xpm, push_right_waternix_xpm};

  xpm_map_t xpm_red_lava[1] = {lava_red};
  xpm_map_t xpm_purple_lava[1] = {lava_purple};
  xpm_map_t xpm_blue_lava[1] = {lava_blue};

  if (state == MAIN_MENU) {
    xpm_map_t xpm_menu_array[1] = {xpm_main_menu};
    xpm_map_t xpm_play_button[2] = {xpm_menu_button, xpm_menu_button_blue};
    xpm_map_t xpm_rules_button[2] = {xpm_menu_button, xpm_menu_button_red};
    xpm_map_t xpm_play_letters[1] = {xpm_play_button_letters};
    xpm_map_t xpm_rules_letters[1] = {xpm_rules_button_letters};
    level = create_sprite(xpm_menu_array, 0, 0, 1);
    play_button = create_sprite(xpm_play_button, 25, 451, 2);
    rules_button = create_sprite(xpm_rules_button, 25, 502, 2);
    play_letters = create_sprite(xpm_play_letters, 25, 451, 1);
    rules_letters = create_sprite(xpm_rules_letters, 25, 502, 1);
  }
  else if (state == RULES_MENU) {
    xpm_map_t xpm_menu_array[1] = {xpm_rules_menu};
    xpm_map_t xpm_exit_buttons[2] = {xpm_exit_button, xpm_exit_button_colored};
    level = create_sprite(xpm_menu_array, 0, 0, 1);
    exit_button = create_sprite(xpm_exit_buttons, 747, 17, 2);
  }
  else if (state == LEVEL_1) {
    xpm_map_t xpm_slider_array[1] = {xpm_slider};
    xpm_map_t xpm_box1_array[1] = {xpm_box1};
    xpm_map_t xpm_box2_array[1] = {xpm_box2};
    xpm_map_t xpm_level1_array[1] = {xpm_level1};
    xpm_map_t xpm_level1_collisions_array[1] = {xpm_level1_collisions};
    game_clock = create_clock(652, 20);
    level = create_sprite(xpm_level1_array, 0, 0, 1);
    level_collisions = create_sprite(xpm_level1_collisions_array, 0, 0, 1);
    firemi = create_sprite(xpm_firemi_array, 20, 510, 11);
    waternix = create_sprite(xpm_waternix_array, 50, 510, 11);
    slider = create_sprite(xpm_slider_array, 317, 90, 1);
    box1 = create_sprite(xpm_box1_array, 390, 509, 1);
    box2 = create_sprite(xpm_box2_array, 563, 354, 1);
    game_button1 = create_game_button(xpm_button_1, 66, 376, SOUTH);
    game_button4 = create_game_button(xpm_button_2, 22, 165, NORTH);
    game_button5 = create_game_button(xpm_button_3, 246, 75, NORTH);
    buttons1[0] = game_button1;
    game_bar1 = create_game_bar(xpm_bar_1, 555, 510, 0, 0, 0, -90, -1, buttons1, 1);
    game_button2 = create_game_button(xpm_button_2, 705, 570, NORTH);
    buttons2[0] = game_button2;
    buttons2[1] = game_button4;
    game_bar2 = create_game_bar(xpm_bar_2, 195, 270, 104, 270, 0, 0, 0, buttons2, 2);
    game_button3 = create_game_button(xpm_button_3, 720, 255, NORTH);
    buttons3[0] = game_button3;
    buttons3[1] = game_button5;
    game_bar3 = create_game_bar(xpm_bar_3, 645, 90, 554, 90, 0, 0, 0, buttons3, 2);

    red_lava = create_sprite(xpm_red_lava, 255, 584, 1);
    red_lava2 = create_sprite(xpm_red_lava, 391, 405, 1);
    blue_lava = create_sprite(xpm_blue_lava, 585, 584, 1);
    purple_lava = create_sprite(xpm_purple_lava, 301, 270, 1);

    //title elements
    xpm_map_t level1_completed_title_xpm_array[1] = {level1_completed_title};
    level_completed = create_sprite(level1_completed_title_xpm_array, 0, 0, 1);

    objs_to_collide[0] = firemi;
    objs_to_collide[1] = waternix;
    objs_to_collide[2] = box1;
    objs_to_collide[3] = box2;
  }
  if (state == LEVEL_2) {
    xpm_map_t xpm_wind[6] = {xpm_wind1, xpm_wind2, xpm_wind3, xpm_wind4, xpm_wind5, xpm_wind6};

    wind = create_sprite(xpm_wind, 15, 305, 6); 
    wind2 = create_sprite(xpm_wind, 732, 305, 6); 

    game_clock = create_clock(652, 20);
    firemi = create_sprite(xpm_firemi_array, 32, 535, 11);
    waternix = create_sprite(xpm_waternix_array, 721, 535, 11);

    objs_to_collide[0] = firemi;
    objs_to_collide[1] = waternix;

    xpm_map_t xpm_level2_array[1] = {xpm_level2};
    xpm_map_t xpm_level2_collisions_array[1] = {xpm_level2_collisions};

    xpm_map_t level2_completed_title_xpm_array[1] = {level2_completed_title};
    level_completed = create_sprite(level2_completed_title_xpm_array, 0, 0, 1);

    level = create_sprite(xpm_level2_array, 0, 0, 1);
    level_collisions = create_sprite(xpm_level2_collisions_array, 0, 0, 1);

    red_lava = create_sprite(xpm_red_lava, 120, 586, 1);
    red_lava2 = create_sprite(xpm_red_lava, 120, 495, 1);
    blue_lava = create_sprite(xpm_blue_lava, 577, 586, 1);
    blue_lava2 = create_sprite(xpm_blue_lava, 615, 495, 1);
    purple_lava = create_sprite(xpm_purple_lava, 343, 282, 1);

    game_button1 = create_game_button(xpm_button1_level2, 15, 450, EAST);
    game_button2 = create_game_button(xpm_button2_level2, 771, 450, WEST);
    game_button3 = create_game_button(xpm_button_1, 225, 328, SOUTH);
    game_button4 = create_game_button(xpm_button3_level2, 540, 328, SOUTH);
    game_button5 = create_game_button(xpm_button4_level2, 106, 120, SOUTH);
    game_button6 = create_game_button(xpm_button5_level2, 660, 120, SOUTH);

    buttons1[0] = game_button1;
    buttons2[0] = game_button2;
    buttons3[0] = game_button3;
    buttons4[0] = game_button4;
    buttons5[0] = game_button5;
    buttons6[0] = game_button6;

    game_bar1 = create_game_bar(xpm_bar1_level2, 196, 328, 0, 0, 0, -90, -1, buttons4, 1);
    game_bar2 = create_game_bar(xpm_bar2_level2, 285, 328, 0, 0, 0, 90, 1, buttons2, 1);
    game_bar3 = create_game_bar(xpm_bar3_level2, 511, 328, 0, 0, 0, -90, -1, buttons1, 1);
    game_bar4 = create_game_bar(xpm_bar_1, 601, 328, 0, 0, 0, 90, 1, buttons3, 1);
    game_bar5 = create_game_bar(xpm_bar4_level2, 224, 210, 224, 90, 0, 0, 0, buttons6, 1);
    game_bar6 = create_game_bar(xpm_bar5_level2, 495, 210, 495, 90, 0, 0, 0, buttons5, 1);

    lever = create_game_lever(xpm_lever, xpm_lever_red, xpm_lever_base_green, 563, 282);
  }
  if (state == LEVEL_3) {
    xpm_map_t xpm_wind[6] = {xpm_wind1, xpm_wind2, xpm_wind3, xpm_wind4, xpm_wind5, xpm_wind6};

    wind = create_sprite(xpm_wind, 212, 394, 6); 
    wind2 = create_sprite(xpm_wind, 549, 394, 6); 

    game_clock = create_clock(652, 20);
    xpm_map_t xpm_level3_array[1] = {xpm_level3};
    xpm_map_t xpm_level3_collisions_array[1] = {xpm_level3_collisions};

    xpm_map_t level3_completed_title_xpm_array[1] = {level3_completed_title};
    level_completed = create_sprite(level3_completed_title_xpm_array, 0, 0, 1);

    level = create_sprite(xpm_level3_array, 0, 0, 1);
    level_collisions = create_sprite(xpm_level3_collisions_array, 0, 0, 1);

    waternix = create_sprite(xpm_waternix_array, 23, 443, 11);
    firemi = create_sprite(xpm_firemi_array, 734, 443, 11);

    objs_to_collide[0] = firemi;
    objs_to_collide[1] = waternix;

    game_button1 = create_game_button(xpm_button1_level3, 15, 90, EAST);
    game_button2 = create_game_button(xpm_button2_level3, 771, 88, WEST);
    game_button3 = create_game_button(xpm_button3_level3, 240, 180, NORTH);
    game_button4 = create_game_button(xpm_button4_level3, 540, 180, NORTH);
    game_button5 = create_game_button(xpm_button5_level3, 330, 286, WEST);
    game_button6 = create_game_button(xpm_button6_level3, 480, 284, EAST);
    game_button7 = create_game_button(xpm_button7_level3, 225, 570, NORTH);

    buttons1[0] = game_button1;
    buttons2[0] = game_button2;
    buttons3[0] = game_button3;
    buttons3[1] = game_button5;
    buttons3[2] = game_button6;
    buttons4[0] = game_button4;
    buttons4[1] = game_button7;

    game_bar1 = create_game_bar(xpm_bar1_level3, 194, 15, 0, 0, 0, 90, 1, buttons2, 1);
    game_bar2 = create_game_bar(xpm_bar2_level3, 600, 15, 0, 0, 0, -90, -1, buttons1, 1);
    game_bar3 = create_game_bar(xpm_bar3_level3, 368, 195, 283, 195, 0, 0, 0, buttons3, 3);
    game_bar4 = create_game_bar(xpm_bar4_level3, 368, 330, 368, 496, 0, 0, 0, buttons4, 2);

    red_lava = create_sprite(xpm_red_lava, 51, 589, 1);
    red_lava2 = create_sprite(xpm_red_lava, 255, 317, 1);
    blue_lava = create_sprite(xpm_blue_lava, 640, 588, 1);
    blue_lava2 = create_sprite(xpm_blue_lava, 481, 317, 1);
  }
  if (state == LEVEL_4) {
    xpm_map_t xpm_wind[6] = {xpm_wind1, xpm_wind2, xpm_wind3, xpm_wind4, xpm_wind5, xpm_wind6};

    wind = create_sprite(xpm_wind, 46, 486, 6); 

    game_clock = create_clock(652, 20);
    xpm_map_t xpm_level4_array[1] = {xpm_level4};
    xpm_map_t xpm_level4_collisions_array[1] = {xpm_level4_collisions};
    xpm_map_t xpm_box1_array[1] = {xpm_box1};
    xpm_map_t xpm_box2_array[1] = {xpm_box2};

    xpm_map_t level4_completed_title_xpm_array[1] = {level4_completed_title};
    level_completed = create_sprite(level4_completed_title_xpm_array, 0, 0, 1);

    level = create_sprite(xpm_level4_array, 0, 0, 1);
    level_collisions = create_sprite(xpm_level4_collisions_array, 0, 0, 1);

    firemi = create_sprite(xpm_firemi_array, 640, 430, 11);
    waternix = create_sprite(xpm_waternix_array, 734, 54, 11);
    box1 = create_sprite(xpm_box1_array, 600, 29, 1);
    box2 = create_sprite(xpm_box2_array, 250, 278, 1);

    objs_to_collide[0] = firemi;
    objs_to_collide[1] = waternix;
    objs_to_collide[2] = box1;
    objs_to_collide[3] = box2;

    game_button1 = create_game_button(xpm_button1_level4, 450, 75, SOUTH);
    game_button2 = create_game_button(xpm_button2_level4, 705, 480, NORTH);
    game_button3 = create_game_button(xpm_button3_level4, 705, 510, SOUTH);

    buttons1[0] = game_button1;
    buttons2[0] = game_button2;
    buttons3[0] = game_button3;

    purple_lava = create_sprite(xpm_purple_lava, 466, 587, 1);

    game_bar1 = create_game_bar(xpm_bar1_level4, 435, 180, 0, 0, 0, -90, -1, buttons1, 1);
    game_bar2 = create_game_bar(xpm_bar2_level4, 600, 272, 509, 272, 0, 0, 0, buttons2, 1);
    game_bar3 = create_game_bar(xpm_bar3_level4, 165, 15, 0, 0, 0, 90, 1, buttons3, 1);
    
    purple_lava2 = create_sprite(xpm_purple_lava, 300, 586, 1);
  }
  if(state == PAUSE){
    xpm_map_t xpm_pause_array[1] = {pause_xpm};

    level = create_sprite(xpm_pause_array, 0, 0, 1);

    xpm_map_t xpm_play_button[2] = {pause_button1_xpm, pause_button2_xpm};

    play_button = create_sprite(xpm_play_button, 300, 481, 2);
  }
}

void(draw_level)(enum game_state state) {
  if (state == MAIN_MENU) {
    draw_sprite(level);
    draw_sprite(play_button);
    draw_sprite(rules_button);
    draw_sprite(play_letters);
    draw_sprite(rules_letters);
    draw_date(prev_time, 550, 470, numbers_map, img);
    draw_cursor(cursor, level);
  }
  else if (state == RULES_MENU) {
    draw_sprite(level);
    draw_sprite(exit_button);
    draw_cursor(cursor, level);
  }
  else if (state == LEVEL_1) {
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
    draw_sprite(game_button4->button_sprite);
    draw_sprite(game_button5->button_sprite);

    draw_sprite(red_lava);
    draw_sprite(red_lava2);
    draw_sprite(blue_lava);
    draw_sprite(purple_lava);
  }
  if (state == LEVEL_2) {
    draw_sprite(level);
    draw_clock(game_clock);
    draw_sprite(firemi);
    draw_sprite(waternix);

    draw_sprite(red_lava);
    draw_sprite(red_lava2);
    draw_sprite(blue_lava);
    draw_sprite(blue_lava2);

    draw_sprite(game_button1->button_sprite);
    draw_sprite(game_button2->button_sprite);
    draw_sprite(game_button3->button_sprite);
    draw_sprite(game_button4->button_sprite);
    draw_sprite(game_button5->button_sprite);
    draw_sprite(game_button6->button_sprite);

    draw_sprite(game_bar1->bar_sprite);
    draw_sprite(game_bar2->bar_sprite);
    draw_sprite(game_bar3->bar_sprite);
    draw_sprite(game_bar4->bar_sprite);
    draw_sprite(game_bar5->bar_sprite);
    draw_sprite(game_bar6->bar_sprite);

    draw_sprite(lever->lever_sprite);
    draw_sprite(lever->lever_base_sprite);
  }
  if (state == LEVEL_3) {
    draw_sprite(level);
    draw_clock(game_clock);
    draw_sprite(firemi);
    draw_sprite(waternix);

    draw_sprite(red_lava);
    draw_sprite(red_lava2);
    draw_sprite(blue_lava);
    draw_sprite(blue_lava2);

    draw_sprite(game_button1->button_sprite);
    draw_sprite(game_button2->button_sprite);
    draw_sprite(game_button3->button_sprite);
    draw_sprite(game_button4->button_sprite);
    draw_sprite(game_button5->button_sprite);
    draw_sprite(game_button6->button_sprite);
    draw_sprite(game_button7->button_sprite);

    draw_sprite(game_bar1->bar_sprite);
    draw_sprite(game_bar2->bar_sprite);
    draw_sprite(game_bar3->bar_sprite);
    draw_sprite(game_bar4->bar_sprite);
  }
  if (state == LEVEL_4) {
    draw_sprite(level);
    draw_clock(game_clock);
    draw_sprite(firemi);
    draw_sprite(waternix);
    draw_sprite(box1);
    draw_sprite(box2);

    draw_sprite(purple_lava);
    draw_sprite(purple_lava2);

    draw_sprite(game_button1->button_sprite);
    draw_sprite(game_button2->button_sprite);
    draw_sprite(game_button3->button_sprite);

    draw_sprite(game_bar1->bar_sprite);
    draw_sprite(game_bar2->bar_sprite);
    draw_sprite(game_bar3->bar_sprite);
  }
  if(state == PAUSE){
    draw_sprite(level);
    draw_sprite(play_button);
  }
  copy_buffer_to_vram();
  //switch_display_start();
}

void(handle_level)(enum game_state *state,  enum game_state * prev_state, bool keys_firemi[4], bool keys_waternix[4]) {
  copy_to_buffer(level->map);
  bool game_over = false;
  bool win = false;
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
    if (prev_time.year != 0) //for when it hasn't received an rtc packet yet
      draw_date(prev_time, 550, 470, numbers_map, img);
    draw_cursor(cursor, level);
  }
  else if (*state == RULES_MENU) {
    if (cursor->x >= exit_button->x && cursor->x <= exit_button->x + exit_button->width && cursor->y >= exit_button->y && cursor->y <= exit_button->y + exit_button->height)
      exit_button->map = exit_button->xpms[1];
    else
      exit_button->map = exit_button->xpms[0];

    draw_sprite(exit_button);
    draw_cursor(cursor, level);
  }
  else if (*state == LEVEL_1) {
    //level1 handlers
    handle_lava(red_lava, level, 255, &red_lava_change, 120);
    handle_lava(red_lava2, level, 391, &red_lava2_change, 105);
    handle_lava(blue_lava, level, 585, &lava_change_blue, 90);
    handle_lava(purple_lava, level, 301, &lava_change_purple, 90);

    handle_game_button(game_button1, level, 4, objs_to_collide);
    handle_game_bar(game_bar1, level, objs_to_collide, 4);
    handle_game_button(game_button2, level, 4, objs_to_collide);
    handle_game_bar(game_bar2, level, objs_to_collide, 4);
    handle_game_button(game_button3, level, 4, objs_to_collide);
    handle_game_bar(game_bar3, level, objs_to_collide, 4);

    handle_slider_move(slider, level, level_collisions);
    handle_game_box(firemi, waternix, box1, level, level_collisions);
    handle_game_box(firemi, waternix, box2, level, level_collisions);
    handle_game_button(game_button4, level, 4, objs_to_collide);
    handle_game_button(game_button5, level, 4, objs_to_collide);

    draw_clock(game_clock);
    win = handle_win(firemi, waternix, level_completed, 105, 30, 30, 30, 45, 65);
    if (win) {
      delete_level(*state);
      *state = LEVEL_2;
      create_level(*state);
      draw_level(*state);
    }
  }
  if (*state == LEVEL_2) {
    handle_wind(wind, 408, 60, firemi, waternix, &map_wind, &wind_speed, &wind_speed2, level_collisions);
    handle_wind(wind2, 408, 60, firemi, waternix, &map_wind2, &wind_speed_2, &wind_speed2_2, level_collisions);

    handle_lava(red_lava, level, 120, &red_lava_change, 106);
    handle_lava(red_lava2, level, 120, &red_lava2_change, 90);
    handle_lava(blue_lava, level, 577, &lava_change_blue, 120);
    handle_lava(blue_lava2, level, 615, &lava_change_blue2, 90);
    handle_lava(purple_lava, level, 343, &lava_change_purple, 120);
    handle_game_lever(lever, prev_mouse_packet, cursor, firemi, waternix);

    handle_game_button(game_button1, level, 2, objs_to_collide);
    handle_game_button(game_button2, level, 2, objs_to_collide);
    handle_game_button(game_button3, level, 2, objs_to_collide);
    handle_game_button(game_button4, level, 2, objs_to_collide);
    handle_game_button(game_button5, level, 2, objs_to_collide);
    handle_game_button(game_button6, level, 2, objs_to_collide);

    game_button2->pressed = game_button2->pressed || lever->active;

    handle_game_bar(game_bar1, level, objs_to_collide, 2);
    handle_game_bar(game_bar2, level, objs_to_collide, 2);
    handle_game_bar(game_bar3, level, objs_to_collide, 2);
    handle_game_bar(game_bar4, level, objs_to_collide, 2);
    handle_game_bar(game_bar5, level, objs_to_collide, 2);
    handle_game_bar(game_bar6, level, objs_to_collide, 2);
    draw_clock(game_clock);

    win = handle_win(firemi, waternix, level_completed, 424, 138, 350, 138, 45, 65);
  }
  if (*state == LEVEL_3) {
    handle_wind(wind, 496, 300, firemi, waternix, &map_wind, &wind_speed, &wind_speed2, level_collisions);
    handle_wind(wind2, 496, 300, firemi, waternix, &map_wind2, &wind_speed_2, &wind_speed2_2, level_collisions);

    handle_lava(red_lava, level, 51, &red_lava_change, 120);
    handle_lava(red_lava2, level, 255, &red_lava2_change, 90);
    handle_lava(blue_lava, level, 640, &lava_change_blue, 120);
    handle_lava(blue_lava2, level, 481, &lava_change_blue2, 75); 

    handle_game_button(game_button1, level, 2, objs_to_collide);
    handle_game_button(game_button2, level, 2, objs_to_collide);
    handle_game_button(game_button3, level, 2, objs_to_collide);
    handle_game_button(game_button4, level, 2, objs_to_collide);
    handle_game_button(game_button5, level, 2, objs_to_collide);
    handle_game_button(game_button6, level, 2, objs_to_collide);
    handle_game_button(game_button7, level, 2, objs_to_collide);

    handle_game_bar(game_bar1, level, objs_to_collide, 2);
    handle_game_bar(game_bar2, level, objs_to_collide, 2);
    handle_game_bar(game_bar3, level, objs_to_collide, 2);
    handle_game_bar(game_bar4, level, objs_to_collide, 2);

    draw_clock(game_clock);

    win = handle_win(firemi, waternix, level_completed, 433, 530, 359, 530, 45, 65);
  }
  if (*state == LEVEL_4) {
    handle_wind(wind, 586, 186, firemi, waternix, &map_wind, &wind_speed, &wind_speed2, level_collisions);

    handle_lava(purple_lava2, level, 300, &lava_change_blue, 90); //to recicle varaiables, used lava_change_blue here
    handle_lava(purple_lava, level, 466, &lava_change_purple, 80);

    handle_game_button(game_button1, level, 4, objs_to_collide);
    handle_game_button(game_button2, level, 4, objs_to_collide);
    handle_game_button(game_button3, level, 4, objs_to_collide);

    handle_game_bar(game_bar1, level, objs_to_collide, 4);
    handle_game_bar(game_bar2, level, objs_to_collide, 4);
    handle_game_bar(game_bar3, level, objs_to_collide, 4);

    handle_game_box(firemi, waternix, box1, level, level_collisions);
    handle_game_box(firemi, waternix, box2, level, level_collisions);

    draw_clock(game_clock);

    win = handle_win(firemi, waternix, level_completed, 275, 108, 201, 108, 45, 65);
  }

  if (*state != MAIN_MENU && *state != PAUSE && *state != RULES_MENU) {
    handle_characters_move(firemi, waternix, level, keys_firemi, keys_waternix, &game_over, &n_maps_f, &n_maps_w, &n_map_2_f, &n_map_2_w, level_collisions);
    draw_cursor(cursor, level);
    if (game_over) {
      handle_lost();
      delete_level(*state);
      *state = MAIN_MENU;
      create_level(*state);
      draw_level(*state);
    }
    if (win) {
      if((*state) == LEVEL_1){
        delete_level(*state);
        (*state) = LEVEL_2;
        create_level(*state);
        draw_level(*state);
      }
      else if((*state) == LEVEL_2){
        delete_level(*state);
        (*state) = LEVEL_3;
        create_level(*state);
        draw_level(*state);
      }
      else if((*state) == LEVEL_3){
        delete_level(*state);
        (*state) = LEVEL_4;
        create_level(*state);
        draw_level(*state);
      }
      else if((*state) == LEVEL_4){
        delete_level(*state);

        xpm_map_t xpm_final_title[1] = {final_menssage_title};

        Sprite* final_menssage = create_sprite(xpm_final_title, 0, 0, 1); 

        draw_sprite(final_menssage);

        sleep(2);

        *state = MAIN_MENU;

        create_level(*state);
        draw_level(*state);
      }
    }
  }

  if (*state == PAUSE) {
    if (cursor->x >= play_button->x && cursor->x <= play_button->x + play_button->width && cursor->y >= play_button->y && cursor->y <= play_button->y + play_button->height)
      play_button->map = play_button->xpms[1];
    else
      play_button->map = play_button->xpms[0];

    draw_sprite(play_button);
    draw_cursor(cursor, level);
  }
  copy_buffer_to_vram();
  //switch_display_start();
}

void(tick_game_clock)() {
  if (game_clock != NULL)
    tick_clock(game_clock, level);
}

void(update_game_cursor)(enum game_state *state, struct packet packet, enum game_state prev_state) {
  update_cursor(cursor, packet);
  prev_mouse_packet = packet;
  if (*state == MAIN_MENU && packet.lb) {
    if (cursor->x >= play_button->x && cursor->x <= play_button->x + play_button->width && cursor->y >= play_button->y && cursor->y <= play_button->y + play_button->height) {
      delete_level(*state);
      *state = LEVEL_1;
      create_level(*state);
      draw_level(*state);
    }
    if (cursor->x >= rules_button->x && cursor->x <= rules_button->x + rules_button->width && cursor->y >= rules_button->y && cursor->y <= rules_button->y + rules_button->height) {
      delete_level(*state);
      *state = RULES_MENU;
      create_level(*state);
      draw_level(*state);
    }
  }
  else if (*state == RULES_MENU && packet.lb) {
    if (cursor->x >= exit_button->x && cursor->x <= exit_button->x + exit_button->width && cursor->y >= exit_button->y && cursor->y <= exit_button->y + exit_button->height) {
      delete_level(*state);
      *state = MAIN_MENU;
      create_level(*state);
      draw_level(*state);
    }
  }
  else if(*state == PAUSE && packet.lb){
      if (cursor->x >= play_button->x && cursor->x <= play_button->x + play_button->width && cursor->y >= play_button->y && cursor->y <= play_button->y + play_button->height) {
      delete_level(*state);
      *state = prev_state;
      create_level(*state);
      draw_level(*state);
    }
  }
}

void update_game_time(enum game_state state, rtc_time time) {
  prev_time = time;
}

void (change_levels)(enum game_state * state, uint8_t change_level) {
  if (change_level != 0 && *state != PAUSE) {
    if (change_level == 1 && *state != LEVEL_1) {
      delete_level(*state);
      *state = LEVEL_1;
      create_level(*state);
      draw_level(*state);
    }
    if (change_level == 2 && *state != LEVEL_2) {
      delete_level(*state);
      *state = LEVEL_2;
      create_level(*state);
      draw_level(*state);
    }
    if (change_level == 3 && *state != LEVEL_3) {
      delete_level(*state);
      *state = LEVEL_3;
      create_level(*state);
      draw_level(*state);
    }
    if (change_level == 4 && *state != LEVEL_4) {
      delete_level(*state);
      *state = LEVEL_4;
      create_level(*state);
      draw_level(*state);
    }
  }
}

void(delete_level)(enum game_state state) {
  delete_sprite(level);
  if (state == MAIN_MENU) {
    delete_cursor(cursor);
    delete_sprite(play_button);
    delete_sprite(rules_button);
    delete_sprite(play_letters);
    delete_sprite(rules_letters);
  }
  if (state == RULES_MENU) {
    delete_cursor(cursor);
    delete_sprite(exit_button);
  }
  else if (state == LEVEL_1) {
    delete_sprite(level_collisions);
    delete_sprite(firemi);
    delete_sprite(waternix);
    delete_sprite(box1);
    delete_sprite(box2);
    delete_sprite(red_lava);
    delete_sprite(red_lava2);
    delete_sprite(blue_lava);
    delete_sprite(purple_lava);

    delete_clock(game_clock);
    delete_cursor(cursor);

    delete_game_button(game_button1);
    delete_game_bar(game_bar1);
    delete_game_button(game_button2);
    delete_game_bar(game_bar2);
    delete_game_button(game_button3);
    delete_game_bar(game_bar3);
    delete_game_button(game_button4);
    delete_game_button(game_button5);
    delete_sprite(slider);

    delete_sprite(level_completed);
  }
  if (state == LEVEL_2) {
    delete_sprite(level_collisions);
    delete_sprite(level_completed);

    delete_sprite(firemi);
    delete_sprite(waternix);

    delete_clock(game_clock);
    delete_cursor(cursor);

    delete_sprite(red_lava);
    delete_sprite(red_lava2);
    delete_sprite(blue_lava);
    delete_sprite(blue_lava2);

    delete_game_button(game_button1);
    delete_game_button(game_button2);
    delete_game_button(game_button3);
    delete_game_button(game_button4);
    delete_game_button(game_button5);
    delete_game_button(game_button6);

    delete_game_bar(game_bar1);
    delete_game_bar(game_bar2);
    delete_game_bar(game_bar3);
    delete_game_bar(game_bar4);
    delete_game_bar(game_bar5);
    delete_game_bar(game_bar6);

    delete_game_lever(lever);

    delete_sprite(wind);
    delete_sprite(wind2);
  }
  if (state == LEVEL_3) {
    delete_sprite(level_collisions);
    delete_sprite(level_completed);

    delete_sprite(firemi);
    delete_sprite(waternix);

    delete_clock(game_clock);
    delete_cursor(cursor);

    delete_sprite(red_lava);
    delete_sprite(red_lava2);
    delete_sprite(blue_lava);
    delete_sprite(blue_lava2);

    delete_game_button(game_button1);
    delete_game_button(game_button2);
    delete_game_button(game_button3);
    delete_game_button(game_button4);
    delete_game_button(game_button5);
    delete_game_button(game_button6);
    delete_game_button(game_button7);

    delete_game_bar(game_bar1);
    delete_game_bar(game_bar2);
    delete_game_bar(game_bar3);
    delete_game_bar(game_bar4);

    delete_sprite(wind);
    delete_sprite(wind2);
  }
  if (state == LEVEL_4) {
    delete_sprite(level_collisions);
    delete_sprite(level_completed);

    delete_clock(game_clock);
    delete_cursor(cursor);

    delete_sprite(firemi);
    delete_sprite(waternix);

    delete_sprite(box1);
    delete_sprite(box2);

    delete_sprite(purple_lava);
    delete_sprite(purple_lava2);

    delete_game_button(game_button1);
    delete_game_button(game_button2);
    delete_game_button(game_button3);

    delete_game_bar(game_bar1);
    delete_game_bar(game_bar2);
    delete_game_bar(game_bar3);

    delete_sprite(wind);
  }
}
