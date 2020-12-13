#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

int (keyboard_writing_cmd)(int reg, int cmd, int tries);

int (keyboard_reading_cmd)(int tries);

void (keyboard_ih)();

int (keyboard_unsubscribe_int)();

int (keyboard_subscribe_int)(uint8_t *bit_no);
#endif
