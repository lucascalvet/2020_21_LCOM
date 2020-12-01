/* KBC related functions that works 
for mouse and keyboard operations */
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include "i8042.h"

//writes command to control register
int(kbc_write_cmd)(int command);

//reads data from KBC output buffer
int(kbc_read_data)(uint8_t *data);

//clears the KBC output buffer
int(output_buff_flush)();
