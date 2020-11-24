#include <lcom/lcf.h>
#include "video_gr.h"
#include <minix/driver.h>
#include <sys/mman.h>
#include "keyboard.h"
#include "i8042.h"

static void *video_mem; 
 
static unsigned h_res;	         /* Horizontal resolution in pixels */
static unsigned v_res;	         /* Vertical resolution in pixels */
static unsigned bits_per_pixel;  /* Number of VRAM bits per pixel */ 

void * (vg_init)(uint16_t mode){

  /* frame-buffer VM address (static global variable*/

  struct minix_mem_range mr;
  unsigned int vram_base;  /* VRAM's physical addresss */
  unsigned int vram_size;  /* VRAM's size, but you can use the frame-buffer size, instead */
  int r;				    

  /* Use VBE function 0x01 to initialize vram_base and vram_size */
  vbe_mode_info_t info;
  vbe_get_mode_info(mode, &info);
  vram_base = info.PhysBasePtr;
  vram_size = info.XResolution * info.YResolution * info.BitsPerPixel/8;

  //getting the vbe mode info
  h_res = info.XResolution;
  v_res = info.YResolution;
  bits_per_pixel = info.BitsPerPixel;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size;  

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
/* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86));
  reg86.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
  reg86.bx = 1<<14 | mode; // set bit 14: linear framebuffer
  reg86.intno = VBE_INTERRUPT_INSTRUCTION;  // interrupt instruction, in real mode, and the function parameters are passed in the CPU registers.
  //reg86.ah = VBE_AH;  //thus distinguishing it from the standard VGA BIOS functions
  //reg86.al = VBE_SET_MODE;  //VBE function being called is specified in the AL register
  //reg86.bx = mode;   //Bit 14 of the BX register should be set, in order to set the linear frame buffer model ?
  if(sys_int86(&reg86) != OK) {
    printf("\tvg_init(): sys_int86() failed \n");
    return NULL;
  }

  if(video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  return video_mem;
}

/*
int (vg_exit)(void){
  struct reg86 r86;
    
  memset(&r86, 0, sizeof(r86));	// zero the structure 

  r86.intno = VBE_INTERRUPT_INSTRUCTION; 
  r86.ah = VBE_SET_VIDEO_MODE;    
  r86.al = VBE_TEXT_MODE;   

  if( sys_int86(&r86) != OK ) {
    printf("\tvg_exit(): sys_int86() failed \n");
    return 1;
  }
  return 0;
}
*/


void (draw_pixel)(uint16_t x, uint16_t y, uint32_t color){

  char* pointer = video_mem; //pointer to video memory adress 

  pointer += x + h_res * y * (bits_per_pixel / 8);  // gets correct position of memory map to change according to x, y and bits_per_pixel

  if(x < h_res && y < v_res) *pointer = color;  //if x and y exceeds the window size doesn't change anything
}

//not used yet
void (draw_line)(uint16_t init_x, uint16_t init_y, uint16_t final_x, uint16_t final_y , uint32_t color){
  int dx = final_x - init_x;  //x distance between initial and final x value
  int dy = final_y - init_y;  //y distance between initial and final y value
  int iterations = 0;  //number of iterations needed to draw the line

  int x_increment = dx / iterations;
  int y_increment = dy / iterations;

  if(abs(dx) > abs(dy)) iterations = dx;  //chose the greatest of the distances
  else iterations = dy;

  for(int i = 0; i < iterations; i++){   //draw the pixels through the coordinates wanted
    draw_pixel(init_x + x_increment, init_y + y_increment, color);
  }
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
  for(int i = 0; i < len; i++){
    if(x + i  > len) return 1;
    draw_pixel(x + i, y, color);
  }
  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for(int i = 0; i < height; i++){
    if(vg_draw_hline(x, y + i, width, color) == 1) return 1;
  }

  return 0;
}

void (create_sprite)(){
}

void (destroy_sprite)(){
}

int(kbd_interrupt_esc)() {
  int ipc_status;
  message msg;

  uint8_t bit_no2;
  keyboard_subscribe_int(&bit_no2);

  uint64_t kbd_int_bit = BIT(bit_no2);

  int r;

  while(data != ESC){

    if((r = driver_receive(ANY, &msg, &ipc_status)) != OK){
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)){
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & kbd_int_bit) {
            kbc_ih();
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
  }

  keyboard_unsubscribe_int();

  return 0;
}


//test_rectangle 
//shall change the video mode to that in its argument
//map the video memory to the process' address space and 
//draw a rectangle. 

//TO DO functions:
//create sprite  (xpm_img img, xpm_load)
//destrory sprite (frees the memory allocated to use sprite)
