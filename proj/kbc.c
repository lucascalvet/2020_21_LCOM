#include "kbc.h"

/**
 * @brief writes command to KBC control register 0x64
 * @param command specifies the command to be sent to KBC
 * @return 0 if no erros ocurred, 1 otherwise
 */
int(kbc_write_cmd)(int command) {

  uint8_t status = 0; //to retrieve status register byte

  for (int i = 0; i < NUM_TRIES; i++) { //loop while 8042 input buffer is not empty max [tries] times, then exit
    util_sys_inb(KBC_STATUS_REG, &status);
    
    if (!(status & KBC_ST_IBF)) { //only executes when input is free
      sys_outb(KBC_CONTROL_REG, command);
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

/**
 * @brief writes argument to KBC input buffer 0x60
 * @param arg specifies the argument to be sent to KBC
 * @return 0 if no erros ocurred, 1 otherwise
 */
int(kbc_write_arg)(int arg) {

  uint8_t status = 0; //to retrieve status register byte

  for (int i = 0; i < NUM_TRIES; i++) { //loop while 8042 input buffer is not empty max [tries] times, then exit
    util_sys_inb(KBC_STATUS_REG, &status);

    if (!(status & KBC_ST_IBF)) { //only executes when input is free
      sys_outb(KBC_INPUT_BUF, arg);
      return 0;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

/**
 * @brief reads data in the output buffer (0x60) of KBC
 * @param data the variable that is holding the readed data
 * @return 0 if no erros ocurred, 1 otherwise
 */
int(kbc_read_data)(uint8_t *data) {

  uint8_t status = 0; //to retrieve status register byte

  for (int i = 0; i < NUM_TRIES; i++) {
    util_sys_inb(KBC_STATUS_REG, &status);
    //only if output buffer is full and has data to read
    if (true) { //TODO: KBC_ST_OBF not raised when receiving aknowledgment, but receives ACK successfully, so I changed to always read the OUT_BUF...
      util_sys_inb(KBC_OUT_BUF, data);
      return 0;
    }
    
    if (status & (KBC_ST_PARITY | KBC_ST_TIMEOUT)) //checks for errors
      return 1;
    
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}
  

/**
 * @brief cleans data from output buffer of KBC
 * @return 0 if no erros ocurred, 1 otherwise
 */
int(output_buff_flush)() {
  uint8_t flushed_data = 0;

  //only needs to read the out buffer one time and the byte is flushed
  kbc_read_data(&flushed_data);

  return 0;
}
