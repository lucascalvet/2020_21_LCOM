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
#define NUM_TRIES 5

//scan codes
#define TWO_BYTE_SCNCODE_PREFIX 0xE0
#define ESC 0x81
#define BACKSPACE 0x0e
#define KEY_MAKE_W 0x11
#define KEY_BREAK_W 0x91
#define KEY_MAKE_A 0x1e
#define KEY_BREAK_A 0x9e
#define KEY_MAKE_S 0x1f
#define KEY_BREAK_S 0x9f
#define KEY_MAKE_D 0x20
#define KEY_BREAK_D 0xa0

#define KEY_MAKE_ARROW_UP 0x48
#define KEY_BREAK_ARROW_UP 0xc8
#define KEY_MAKE_ARROW_LEFT 0x4b
#define KEY_BREAK_ARROW_LEFT 0xcb
#define KEY_MAKE_ARROW_DOWN 0x50
#define KEY_BREAK_ARROW_DOWN 0xd0
#define KEY_MAKE_ARROW_RIGHT 0x4d
#define KEY_BREAK_ARROW_RIGHT 0xcd

#define KEY_MAKE_I 0x17
#define KEY_MAKE_J 0x24
#define KEY_MAKE_K 0x25
#define KEY_MAKE_L 0x26
#define KEY_BREAK_I 0x97
#define KEY_BREAK_J 0xa4
#define KEY_BREAK_K 0xa5
#define KEY_BREAK_L 0xa6

//http://users.utcluj.ro/~baruch/sie/labor/PS2/Scan_Codes_Set_1.htm

//leaved this here because it's an alternative key set if we need
            /*
            if (data == KEY_MAKE_I) keys_waternix[0] = true;
            if (data == KEY_MAKE_J) keys_waternix[1] = true;
            if (data == KEY_MAKE_K) keys_waternix[2] = true;
            if (data == KEY_MAKE_L) keys_waternix[3] = true;
            if (data == KEY_BREAK_I) keys_waternix[0] = false;
            if (data == KEY_BREAK_J) keys_waternix[1] = false;
            if (data == KEY_BREAK_K) keys_waternix[2] = false;
            if (data == KEY_BREAK_L) keys_waternix[3] = false;
            */
            //printf("\nkeys = {%x, %x, %x, %x}", keys_firemi[0], keys_firemi[1], keys_firemi[2], keys_firemi[3]);



#endif
