#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

uint8_t data;

int (keyboard_writing_cmd)(int reg, int cmd, int tries);

int (keyboard_reading_cmd)(uint8_t st, int tries);

int (keyboard_ih)(uint8_t cmd);

int (keyboard_unsubscribe_int)();

int (keyboard_subscribe_int)(uint8_t *bit_no);
#endif
