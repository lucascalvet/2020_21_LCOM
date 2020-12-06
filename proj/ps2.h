/* KBC related to PS2 mouse constants */

#ifndef PS2_H
#define PS2_H

//commands passed as arguments of command 0xD4 to KBC
#define PS2_RESET           0xFF
#define PS2_RESEND          0xFE
#define PS2_SET_DEFAULTS    0xF6
#define PS2_DISABLE_DT_REP  0xF5
#define PS2_ENABLE_DT_REP   0xF4
#define PS2_SET_SMPL_RATE   0xF3
#define PS2_SET_REMOTE_MODE 0xF0
#define PS2_READ_DT         0xEB
#define PS2_SET_STREAM_MODE 0xEA
#define PS2_ST_REQUEST      0xE9
#define PS2_SET_RESOLUTION  0xE8
#define SET_SCALING_2_1     0xE7
#define SET_SCALING_1_1     0xE6

//mouse controller acknowledgment bytes
#define MOUSE_ACK    0xFA  //everything OK
#define MOUSE_NACK   0xFE  //invalid byte
#define MOUSE_ERROR  0xFC  //second consecutive invalid byte

//to see if codition of first byte is true
#define FIRST_BYTE_CHECK  BIT(3)

//first byte of mouse packet structure
#define PACKET_LB           BIT(0)
#define PACKET_RB           BIT(1)
#define PACKET_MB           BIT(2)
#define PACKET_MSB_Y_DELTA  BIT(5)
#define PACKET_MSB_X_DELTA  BIT(4)
#define PACKET_X_OVFL       BIT(6)
#define PACKET_Y_OVFL       BIT(7)

#endif //PS2_H
