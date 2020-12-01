#include "mouse.h"
#include "ps2.h"
#include "utils.h"

int hook_id_mouse = 2;
uint8_t data; //used to hold the data from interrupt handler

/**
 * @brief subscribes mouse interruptions
 * @param bit_no
 * @return 0 if no erros ocurred, 1 otherwise
 */
int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id_mouse;

  //uses IRQ_EXCLUSIVE to prevent minix interrupt handler
  if (sys_irqsetpolicy(KBC_IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse) != OK)
    return 1;
  return 0;
}

/**
 * @brief unsunscribes mouse interruptions
 * @return 0 if no erros ocurred, 1 otherwise
 */
int(mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id_mouse) != OK)
    return 1;
  return 0;
}

/**
 * @brief writes command to mouse (KBC_WRITE_TO_MOUSE KBC command argument) and prints in console mouse acknowledgment 
 * @param command the command to be input to the mouse
 * @return 0 if no erros ocurred, 1 otherwise
 */
int(mouse_write_cmd)(uint8_t command) {

  uint8_t acknowledgment_data = 0;

    kbc_write_cmd(KBC_WRITE_TO_MOUSE);

    sys_outb(KBC_INPUT_BUF, command); //command (argument of KBC_WRITE_TO_MOUSE kbc command)

    kbc_read_data(&acknowledgment_data);

    if (acknowledgment_data == MOUSE_ACK) {
      if(command == PS2_ENABLE_DT_REP)
        printf("Everything is Ok, mouse data report enabled!\n");
      else if(command == PS2_DISABLE_DT_REP)
        printf("Everything is OK, mouse data report disabled!\n");
      else
         printf("Everything is OK!\n");
    }
    else if (acknowledgment_data == MOUSE_NACK) {
      printf("Invalid byte!\n");
    }
    else if (acknowledgment_data == MOUSE_ERROR) {
      printf("Second consecutive invalid byte!\n");
    }

  return 0;
}

/**
 * @brief enables data reporting on mouse
 * @return 0 if no erros ocurred, 1 otherwise
 */
int(mouse_enable_data_report)() {
  if (mouse_write_cmd(PS2_ENABLE_DT_REP) != OK)
    return 1;
  return OK;
}

/**
 * @brief disables data reporting on mouse
 * @return 0 if no erros ocurred, 1 otherwise
 */
int(mouse_disable_data_report)() {
  if (mouse_write_cmd(PS2_DISABLE_DT_REP) != OK)
    return 1;
  return OK;
}

/**
 * @brief builds the mouse packe with 3 bytes
 * @param counter use to keep track of the byte number
 * @param packet pointer to packet array 
 * @return none
 */
void(build_packet)(int *counter, uint8_t packet[]) { //builds the struct packet to use
  if (*counter == 0) {                               //data & FIRST_BYTE_CHECK &
    packet[0] = data;
  }
  else if (*counter == 1) {
    packet[1] = data;
  }
  else if (*counter == 2) {
    packet[2] = data;
  }

  printf("%d", *counter);

  (*counter)++;

  if (*counter == 3) {
    struct packet packet_struct;

    for (int i = 0; i < 3; i++) {
      packet_struct.bytes[i] = packet[i];
    }

    bool rb = packet[0] & PACKET_RB;
    bool mb = packet[0] & PACKET_MB;
    bool lb = packet[0] & PACKET_LB;
    bool x_ov = packet[0] & PACKET_X_OVFL;
    bool y_ov = packet[0] & PACKET_Y_OVFL;

    packet_struct.rb = rb;
    packet_struct.mb = mb;
    packet_struct.lb = lb;
    packet_struct.x_ov = x_ov;
    packet_struct.y_ov = y_ov;

    uint16_t x = 0, y = 0;

    //convert from 2's complement to 16 bit normal

    packet_struct.delta_x = x;
    packet_struct.delta_y = y;

    mouse_print_packet(&packet_struct); //prints mouse packet

    *counter = 0;
  }
}

/**
 * @brief handles mouse interrupts
 * @return none
 */
void(mouse_ih)() {
  kbc_read_data(&data);
}
