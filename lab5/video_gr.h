#ifndef VIDEO_GR_H
#define VIDEO_GR_H

#include "i8042.h"
#include "keyboard.h"
#include <lcom/lcf.h>
#include <minix/driver.h>
#include <sys/mman.h>

//VBE standard Instruction BIOS interface
#define VBE_INTERRUPT_INSTRUCTION 0x10

//VBE functions
#define VBE_GET_CONTROLLER_INFO_FUNCTION 0x4F00 //gets VBE controller info
#define VBE_GET_MODE_INFO_FUNCTION 0x4F01       //gets the info of a mode
#define VBE_SET_MODE_FUNCTION 0x4F02            //sets graphics mode
#define VBE_GET_CURRENT_MODE_FUNCTION 0x4F03    //gets current mode function

//Activate linear frame buffer model, when setting VBE mode
#define VBE_LINEAR_ACTIVATE BIT(14)

//VBE and minix video modes
#define VBE_TEXT_MODE 0x03
#define VBE_INDEXED_1024_MODE 0x105
#define VBE_DIRECT_640_MODE 0x110
#define VBE_DIRECT_800_MODE 0x115
#define VBE_DIRECT_1280_MODE 0x11A
#define VBE_DIRECT_1152_MODE 0x14C

//translates bits to bytes to prevent errors on manual translation
int (bits_to_bytes)();

//print errors that may happen in sys_int86 call 
void(sys_int86_print_errors)(const reg86_t reg86);

//gets VBE mode info
void(get_vbe_mode_info)(uint16_t mode, vbe_mode_info_t *mode_info);

//gets VBE current mode
int(get_vbe_current_mode)();

//gets VBE controller information
void(get_vbe_controller_info)(vg_vbe_contr_info_t *controller_info);

//initializes static mode dependet variables with respetive values
void(vbe_mode_info_variables_init)(vbe_mode_info_t *mode_info);

//initializes the graphics card with given mode, using VBE
void *(vg_init)(uint16_t mode);

//assembles color bytes of an map starting at given position, according to nº bits_per_pixel
uint32_t (color_assembler)(const uint8_t *map, int *map_position);

//draws a pixel in screen
void(draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

//draws a pattern with rectangles
int(draw_rectangle_pattern)(uint32_t first, uint8_t step, uint16_t mode, uint8_t no_rectangles);

//waits for ESC to be pressed
int(kbd_interrupt_esc)();

#endif //VIDEO_GR_H
