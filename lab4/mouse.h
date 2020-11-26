#ifndef MOUSE_H
#define MOUSE_H

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

int (kbc_writing_cmd)(int reg, int cmd, int tries);

int (kbc_reading_cmd)(uint8_t st, int tries);

int (mouse_write_cmd)(uint8_t cmd);

int (mouse_read_cmd)(uint8_t st);

void (mouse_ih)();

int (output_buff_flush)();

void (assemble_bytes)(int *counter, uint8_t pac[3]);

void (build_packet)(int *counter, uint8_t pac[3], uint32_t *cnt);
/*
int (enable_data_reporting)();
*/

#endif
