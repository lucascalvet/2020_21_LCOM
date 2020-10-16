#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t ctrl;
  if (timer_get_conf(timer, &ctrl) != OK)
      return 1;
  ctrl <<= 4;
  ctrl >>= 4;
  ctrl |= (timer << 6) | TIMER_LSB_MSB;
  if (sys_outb(TIMER_CTRL, ctrl) != OK)
      return 1;
  uint16_t val = TIMER_FREQ/freq;
  uint8_t send;
  int port = TIMER_0 + timer;
  if (util_get_LSB(val, &send) != OK)
      return 1;
  if (sys_outb(port, send) != OK)
      return 1;
  if (util_get_MSB(val, &send) != OK)
      return 1;
  if (sys_outb(port, send) != OK)
      return 1;
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if(timer < 0 || timer > 2)
      return 1;
  uint32_t rb = 0;
  rb |= TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  if(sys_outb(TIMER_CTRL, rb) != OK)
      return 1;
  int port = TIMER_0 + timer;
  if(util_sys_inb(port, st) != OK)
      return 1;
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val conf;
  switch (field) {
      case tsf_all:
          conf.byte = st;
          break;
      case tsf_initial:
          st <<= 2;
          st >>= 6;
          switch (st) {
              case 1:
                  conf.in_mode = LSB_only;
                  break;
              case 2:
                  conf.in_mode = MSB_only;
                  break;
              case 3:
                  conf.in_mode = MSB_after_LSB;
                  break;
              default:
                  conf.in_mode = INVAL_val;
          }
          break;
      case tsf_mode:
          st <<= 4;
          st >>= 5;
          if (st > 5)
              st -= 4;
          conf.count_mode = st;
          break;
      case tsf_base:
          st <<= 7;
          st >>= 7;
          conf.bcd = st;
          break;
  }
  if(timer_print_config(timer, field, conf) != OK)
      return 1;
  return 0;
}
