//LCOM header files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <lcom/timer.h>

//c header files
#include <stdbool.h>
#include <stdint.h>

//project header files - modulos
#include "game_engine.h"
#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"
#include "video_gr.h"

//project header files - xpm's
#include "xpm_firemi.h"
#include "xpm_game_elements.h"
#include "xpm_levels.h"
#include "xpm_slider.h"
#include "xpm_waternix.h"
//#include "xpm_titles.h"

//global variables
unsigned timer_counter = 0;
extern uint8_t bytes[2];
extern bool making_scancode;
bool exit_game = false;
bool game_over = false;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

//xpm array encapsulation for sprite creation
xpm_map_t xpm_leve1_array[1] = {xpm_level1_no_lava};
//xpm_map_t xpm_leve1_array[1] = {xpm_level1_bricks};

xpm_map_t xpm_firemi_array[11] = {idle1_firemi_xpm, idle2_firemi_xpm, idle3_firemi_xpm, l1_firemi_xpm, l2_firemi_xpm, l3_firemi_xpm, r1_firemi_xpm, r2_firemi_xpm, r3_firemi_xpm, push_left_firemi_xpm, push_right_firemi_xpm};

xpm_map_t xpm_waternix_array[11] = {idle1_waternix_xpm, idle2_waternix_xpm, idle3_waternix_xpm, l1_waternix_xpm, l2_waternix_xpm, l3_waternix_xpm, r1_waternix_xpm, r2_waternix_xpm, r3_waternix_xpm, push_left_waternix_xpm, push_right_waternix_xpm};

xpm_map_t xpm_slider_array[1] = {xpm_slider};
xpm_map_t xpm_box1_array[1] = {xpm_box1};
xpm_map_t xpm_box2_array[1] = {xpm_box2};

xpm_map_t xpm_red_lava[1] = {lava_red};
xpm_map_t xpm_purple_lava[1] = {lava_purple};
xpm_map_t xpm_blue_lava[1] = {lava_blue};

/**
 * @brief game main loop, where the driver receive is called
 * @return 0 if no erros, 1 otherwise
 */
int(proj_main_loop)(int argc, char *argv[]) {
  printf("Welcome to FireMi & WaterNix!!!\n");

  //initiating the VBE
  vg_init(VBE_DIRECT_800_MODE);

  //creating the clock element
  Clock *clock = create_clock(652, 20);

  //creating the cursor
  Cursor *cursor = create_cursor(400, 300);

  //creating main character sprites
  Sprite *firemi = create_sprite(xpm_firemi_array, 20, 510, 11);
  Sprite *waternix = create_sprite(xpm_waternix_array, 50, 510, 11);

  //creating game board sprite elements for level1
  Sprite *level_1 = create_sprite(xpm_leve1_array, 0, 0, 1);
  Sprite *slider = create_sprite(xpm_slider_array, 317, 90, 1);
  Sprite *box1 = create_sprite(xpm_box1_array, 390, 509, 1);
  Sprite *box2 = create_sprite(xpm_box2_array, 563, 354, 1);
  Game_button *game_button1 = create_game_button(xpm_button_1, 66, 376, SOUTH);
  Game_button *buttons1[1] = {game_button1};
  Game_bar *game_bar1 = create_game_bar(xpm_bar_1, 555, 510, 0, 0, 0, -90, -1, buttons1, 1);
  Game_button *game_button2 = create_game_button(xpm_button_2, 705, 570, NORTH);
  Game_button *buttons2[1] = {game_button2};
  Game_bar *game_bar2 = create_game_bar(xpm_bar_2, 195, 270, 104, 270, 0, 0, 0, buttons2, 1);
  Game_button *game_button3 = create_game_button(xpm_button_3, 720, 255, NORTH);
  Game_button *buttons3[1] = {game_button3};
  Game_bar *game_bar3 = create_game_bar(xpm_bar_3, 645, 90, 554, 90, 0, 0, 0, buttons3, 1);

  Sprite *red_lava = create_sprite(xpm_red_lava, 255, 584, 1);
  Sprite *blue_lava = create_sprite(xpm_blue_lava, 585, 584, 1);
  Sprite *purple_lava = create_sprite(xpm_purple_lava, 391, 403, 1);
  //to handle lava movement
  bool lava_change = false;
  bool lava_change_blue = false;
  bool lava_change_purple = false;

  //title elements
  xpm_map_t level1_completed_title_xpm_array[1] = {level1_completed_title};
  Sprite *level1_completed = create_sprite(level1_completed_title_xpm_array, 0, 0, 1);

  //used to acknoledge the button of who ca trigger him TODO: can used for bar collision against them wich is not done yet
  Sprite *objs_to_collide[4] = {firemi, waternix, box1, box2};

  //EXPEIRMENTTTTTT
  /*
  Sprite * firemi = create_sprite(xpm_firemi_array, 200, 310, 6);
  draw_resized_sprite(firemi, 100, 100);
  sleep(4);
  */

  //draw_snow(1, 5, 800, 600, 130);

  //drawing level1 elements
  draw_clock(clock);
  draw_sprite(level_1);
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

  //set of keys to the two main characters
  bool keys_firemi[4] = {0, 0, 0, 0};   //{W, A, S, D}
  bool keys_waternix[4] = {0, 0, 0, 0}; //{^, <-, v, ->}

  //to handle the change of xpm to animate sprite when running
  int n_maps_f = 0, n_maps_w = 0;
  int n_map_2_f = 0, n_map_2_w = 0;

  int ipc_status;
  message msg;
  uint8_t kbd_bit_no, timer_bit_no, mouse_bit_no, rtc_bit_no;
  timer_set_frequency(0, 60);
  int r, wait = 60 / FPS;
  struct packet mouse_packet;
  rtc_time time;

  //subscribing the interrupt notifications for all devices needed
  if (keyboard_subscribe_int(&kbd_bit_no) != OK)
    return 1;
  if (timer_subscribe_int(&timer_bit_no) != OK)
    return 1;
  if (mouse_subscribe_int(&mouse_bit_no) != OK)
    return 1;
  if (rtc_subscribe_int(&rtc_bit_no) != OK)
    return 1;
  rtc_enable_update_int();

  uint64_t kbd_irq_set = BIT(kbd_bit_no);
  uint64_t timer_irq_set = BIT(timer_bit_no);
  uint64_t mouse_irq_set = BIT(mouse_bit_no);
  uint64_t rtc_irq_set = BIT(rtc_bit_no);

  //main driver receive loop
  while (bytes[0] != ESC && !exit_game) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //hardware interrupt notification
          if (msg.m_notify.interrupts & kbd_irq_set) {
            keyboard_ih();

            if (bytes[0] == KEY_MAKE_W)
              keys_firemi[0] = true;
            if (bytes[0] == KEY_MAKE_A)
              keys_firemi[1] = true;
            if (bytes[0] == KEY_MAKE_S)
              keys_firemi[2] = true;
            if (bytes[0] == KEY_MAKE_D)
              keys_firemi[3] = true;
            if (bytes[0] == KEY_BREAK_W)
              keys_firemi[0] = false;
            if (bytes[0] == KEY_BREAK_A)
              keys_firemi[1] = false;
            if (bytes[0] == KEY_BREAK_S)
              keys_firemi[2] = false;
            if (bytes[0] == KEY_BREAK_D)
              keys_firemi[3] = false;

            if (bytes[0] == TWO_BYTE_SCNCODE_PREFIX && !making_scancode) {
              if (bytes[1] == KEY_MAKE_ARROW_UP)
                keys_waternix[0] = true;
              if (bytes[1] == KEY_MAKE_ARROW_LEFT)
                keys_waternix[1] = true;
              if (bytes[1] == KEY_MAKE_ARROW_DOWN)
                keys_waternix[2] = true;
              if (bytes[1] == KEY_MAKE_ARROW_RIGHT)
                keys_waternix[3] = true;
              if (bytes[1] == KEY_BREAK_ARROW_UP)
                keys_waternix[0] = false;
              if (bytes[1] == KEY_BREAK_ARROW_LEFT)
                keys_waternix[1] = false;
              if (bytes[1] == KEY_BREAK_ARROW_DOWN)
                keys_waternix[2] = false;
              if (bytes[1] == KEY_BREAK_ARROW_RIGHT)
                keys_waternix[3] = false;
            }
          }
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            if (timer_counter % wait == 0) {
              //level1 handlers
              handle_lava(red_lava, level_1, 255, &lava_change, 120);
              handle_lava(blue_lava, level_1, 585, &lava_change_blue, 90);
              handle_lava(purple_lava, level_1, 391, &lava_change_purple, 90);
              handle_game_button(game_button1, level_1, 4, objs_to_collide);
              handle_game_bar(game_bar1, level_1, objs_to_collide, 4);
              handle_game_button(game_button2, level_1, 4, objs_to_collide);
              handle_game_bar(game_bar2, level_1, objs_to_collide, 4);
              handle_game_button(game_button3, level_1, 4, objs_to_collide);
              handle_game_bar(game_bar3, level_1, objs_to_collide, 4);
              handle_slider_move(slider, level_1);
              handle_game_box(firemi, waternix, box1, level_1);
              handle_game_box(firemi, waternix, box2, level_1);
              handle_characters_move(firemi, waternix, level_1, keys_firemi, keys_waternix, &game_over, &n_maps_f, &n_maps_w, &n_map_2_f, &n_map_2_w);
              draw_cursor(cursor, level_1);
              handle_win(firemi, waternix, level1_completed);
              if (game_over) {
                handle_lost();
                exit_game = true;
              }
            }
            if (timer_counter % 60 == 0) {
              tick_clock(clock, level_1);
              timer_counter = 0;
            }
          }
          if (msg.m_notify.interrupts & mouse_irq_set) {
            mouse_ih();
            if (build_packet(&mouse_packet)) {
              update_cursor(cursor, mouse_packet);
            }
          }
          if (msg.m_notify.interrupts & rtc_irq_set) {
            rtc_ih();
            rtc_get_time(&time);
            rtc_print_time(time);
          }
          break;
        default:
          break;
      }
    }
  }

  //unsubscribing the interrupt notifications for both devices
  if (keyboard_unsubscribe_int() != OK)
    return 1;
  if (timer_unsubscribe_int() != OK)
    return 1;
  if (mouse_unsubscribe_int() != OK)
    return 1;

  //exiting the VBE graphics mode, puting back to text mode
  vg_exit();

  //freing the sprites memory
  delete_sprite(level_1);
  delete_sprite(firemi);
  delete_sprite(waternix);
  delete_sprite(box1);
  delete_sprite(box2);

  delete_clock(clock);
  delete_cursor(cursor);

  delete_game_button(game_button1);
  delete_game_bar(game_bar1);
  delete_game_button(game_button2);
  delete_game_bar(game_bar2);
  delete_game_button(game_button3);
  delete_game_bar(game_bar3);

  delete_sprite(level1_completed);

    delete_sprite(red_lava);
  delete_sprite(purple_lava);
  delete_sprite(blue_lava);

  return 0;
}
