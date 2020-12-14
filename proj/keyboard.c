//#include <minix/sysutil.h> //for tickdelay(micros_to_ticks(DELAY_US));

#include "i8042.h"
#include "utils.h"
#include "keyboard.h"
#include "kbc.h"

uint8_t data;
uint8_t bytes[2];

uint8_t st;
uint8_t temp;
int hook_id_kbd;

int (keyboard_subscribe_int)(uint8_t *bit_no){
    hook_id_kbd = 1;
    *bit_no = hook_id_kbd;
    if(sys_irqsetpolicy(KBC_IRQ_KEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd) != OK) 
        return 1;
    return 0;
}

int (keyboard_unsubscribe_int)(){
    if(sys_irqrmpolicy(&hook_id_kbd) != OK)
        return 1;
    return 0;
}
/*
int (keyboard_writing_cmd)(int reg, int cmd, int tries){
    for(int i = 0; i < tries; i--) {  //loop while 8042 input buffer is not empty max [tries] times, then exit
        if(util_sys_inb(KBC_STATUS_REG, &st) != OK)
            return 1;

        if((st & KBC_ST_IBF) == 0 ) {
            sys_outb(reg, cmd);
            return 0;
        }
        //tickdelay(micros_to_ticks(DELAY_US)); 
    }
    return 0;
}

int (keyboard_reading_cmd)(int tries){

    for(int i = 0; i < tries; i--) {
        if(util_sys_inb(KBC_STATUS_REG, &st) != OK)
            return -1;
if(true){
        //if(st & KBC_ST_OBF) {
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
*/
void (keyboard_ih)(){  //keyboard interrupt handler
    bool make_identifier;

    if(kbc_read_data(&data) == 0){
        
        make_identifier = data >> 7; //shitf value to stay with make
        
        bytes[0] = data;        

        if(data != TWO_BYTE_SCNCODE_PREFIX){
            kbd_print_scancode(make_identifier, 1, bytes);
        }
        else{
            kbc_read_data(&data);

            make_identifier = data >> 7; //shitf value to stay with make

            bytes[1] = data;
            kbd_print_scancode(make_identifier, 2, bytes);
        }
    }
}
