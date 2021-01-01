#include "uart.h"
#include "utils.h"

int hook_id_uart;

int(rtc_subscribe_int)(uint8_t *bit_no) {
  hook_id_uart = 4;
  *bit_no = hook_id_uart;
  if (sys_irqsetpolicy(UART_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_uart) != OK)
    return 1;
  return 0;
}

int(rtc_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id_uart) != OK)
    return 1;
  return 0;
}
