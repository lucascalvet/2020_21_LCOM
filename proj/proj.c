//LCOM header files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <lcom/timer.h>

//c header files
#include <stdbool.h>
#include <stdint.h>

//project header files - modulos
#include "video_gr.h"
#include "game_engine.h"

//project header files - xpm's
#include "xpm_levels.h"
#include "xpm_characters.h"
#include "xpm_titles.h"
#include "xpm_game_elements.h"

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
xpm_map_t xpm_leve1_array[1] = {xpm_level1_without_elements};
xpm_map_t xpm_firemi_array[3] = {xpm_firemi, firemi_run_l, firemi_run_r};
xpm_map_t xpm_waternix_array[3] = {xpm_waternix, waternix_run_l, waternix_run_r};
xpm_map_t xpm_game_button[1] = {xpm_button};

/**
 * @brief game main loop, where the driver receive is called
 * @return 0 if no erros, 1 otherwise
 */
int(proj_main_loop)(int argc, char *argv[]){
  printf("Welcome to FireMi & WaterNix!!!\n");

  //initiating the VBE
  vg_init(VBE_DIRECT_800_MODE);

  //creating main sprites needed
  Sprite * level_1 = create_sprite(xpm_leve1_array, 0, 0, 1);
  Sprite * firemi = create_sprite(xpm_firemi_array, 20, 510, 3);
  Sprite * waternix = create_sprite(xpm_waternix_array, 50, 510, 3);

  //creating game board sprite elements
  Game_button *game_button = create_game_button(xpm_game_button, 80, 560, 1, NORTH);

  //drawing main sprites
  draw_sprite(level_1);
  draw_sprite(firemi);
  draw_sprite(waternix);
  draw_game_button(game_button);

  //set of keys to the two main characters
  bool keys_firemi[4] = {0, 0, 0, 0}; //{W, A, S, D}
  bool keys_waternix[4] = {0, 0, 0, 0}; //{^, <-, v, ->}

  int ipc_status;
  message msg;
  uint8_t kbd_bit_no, timer_bit_no;
  timer_set_frequency(0, 60);
  int r, wait = 60 / FPS;

  //subscribing the intrrupt notifications for both devices
  if (keyboard_subscribe_int(&kbd_bit_no) != OK) return 1;
  if (timer_subscribe_int(&timer_bit_no) != OK) return 1;

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
            
            if (bytes[0] == KEY_MAKE_W) keys_firemi[0] = true;
            if (bytes[0] == KEY_MAKE_A) keys_firemi[1] = true;
            if (bytes[0] == KEY_MAKE_S) keys_firemi[2] = true;
            if (bytes[0] == KEY_MAKE_D) keys_firemi[3] = true;
            if (bytes[0] == KEY_BREAK_W) keys_firemi[0] = false;
            if (bytes[0] == KEY_BREAK_A) keys_firemi[1] = false;
            if (bytes[0] == KEY_BREAK_S) keys_firemi[2] = false;
            if (bytes[0] == KEY_BREAK_D) keys_firemi[3] = false;
            
            if(bytes[0] == TWO_BYTE_SCNCODE_PREFIX && !making_scancode){
              if (bytes[1] == KEY_MAKE_ARROW_UP) keys_waternix[0] = true;
              if (bytes[1] == KEY_MAKE_ARROW_LEFT) keys_waternix[1] = true;
              if (bytes[1] == KEY_MAKE_ARROW_DOWN) keys_waternix[2] = true;
              if (bytes[1] == KEY_MAKE_ARROW_RIGHT) keys_waternix[3] = true;
              if (bytes[1] == KEY_BREAK_ARROW_UP) keys_waternix[0] = false;
              if (bytes[1] == KEY_BREAK_ARROW_LEFT) keys_waternix[1] = false;
              if (bytes[1] == KEY_BREAK_ARROW_DOWN) keys_waternix[2] = false;
              if (bytes[1] == KEY_BREAK_ARROW_RIGHT) keys_waternix[3] = false;
            }
          }
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            if(timer_counter % wait == 0){
              handle_characters_move(firemi, waternix, level_1, keys_firemi, keys_waternix, &game_over, game_button);
            }
          }
          break;
        default:
          break;
      }
    }
  }

  //unsubscribing the intrrupt notifications for both devices
  if (keyboard_unsubscribe_int() != OK) return 1;
  if (timer_unsubscribe_int() != OK) return 1;

  if(game_over){
    xpm_map_t game_over_title_xpm_array[1] = {game_over_title_xpm};
    Sprite * game_over_title = create_sprite(game_over_title_xpm_array, 0, 0, 1);
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
  delete_game_button(game_button);

  return 0;
}
