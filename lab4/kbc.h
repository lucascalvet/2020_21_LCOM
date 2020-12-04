/* KBC related functions that works 
for mouse and keyboard operations */
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include "i8042.h"

#define NUM_TRIES 100

//writes command to control register
int(kbc_write_cmd)(int command);

//writes command to input buffer
int(kbc_write_arg)(int command);

//reads data from KBC output buffer
int(kbc_read_data)(uint8_t *data);

//clears the KBC output buffer
int(output_buff_flush)();
