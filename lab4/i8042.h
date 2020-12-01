/* separated i8042 (KBC) constants to different 
file instead of kbc.h to be easier to read */

#ifndef i8042_H
#define i8042_H

//port addresses
#define KBC_OUT_BUF     0x60  //receives data from PS2 mouse
#define KBC_INPUT_BUF   0x60  
#define KBC_STATUS_REG  0x64  //it has information about the state of the KBC configuration
#define KBC_CONTROL_REG 0x64

//status register commmands
#define KBC_READ_CMD       0x20
#define KBC_WRITE_CMD      0x60
#define KBC_SELF_TEST_CMD  0xAA
#define KBC_IF_TEST_CMD    0xAB
#define KBC_DISABLE_CMD    0xAD
#define KBC_ENABLE_CMD     0xAE
#define KBC_DISABLE_MOUSE  0xA7
#define KBC_ENABLE_MOUSE   0xA8
#define KBC_MOUSE_IF_CHECK 0xA9
#define KBC_WRITE_TO_MOUSE 0xD4

//status register command bits
#define KBC_ST_PARITY   BIT(7)  //parity error
#define KBC_ST_TIMEOUT  BIT(6)  //timeout error
#define KBC_ST_AUX      BIT(5)  //if enabled data is coming from mouse
#define KBC_ST_IBF      BIT(1)  //input buffer full
#define KBC_ST_OBF      BIT(0)  //output buffer full

//interrupt request lines
#define KBC_IRQ_KEYBOARD 1
#define KBC_IRQ_MOUSE    12

//measuring time
#define DELAY_US 20000

//scan codes
#define TWO_BYTE_SCNCODE_PREFIX 0xE0
#define ESC 0x81

#endif
