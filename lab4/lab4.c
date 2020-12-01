#include "mouse.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
<<<<<<< HEAD
#include "mouse.h"
#include <lcom/timer.h>

uint8_t byte;
uint8_t packet[3];
static int counter = 0;
=======
>>>>>>> 96d8dac7cde79e76913f9ad138e10eb70644c91d

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {
  int ipc_status, r;
  message msg;
  uint8_t bit_no = 2;

<<<<<<< HEAD
  mouse_subscribe_int(&bit_no);
=======
  if(mouse_subscribe_int(&bit_no) != OK)
    return 1;

  if(mouse_enable_data_report() != OK)
    return 1;
>>>>>>> 96d8dac7cde79e76913f9ad138e10eb70644c91d

  mouse_enable_data_reporting();
  //enable_data_reporting();

  uint64_t irq_set = BIT(bit_no);

  int counter = 0;
  uint8_t packet[3];

  while (cnt > 0) {
    
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {

      printf("Driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();
            build_packet(&counter, packet);
            if(counter == 3)
              cnt--;
          }
          break;
        default:
          break;
      }
    }
  }
<<<<<<< HEAD
  disable_data_reporting();
  mouse_unsubscribe_int();
 /*
  if(output_buff_flush() != OK)
    return -1;
*/
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status;
  message msg;
  uint8_t mouse_bit_no, timer_bit_no;
  int r, freq = sys.hz();
  uint32_t cnt = 0; //not needed, but is function argument

  mouse_enable_data_reporting();

  mouse_subscribe_int(&mouse_bit_no);
  timer_subscribe_int(&timer_bit_no);

  uint64_t mouse_irq_set = BIT(bit_no);
  uint64_t timer_irq_set = BIT(bit_no);

/*
  if(enable_data_reporting() != OK)
    return -1;
*/

  while (cnt > 0){ 
    if((r = driver_receive(ANY, &msg, &ipc_status)) != OK){
      printf("Driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)){
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_irq_set) {
            //printf("cnt: %d\n", cnt);
            mouse_ih();
            assemble_bytes(&counter, packet);
            build_packet(&counter, packet, &cnt);
          }
          if (msg.m_notify.interrupts & timer_irq_set) {
            if(counter % freq == 0){
              timer_print_elapsed_time();
              counter = 0;
              n--;
            }
          }
          break;
        default:
          break;
      }
    }
  }
  mouse_unsubscribe_int();
 /*
  if(output_buff_flush() != OK)
    return -1;
*/
  return 0;
}
=======

  if(mouse_disable_data_report() != OK)
    return 1;
>>>>>>> 96d8dac7cde79e76913f9ad138e10eb70644c91d

  if(mouse_unsubscribe_int() != OK)
    return 1;

  if (output_buff_flush() != OK)

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  /* To be completed */
  printf("%s(%u): under construction\n", __func__, idle_time);
  return 1;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  /* To be completed */
  printf("%s: under construction\n", __func__);
  return 1;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* To be completed */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}
