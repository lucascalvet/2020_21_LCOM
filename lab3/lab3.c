#include <lcom/lcf.h>
#include <minix/sysutil.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"
#include "utils.h"
#include <lcom/lab2.h>
//#include "timer.c"
#include <lcom/timer.h>

unsigned counter = 0;


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() { 
  int ipc_status;
  message msg;

  uint8_t bit_no;
  keyboard_subscribe_int(&bit_no);

  uint64_t irq_set = BIT(bit_no);
  int r;

  while (data != ESC){ 
    if((r = driver_receive(ANY, &msg, &ipc_status)) != OK){
      printf("Driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)){
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
          }
          break;
        default:
          break;
      }
    }
  }
  printf("Counter: %d", count);

  keyboard_unsubscribe_int();
  return 0;
}

int(kbd_test_poll)() {
  uint8_t sta = 0;

  count = 0;

  /*keyboard_writing_cmd(KBC_CMD_BUF, KBC_READ_CMD, 3);
  printf("after kbc write");

  if(util_sys_inb(KBC_ST_REG, &sta) != OK)
    return 1;
  */
  printf("before while");

  while (data != ESC){
    printf("In while");
      kbc_ih();  //not checking anywhere for KBC_ST_AUX
  }

  //restore
  keyboard_writing_cmd(KBC_CMD_BUF, KBC_READ_CMD, 3);
  keyboard_reading_cmd(sta, 3);
  keyboard_writing_cmd(KBC_CMD_BUF, KBC_WRITE_CMD , 3);
  keyboard_writing_cmd(KBC_INPUT_BUF, KBC_ST_OBF | data, 3);

  printf("Sys_inb calls: %d", count);

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status;
  int idle = n;
  message msg;

  uint8_t bit_no1, bit_no2;
  keyboard_subscribe_int(&bit_no2);
  timer_subscribe_int(&bit_no1);

  uint64_t kbd_int_bit = BIT(bit_no2);
  uint64_t timer0_int_bit = BIT(bit_no1);

  int r;

  while(data != ESC && n > 0){
    //printf("HERE");
    if((r = driver_receive(ANY, &msg, &ipc_status)) != OK){
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)){
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & kbd_int_bit) { // KBD int?
            kbc_ih();
            n = idle;
          }
          if (msg.m_notify.interrupts & timer0_int_bit) { // Timer0 int?
            timer_int_handler();
            if(counter % 60 == 0){
              timer_print_elapsed_time();
              counter = 0;
              n--;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
  }

  timer_unsubscribe_int();
  keyboard_unsubscribe_int();

  return 0;
}
