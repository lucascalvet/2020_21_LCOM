//LCOM header files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <lcom/timer.h>

//c header files
#include <stdbool.h>
#include <stdint.h>

//project header files - modulos
#include "game_handler.h"
#include "game_engine.h"
#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"
#include "video_gr.h"

//global variables
unsigned timer_counter = 0;
extern uint8_t bytes[2];
extern bool making_scancode;

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

/**
 * @brief game main loop, where the driver receive is called
 * @return 0 if no erros, 1 otherwise
 */
int(proj_main_loop)(int argc, char *argv[]) {
  //initiating the VBE
  vg_init(VBE_DIRECT_800_MODE);
  
  enum game_state state = MAIN_MENU;
  enum game_state prev_state = MAIN_MENU;
  create_level(state);
  draw_level(state);

  //EXPEIRMENTTTTTT
  /*
  Sprite * firemi = create_sprite(xpm_firemi_array, 200, 310, 6);
  draw_resized_sprite(firemi, 100, 100);
  sleep(4);
  */

  //draw_snow(1, 5, 800, 600, 130);

  //set of keys to the two main characters
  bool keys_firemi[4] = {0, 0, 0, 0};   //{W, A, S, D}
  bool keys_waternix[4] = {0, 0, 0, 0}; //{^, <-, v, ->}

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
  while (bytes[0] != ESC) {
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

            if(bytes[0] == BACKSPACE && state != MAIN_MENU){
              prev_state = state;
              state = PAUSE;
              create_level(state);
              draw_level(state);
            }
          }
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            if (timer_counter % wait == 0) {
              handle_level(&state, &prev_state, keys_firemi, keys_waternix);
            }
            if (timer_counter % 60 == 0) {
              if (state != MAIN_MENU && state != PAUSE) tick_game_clock();
              timer_counter = 0;
            }
          }
          if (msg.m_notify.interrupts & mouse_irq_set) {
            mouse_ih();
            if (build_packet(&mouse_packet)) {
              update_game_cursor(&state, mouse_packet, prev_state);
            }
          }
          if (msg.m_notify.interrupts & rtc_irq_set) {
            rtc_ih();
            rtc_get_time(&time);
            update_game_time(state, time);
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

  return 0;
}
