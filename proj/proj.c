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
#include "xpm_boal.h"

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
//xpm array encapsulation
xpm_map_t xpm_leve1_array[1] = {xpm_level1};
xpm_map_t xpm_firemi_array[3] = {xpm_firemi, firemi_run_l, firemi_run_r};
xpm_map_t xpm_waternix_array[3] = {xpm_waternix, waternix_run_l, waternix_run_r};

int(proj_main_loop)(int argc, char *argv[]){
  printf("Welcome to FireMi & WaterNix!!!\n");
  //proj_demo(VBE_DIRECT_800_MODE, true, false, 1);
  vg_init(VBE_DIRECT_800_MODE);
  Sprite * level_1 = create_sprite(xpm_leve1_array, 0, 0, 1);
  Sprite * firemi = create_sprite(xpm_firemi_array, 20, 510, 3);
  Sprite * waternix = create_sprite(xpm_waternix_array, 50, 510, 3);
  //Sprite * boal = create_sprite(xpm_boal, 20, 520);
  draw_sprite(level_1);
  draw_sprite(firemi);
  draw_sprite(waternix);
  //draw_sprite(boal);
  //Sprite* collision_objects_firemi[1] = {waternix};
  //Sprite* collision_objects_waternix[1] = {firemi};
  //move_sprite(boal, 21, 300, 0, -1, level_1);
  bool keys_firemi[4] = {0, 0, 0, 0}; //{W, A, S, D}
  bool keys_waternix[4] = {0, 0, 0, 0}; //{^, <-, v, ->}

  int ipc_status;
  message msg;
  uint8_t kbd_bit_no, timer_bit_no;
  timer_set_frequency(0, 60);
  int r, wait = 60 / FPS;

  if (keyboard_subscribe_int(&kbd_bit_no) != OK) return 1;
  if (timer_subscribe_int(&timer_bit_no) != OK) return 1;

  uint64_t kbd_irq_set = BIT(kbd_bit_no);
  uint64_t timer_irq_set = BIT(timer_bit_no);

  while (bytes[0] != ESC) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
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
            /*
            if (data == KEY_MAKE_I) keys_waternix[0] = true;
            if (data == KEY_MAKE_J) keys_waternix[1] = true;
            if (data == KEY_MAKE_K) keys_waternix[2] = true;
            if (data == KEY_MAKE_L) keys_waternix[3] = true;
            if (data == KEY_BREAK_I) keys_waternix[0] = false;
            if (data == KEY_BREAK_J) keys_waternix[1] = false;
            if (data == KEY_BREAK_K) keys_waternix[2] = false;
            if (data == KEY_BREAK_L) keys_waternix[3] = false;
            */
            //printf("\nkeys = {%x, %x, %x, %x}", keys_firemi[0], keys_firemi[1], keys_firemi[2], keys_firemi[3]);
          }
          if (msg.m_notify.interrupts & timer_irq_set) {
            timer_int_handler();
            if(timer_counter % wait == 0){
              handle_characters_move(firemi, waternix, level_1, keys_firemi, keys_waternix);
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

  vg_exit();
  delete_sprite(level_1);
  delete_sprite(firemi);
  delete_sprite(waternix);
  //delete_sprite(boal);
  return 0;
}
