#define VBE_AH 0x4F
#define VBE_SET_MODE 0x02
#define VBE_INTERRUPT_INSTRUCTION 0x10  //standard Instruction BIOS interface
#define VBE_SET_VIDEO_MODE 0x00
#define VBE_TEXT_MODE 0x03   //text mode is a standard CGA mode

//initializes the vbe with given mode
void * (vg_init)(uint16_t mode);

/*
//sets the minix vbe back to text mode
int (vg_exit)(void);
*/

//draw pixel in screen 
void (draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

//draws a line in screen
void (draw_line)(uint16_t init_x, uint16_t init_y, uint16_t final_x, uint16_t final_y , uint32_t color);

//waits for esc
int(kbd_interrupt_esc)();

//creates a sprite object
void (create_sprite)();

//frees memory allocated by the create_sprite  
void (destroy_sprite)();




//use buffer auxiliar in the project!! (have the map in a buffer and at every cycle do a memcopy to vram)
