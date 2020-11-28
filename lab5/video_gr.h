//VBE standard Instruction BIOS interface
#define VBE_INTERRUPT_INSTRUCTION 0x10

//VBE functions
#define VBE_CONTROLLER_FUNCTION 0x4F00       //gets vbe controller info
#define VBE_GET_MODE_INFO_FUNCTION 0x4F01    //gets the info of a mode
#define VBE_SET_MODE_FUNCTION 0x4F02         //sets graphics mode
#define VBE_GET_CURRENT_MODE_FUNCTION 0x4F03 //gets current mode function

//initializes the VBE with given mode
void *(vg_init)(uint16_t mode);

//draws a pixel in screen
void(draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

//waits for ESC to be pressed
int(kbd_interrupt_esc)();

//draws a pattern with rectangles
int(draw_rectangle_pattern)(uint32_t first, uint8_t step, uint16_t mode, uint8_t no_rectangles);
