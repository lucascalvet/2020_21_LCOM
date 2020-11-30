#include "i8042.h"
#include "utils.h"
#include "mouse.h"
#include "ps2.h"

int hook_id_mouse;
uint8_t st;
uint8_t data;

int (mouse_subscribe_int)(uint8_t *bit_no){
    *bit_no = hook_id_mouse;

    if(sys_irqsetpolicy(KBC_IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse) != OK) 
        return 1;
    return 0;
}

int (mouse_unsubscribe_int)(){
    if(sys_irqrmpolicy(&hook_id_mouse) != OK)
        return 1;
    return 0;
}

int (kbc_writing_cmd)(int reg, int cmd, int tries){
    for(int i = 0; i < tries; i--) {  //loop while 8042 input buffer is not empty max [tries] times, then exit
        if(util_sys_inb(KBC_ST_REG, &st) != OK)
            return -1;

        if((st & KBC_ST_IBF) == 0) {
            sys_outb(reg, cmd);
            return 0;
        }
        //tickdelay(micros_to_ticks(DELAY_US)); 
    }
    return 0;
}

int (kbc_reading_cmd)(uint8_t st, int tries){

    for(int i = 0; i < tries; i--) {
        if(util_sys_inb(KBC_ST_REG, &st) != OK)
            return -1;

        if(st & KBC_ST_OBF) {
            if(util_sys_inb(KBC_OUT_BUF, &data) != OK)
                return -1;

        if ((st & (KBC_ST_PARITY  | KBC_ST_TIMEOUT)) == 0 )
            return 0;
        else
            return -1;
        }
    //tickdelay(micros_to_ticks(DELAY_US));
    }
    return -1;
}

int (mouse_write_cmd)(uint8_t cmd){
    if(kbc_writing_cmd(KBC_CMD_BUF, KBC_WRITE_TO_MOUSE, 3) != OK)
        return -1;
    if(kbc_reading_cmd(st, 3) != OK)
        return -1;
    if(kbc_writing_cmd(KBC_CMD_BUF, cmd, 3) != OK)
        return -1;
    if(mouse_read_cmd(st) != OK)  //should discard the byte
        return -1;

    return 0;
}

int (mouse_read_cmd)(uint8_t st){

    if(kbc_reading_cmd(st, 3) != OK)
        return -1;

    if(data & MOUSE_ACK)
        return 0;
    else
        return -1; 
}

int (enable_data_reporting)(){
    if(mouse_write_cmd(PS2_ENABLE_DT_REP) != OK)
        return -1;

    return 0;
}

void (assemble_bytes)(int *counter, uint8_t pac[3]){  //assemble bytes in pac[]
    if((*counter) == 0){  //data & FIRST_BYTE_CHECK & 
        //printf("assmeble0: %d\n", (*counter));
        pac[0] = data;
    }
    if((*counter) == 1){
        //printf("assmeble1: %d\n", (*counter));
        pac[1] = data;
    }
    if((*counter) == 2){
        //printf("assmeble2: %d\n", (*counter));
        pac[2] = data;
    }
    (*counter)++;
}

void (build_packet)(int *counter, uint8_t pac[3], uint32_t *cnt){  //builds the struct packet to use 
    if((*counter) == 3){
        //printf("in couter 3\n");
        struct packet pac_struct;

        for(int i = 0; i < 3; i++){
            pac_struct.bytes[i] = pac[i];
        }

        bool rb_ = pac[0] & PACKET_RB;
        bool mb_ = pac[0] & PACKET_MB;
        bool lb_ = pac[0] & PACKET_LB;
        bool x_ov_ = pac[0] & PACKET_X_OVFL;
        bool y_ov_ = pac[0] & PACKET_Y_OVFL;

        pac_struct.rb = rb_;
        pac_struct.mb = mb_;
        pac_struct.lb = lb_;
        pac_struct.x_ov = x_ov_;
        pac_struct.y_ov = y_ov_;

        uint8_t msb_x = pac[0] << 3;
        msb_x >>= 7;

        uint8_t msb_y = pac[0] << 2;
        msb_y >>= 7;

        uint16_t x, y;
        uint16_t mask = 0xff00; 

        printf("Delta x (8bits): %x \n", pac[1]);
        if(msb_x){
            x = pac[1] | mask;
            printf("Delta x (16bits): %x \n", x);
        }
        else
            x = pac[1];

        if(msb_y)
            y = pac[2] | mask;
        else
            y = pac[2];        

        pac_struct.delta_x = x;
        pac_struct.delta_y = y;

        //printf("before print packet\n");
        
        mouse_print_packet(&pac_struct);

        //printf("after print packet\n");

        (*cnt)--;
        (*counter) = 0;
    }
}

void (mouse_ih)(){  //mouse interrupt handler
    kbc_reading_cmd(st, 3);
}

int (output_buff_flush)(){
  //only needs to read the out buffer one time and the byte is flushed
    if(kbc_reading_cmd(st, 3) != OK)
        return -1;

    return 0; 
}
