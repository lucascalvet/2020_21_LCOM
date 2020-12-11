// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you
#include "video_gr.h"
#include "sprite.h"
#include <lcom/timer.h>
#include "xpm_levels.h"
#include "xpm_characters.h"

unsigned timer_counter = 0;

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

int(proj_main_loop)(int argc, char *argv[]){
  printf("Welcome to FireMi & WaterNix!!!\n");
  proj_demo(VBE_DIRECT_800_MODE, true, false, 1);
  vg_init(VBE_DIRECT_800_MODE);
  Sprite * level_1 = create_sprite(xpm_level1, 0, 0);
  Sprite * firemi = create_sprite(xpm_firemi, 20, 530);
  Sprite * waternix = create_sprite(xpm_waternix, 50, 530);
  draw_sprite(level_1);
  draw_sprite(firemi);
  draw_sprite(waternix);
  bool keys[4] = {0, 0, 0, 0}; //{W, A, S, D}

  int ipc_status;
  message msg;
  uint8_t kbd_bit_no, timer_bit_no;
  timer_set_frequency(0, 60);
  int fr_rate = 30; //TODO: change to macro
  int r, wait = 60 / fr_rate;

  if (keyboard_subscribe_int(&kbd_bit_no) != OK) return 1;
  if (timer_subscribe_int(&timer_bit_no) != OK) return 1;

  uint64_t kbd_irq_set = BIT(kbd_bit_no);
  uint64_t timer_irq_set = BIT(timer_bit_no);

  while (data != ESC) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & kbd_irq_set) {
            kbc_ih();
            if (data == KEY_MAKE_W) keys[0] = true;
            if (data == KEY_MAKE_A) keys[1] = true;
            if (data == KEY_MAKE_S) keys[2] = true;
            if (data == KEY_MAKE_D) keys[3] = true;
            if (data == KEY_BREAK_W) keys[0] = false;
            if (data == KEY_BREAK_A) keys[1] = false;
            if (data == KEY_BREAK_S) keys[2] = false;
            if (data == KEY_BREAK_D) keys[3] = false;
            printf("\nkeys = {%x, %x, %x, %x}", keys[0], keys[1], keys[2], keys[3]);
          }
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            if(timer_counter % wait == 0){
              
            }
          }
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    }
  }

  if (keyboard_unsubscribe_int() != OK) return 1;
  if (timer_unsubscribe_int() != OK) return 1;

  sleep(3);
  vg_exit();
  delete_sprite(level_1);
  return 0;
}
