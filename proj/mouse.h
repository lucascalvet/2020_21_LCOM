/* defines functions that handles 
with the mouse PS2, through KBC */

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include "kbc.h"

#ifndef MOUSE_H
#define MOUSE_H

//subscribes mouse interruptions
int(mouse_subscribe_int)(uint8_t *bit_no);

//unsunscribes mouse interruptions
int(mouse_unsubscribe_int)();

//write command to mouse (after a KBC_WRITE_TO_MOUSE KBC command)
int(mouse_write_cmd)(uint8_t command);

//enables mouse data reporting on mouse
int(mouse_enable_data_report)();

//disables mouse data reporting
int(mouse_disable_data_report)();

//build the mouse packet bytes
bool(build_packet)(struct packet *packet);

//handles the mouse interrupts
void(mouse_ih)();

#endif //MOUSE_H
