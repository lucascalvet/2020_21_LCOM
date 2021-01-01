#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <stdint.h>

#define UART_IRQ 8
int(rtc_subscribe_int)(uint8_t *bit_no);

int(rtc_unsubscribe_int)();

#endif