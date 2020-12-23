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
#include "video_gr.h"

//project header files - xpm's
#include "xpm_characters.h"
#include "xpm_game_elements.h"
#include "xpm_levels.h"
#include "xpm_slider.h"
#include "xpm_titles.h"

//global variables
unsigned timer_counter = 0;
extern uint8_t bytes[2];
extern bool making_scancode;
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
xpm_map_t xpm_leve1_array[1] = {xpm_level1_with_slope_red_lava};
xpm_map_t xpm_firemi_array[6] = {xpm_firemi, firemi_run_l, firemi_run_r, xpm_waternix, waternix_run_l, waternix_run_r};
xpm_map_t xpm_waternix_array[6] = {xpm_waternix, waternix_run_l, waternix_run_r, xpm_firemi, firemi_run_l, firemi_run_r}; //the last xpm are only to test the animated sprite
xpm_map_t xpm_slider_array[1] = {xpm_slider};
xpm_map_t xpm_box_array[1] = {xpm_box};

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

  //creating main character sprites
  Sprite *firemi = create_sprite(xpm_firemi_array, 20, 510, 6);
  Sprite *waternix = create_sprite(xpm_waternix_array, 50, 510, 6);

  //creating game board sprite elements for level1
  Sprite *level_1 = create_sprite(xpm_leve1_array, 0, 0, 1);
  Sprite *slider = create_sprite(xpm_slider_array, 317, 90, 1);
  Sprite *box = create_sprite(xpm_box_array, 390, 509, 1);
  Game_button *game_button1 = create_game_button(xpm_button_1, 66, 376, SOUTH);
  Game_bar *game_bar1 = create_game_bar(xpm_bar_1, 555, 510, 0, 0, 0, -90, -1, game_button1);
  Game_button *game_button2 = create_game_button(xpm_button_2, 705, 570, NORTH);
  Game_bar *game_bar2 = create_game_bar(xpm_bar_2, 195, 270, 106, 270, 0, 0, 0, game_button2);

  //used to acknoledge the button of who ca trigger him TODO: can used for bar collision against them wich is not done yet
  Sprite *objs_to_collide[3] = {firemi, waternix, box};

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
  draw_sprite(box);
  draw_sprite(slider);
  draw_sprite(game_button1->button_sprite);
  draw_sprite(game_bar1->bar_sprite);
  draw_sprite(game_button2->button_sprite);
  draw_sprite(game_bar2->bar_sprite);

  //set of keys to the two main characters
  bool keys_firemi[4] = {0, 0, 0, 0};   //{W, A, S, D}
  bool keys_waternix[4] = {0, 0, 0, 0}; //{^, <-, v, ->}

  //to handle the change of xpm to animate sprite when running
  int n_maps_f = 0;
  int n_maps_w = 0;

  int ipc_status;
  message msg;
  uint8_t kbd_bit_no, timer_bit_no;
  timer_set_frequency(0, 60);
  int r, wait = 60 / FPS;

  //subscribing the intrrupt notifications for both devices
  if (keyboard_subscribe_int(&kbd_bit_no) != OK)
    return 1;
  if (timer_subscribe_int(&timer_bit_no) != OK)
    return 1;

  uint64_t kbd_irq_set = BIT(kbd_bit_no);
  uint64_t timer_irq_set = BIT(timer_bit_no);

  //main driver receive loop
  while (bytes[0] != ESC && !game_over) {
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
              handle_game_button(game_button1, level_1, 3, objs_to_collide);
              handle_game_bar(game_bar1, level_1);
              handle_game_button(game_button2, level_1, 3, objs_to_collide);
              handle_game_bar(game_bar2, level_1);
              handle_slider_move(slider, level_1);
              handle_game_box(firemi, waternix, box, level_1);
              handle_characters_move(firemi, waternix, level_1, keys_firemi, keys_waternix, &game_over, &n_maps_f, &n_maps_w);
            }
            if (timer_counter % 60 == 0) {
              tick_clock(clock, level_1);
              timer_counter = 0;
            }
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

  if (game_over) {
    xpm_map_t game_over_title_xpm_array[1] = {game_over_title_xpm};
    Sprite *game_over_title = create_sprite(game_over_title_xpm_array, 0, 0, 1);
    draw_sprite(game_over_title);
    sleep(4);
    delete_sprite(game_over_title);
  }

  //exiting the VBE graphics mode, puting back to text mode
  vg_exit();

  //freing the sprites memory
  delete_sprite(level_1);
  delete_sprite(firemi);
  delete_sprite(waternix);

  delete_game_button(game_button1);
  delete_game_bar(game_bar1);
  
  delete_game_button(game_button2);
  delete_game_bar(game_bar2);

  return 0;
}
