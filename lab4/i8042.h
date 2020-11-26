#ifndef i8042_H
#define i8042_H

//kbc addresses
#define KBC_OUT_BUF   0x60
#define KBC_INPUT_BUF 0x60  
#define KBC_ST_REG    0x64  //it has information about the state of the keyboard
#define KBC_CMD_BUF   0x64  

//kbc status register commmands
#define KBC_READ_CMD       0x20
#define KBC_WRITE_CMD      0x60
#define KBC_SELF_TEST_CMD  0xAA
#define KBC_IF_TEST_CMD    0xAB
//ADDED IN LAB4
#define KBC_DISABLE_CMD    0xAD
#define KBC_ENABLE_CMD     0xAE
#define KBC_DISABLE_MOUSE  0xA7
#define KBC_ENALBE_MOUSE   0xA8
#define KBC_MOUSE_IF_CHECK 0xA9
#define KBC_WRITE_TO_MOUSE 0xD4

//status register command   
#define KBC_ST_PARITY      BIT(7)  //parity error
#define KBC_ST_TIMEOUT     BIT(6)  //timeout error
#define KBC_ST_AUX         BIT(5)  //mouse data
#define KBC_ST_IBF         BIT(1)  //input buffer full
#define KBC_ST_OBF         BIT(0)  //output buffer full

//interrupt request
#define KBC_IRQ        1  //keyboard interrutp request line
#define KBC_IRQ_MOUSE  12

//measuring time
#define DELAY_US    20000

//scan codes
#define TWO_BYTE_SCNCODE_PREFIX 0xE0
#define ESC  0x81
//#define ESC 0

#endif
