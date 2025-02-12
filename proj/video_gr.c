#include "video_gr.h"
#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

static uint8_t *video_mem;      //virtual vram address
static uint8_t *secondary_buffer; //secondary buffer for double buffering
static unsigned int vram_size;
//static bool display_start; //false if at the beggining, true if at the middle

static unsigned h_res;          //horizontal resolution in pixels
static unsigned v_res;          //vertical resolution in pixels
static unsigned bits_per_pixel; //number of VRAM bits per pixel
static uint8_t red_mask_size = 0;
static uint8_t green_mask_size = 0;
static uint8_t blue_mask_size = 0;
static uint8_t red_field_position = 0, green_field_position = 0, blue_field_position = 0;

/**
 * @brief converts number of bits to it's correspondant number of bytes
 * @return the number of bytes
 */
int(bits_to_bytes)() {
  unsigned int n_bytes = bits_per_pixel / 8;

  if (bits_per_pixel % 8 != 0) { //when bits are not even the correct number of bytes is passed
    n_bytes += 1;
  }

  return n_bytes;
}

/**
 * @brief print errors that may happen in sys_int86 call 
 * @param reg86 the reg86_t struct that contains the register values
 * @return none
 */
void(sys_int86_print_errors)(const reg86_t reg86) {
  if (reg86.ah == 0x01) {
    printf("Function call failed!\n");
  }
  if (reg86.ah == 0x02) {
    printf("Function is not supported in the current hardware configuration!\n");
  }
  if (reg86.ah == 0x03) {
    printf("Function call invalid in current video mode!\n");
  }
  if (reg86.al != 0x4F) {
    printf("Function not supported!\n");
  }
}

/**
 * @brief gets the information about vbe mode given
 * @param mode the mode to get information of
 * @param mode_info the vbe_mode_info_t struct to fill with mode info
 * @return none
 */
void(get_vbe_mode_info)(uint16_t mode, vbe_mode_info_t *mode_info) {
  mmap_t mmap; //memory info of vbe_mode_info_t struct info

  lm_alloc(sizeof(vbe_mode_info_t), &mmap);


  //vbe_get_info(mmap.phys);

  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); //cleaning registers previous to function call to avoid errors

  reg86.intno = VBE_INTERRUPT_INSTRUCTION;
  reg86.ax = VBE_GET_MODE_INFO_FUNCTION;
  reg86.cx = mode;
  reg86.es = PB2BASE(mmap.phys); //passing base info struct address
  reg86.di = PB2OFF(mmap.phys);  //passsing adress offset of info struct

  //making kernell call in real mode (momentaneous switch from minix protected mode)
  if (sys_int86(&reg86) != OK) {
    printf("\tget_vbe_mode_info(): sys_int86() failed \n");
  }

  sys_int86_print_errors(reg86);

  *mode_info = *((vbe_mode_info_t *) mmap.virt); //assigning the info pointer to the one retrieve by kernell call (as well as doing the necessary cast)

  lm_free(&mmap);
}

/**
 * @brief gets the vbe current mode
 * @return the current vbe mode in hexadecimal
 */
int(get_vbe_current_mode)() {
  unsigned short current_mode = 0; //to old the current mode returned in bx register

  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); //cleanning registers previous to function call to avoid errors

  reg86.intno = VBE_INTERRUPT_INSTRUCTION;
  reg86.ax = VBE_GET_CURRENT_MODE_FUNCTION;

  //making kernell call in real mode (momentaneous switch from minix protected mode)
  if (sys_int86(&reg86) != OK) {
    printf("\tget_vbe_current_mode(): sys_int86() failed \n");
  }

  sys_int86_print_errors(reg86);

  current_mode = reg86.bx;

  return current_mode;
}

/**
 * @brief gets VBE controller information
 * @param controller_info struct that will have the controller information
 * @return none
 */
void(get_vbe_controller_info)(vg_vbe_contr_info_t *controller_info) {
  mmap_t mmap; //memory info of vg_vbe_contr_info_t struct info

  lm_alloc(sizeof(vg_vbe_contr_info_t), &mmap);

  controller_info->VBESignature[0] = 'V';
  controller_info->VBESignature[1] = 'B';
  controller_info->VBESignature[2] = 'E';
  controller_info->VBESignature[3] = '2';

  mmap.virt = controller_info;

  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); //cleanning registers previous to function call to avoid errors

  reg86.intno = VBE_INTERRUPT_INSTRUCTION;
  reg86.ax = VBE_GET_CONTROLLER_INFO_FUNCTION;
  reg86.es = PB2BASE(mmap.phys); //passing base info struct address
  reg86.di = PB2OFF(mmap.phys);  //passsing adress offset of info struct

  //making kernell call in real mode (momentaneous switch from minix protected mode)
  if (sys_int86(&reg86) != OK) {
    printf("\tget_vbe_controller_info(): sys_int86() failed \n");
  }

  sys_int86_print_errors(reg86);

  *controller_info = *((vg_vbe_contr_info_t *) mmap.virt); //assigning the info pointer to the one retrieve by kernell call (as well as doing the necessary cast)

  lm_free(&mmap);
}

/**
 * @brief initializes static mode dependet variables with respetive values after a vbe get mode info call
 * @param mode_info vbe_mode_info_t struct with vbe mode info
 * @return none
 */
void(vbe_mode_info_variables_init)(vbe_mode_info_t *mode_info) {
  h_res = mode_info->XResolution;
  v_res = mode_info->YResolution;
  bits_per_pixel = mode_info->BitsPerPixel;
  red_mask_size = mode_info->RedMaskSize;
  green_mask_size = mode_info->RedMaskSize;
  blue_mask_size = mode_info->BlueMaskSize;
  red_field_position = mode_info->RedFieldPosition;
  green_field_position = mode_info->GreenFieldPosition;
  blue_field_position = mode_info->BlueFieldPosition;
}

/**
  * @brief initiates the graphical controller with a video mode, using vbe 
  * @param mode the video mode to be set in the graphics controller
  * @return the VRAM's virtual address (of the first physical memory position) on which the physical address range was mapped
  */
void *(vg_init)(uint16_t mode) {

  struct minix_mem_range mr;
  unsigned int vram_base; // VRAM's physical addresss
  int r;
  //display_start = false;
  //set_display_start(0);

  vbe_mode_info_t mode_info;
  get_vbe_mode_info(mode, &mode_info);
  vram_base = mode_info.PhysBasePtr;
  //vram_size = 2 * mode_info.XResolution * mode_info.YResolution * mode_info.BitsPerPixel / 8;
  vram_size = mode_info.XResolution * mode_info.YResolution * mode_info.BitsPerPixel / 8;

  vbe_mode_info_variables_init(&mode_info);

  //Allow memory mapping
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  //Mapping memory
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  //Allocating memory for secondary buffer
  secondary_buffer = malloc(vram_size);
  memset(secondary_buffer, 0, vram_size);


  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); //cleanning registers previous to function call to avoid errors

  reg86.intno = VBE_INTERRUPT_INSTRUCTION;
  reg86.ax = VBE_SET_MODE_FUNCTION;
  reg86.bx = VBE_LINEAR_ACTIVATE | mode;

  //making kernell call in real mode (momentaneous switch from minix protected mode)
  if (sys_int86(&reg86) != OK) {
    printf("\tvg_init(): sys_int86() failed \n");
    return NULL;
  }

  sys_int86_print_errors(reg86);

  if (video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  return video_mem;
}

/**
 * @brief Set the first scan line to be displayed
 * @param pixel_position the first pixel to be displayed
 */
/*void(set_display_start)(uint16_t pixel_position) {
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); //cleaning registers previous to function call to avoid errors

  reg86.intno = VBE_INTERRUPT_INSTRUCTION;
  reg86.ax = VBE_GET_MODE_INFO_FUNCTION;
  reg86.cx = pixel_position;

  //making kernell call in real mode (momentaneous switch from minix protected mode)
  if (sys_int86(&reg86) != OK) {
    printf("\tget_vbe_mode_info(): sys_int86() failed \n");
  }
}*/

/**
 * @brief assembles color bytes of an map starting at given position, according to nº bytes per pixel, and changes to next position map_position
 * @param map pointer to the pixmap with the pixel colors
 * @param map_position pointer to the starting position of pixmap to assemble the color from
 * @return the assembled color
 */
uint32_t(color_assembler)(const uint8_t *map, int *map_position) {
  uint32_t color = 0;

  for (int i = 0; i < bits_to_bytes(); i++) {
    color <<= 8;
    color |= map[*map_position + i];
  }

  *map_position += bits_to_bytes();

  return color;
}

/**
 * @brief changes the place of red and blue, because of the image converter format wich is BGR
 * @param color the color to be converted
 * @return the converted color
 */
uint32_t(convert_BGR_to_RGB)(int color) {
  int red_mask = 0x0000FF;
  int green_mask = 0x00FF00;
  int blue_mask = 0xFF0000;

  int red = color & red_mask;
  red <<= 16;

  int green = color & green_mask;

  int blue = color & blue_mask;
  blue >>= 16;

  color = red | green | blue;

  return color;
}

/**
 * @brief returns the color at a certain address in the vram (by providing the coordinates)
 * @param x the x coordinate of screen from left to right
 * @param y the y coordinate of screen from top to bottom
 * @return the color in the desired position, -1 if no valid position given
 */
uint32_t(vram_get_color_by_coordinates)(uint16_t x, uint16_t y) {
  uint32_t color = 0;

  uint8_t *pointer = secondary_buffer; //pointer to video memory address
  
  /*if (!display_start)
  {
    pointer += h_res * v_res;
  }*/
  

  pointer += (x + h_res * y) * bits_to_bytes(); //gets correct position of memory map to change

  if (x < h_res && y < v_res) { //if x and y exceeds the window size there is no color to be inputed
    color = 0;

    for (int i = 0; i < bits_to_bytes(); i++) {
      color <<= 8;
      color |= *(pointer + i);
    }
  }

  return color;
}

/**
 * @brief returns the color at a certain coordinate in a pixmap
 * @param x the x coordinate of screen from left to right
 * @param y the y coordinate of screen from top to bottom
 * @param pixmap the pixmap to get color from
 * @param width the width of the pixmap
 * @return the respective color
 */
uint32_t(pixmap_get_color_by_coordinates)(uint16_t x, uint16_t y, uint8_t *pixmap, int width) {
  uint32_t color = 0; //TODO: change "none" value

  //if (x <  || y < v_res) //if x and y exceeds the window size there is no color to be inputed
  //return -1;
  //TODO: check for pixmap limits
  uint8_t *pointer = pixmap; //pointer to video memory address

  pointer += (x + width * y) * bits_to_bytes(); //gets correct position of memory map to change
  for (int i = 0; i < bits_to_bytes(); i++) {
    color <<= 8;
    color |= *(pointer + i);
  }

  return color;
}

/**
 * @brief switch displayed frame buffer
 * 
 */
/*
void(switch_display_start)() {
  if (display_start) {
    set_display_start(0);
  }
  else
  {
    set_display_start(h_res * v_res);
  }
  display_start = !display_start;
}
*/

/**
 * @brief Copies the secondary buffer to the vram
 * 
 */
void(copy_buffer_to_vram)() {
  memcpy(video_mem, secondary_buffer, vram_size);
}

/**
 * @brief Copies a pixmap directly to the frame buffer, using memcpy
 * @param map the pixmap to be copied
 */
void(copy_to_buffer)(uint8_t * map) {
  memcpy(secondary_buffer, map, vram_size);
  /*uint8_t *pointer = video_mem;
  if (!display_start) {
    pointer += h_res * v_res;
  }
  memcpy(pointer, map, vram_size / 2);*/
}

/**
 * @brief changes a pixel color in coordinates x and y of screen
 * @param x the x coordinate of screen from left to right
 * @param y the y coordinate of screen from top to bottom
 * @param color the color to be set in pixel
 * 
 */
void(draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

  uint8_t *pointer = secondary_buffer; //pointer to buffer address
  /*
  if (!display_start) {
    pointer += h_res * v_res;
  }
  */

  pointer += (x + h_res * y) * bits_to_bytes(); //gets correct position of memory map to change according to x, y and bytes per pixel

  if (x < h_res && y < v_res) { //if x and y exceeds the window size doesn't change anything
    for (int i = 0; i < bits_to_bytes(); i++) {
      *(pointer + i) = color;
      color >>= 8;
    }
  }
}

/**
 * @brief draws an horizontal line in screen
 * @param x the starting x coordinate of the line
 * @param y the starting x coordinate of the line
 * @param len the pixel length of the line
 * @param color the color of the line
 * @return 0 upon success, non zero otherwise
 */
int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (int i = 0; i < len; i++) {
    draw_pixel(x + i, y, color);
  }
  return 0;
}

/**
 * @brief draws a rectangle in screen
 * @param x the upper left corner x coordinate of rectangle 
 * @param y the upper left corner y coordinate of the rectangle
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @param color the color of the rectangle
 * @return 0 upon success, non zero otherwise
 */
int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (int i = y; i < y + height; i++) {
    vg_draw_hline(x, i, width, color);
  }

  return 0;
}

/**
 * @brief draws a circle in screen at determinated coordinates and specifified color
 * @param left_corner_x the x coordinate of top left corner of the rectangle that embeddeds the circle
 * @param left_corner_y the y coordinate of top left corner of the rectangle that embeddeds the circle
 * @param radius the radius of the circle
 * @param color the color of the circle
 * @return 0 upon success, non zero otherwise
 */
int(vg_draw_circle)(uint16_t left_corner_x, uint16_t left_corner_y, uint16_t radius, uint32_t color) {
  //calculating the circle center
  double center_x = left_corner_x + radius;
  double center_y = left_corner_y + radius;

  //drawing the circle
  for (int row = left_corner_y; row < left_corner_y + radius * 2; row++) {
    for (int col = left_corner_x; col < left_corner_x + radius * 2; col++) {
      if (pow((double) (col - center_x), 2) + pow((double) (row - center_y), 2) - pow((double) radius, 2) <= 0) {
        draw_pixel(col, row, color);
      }
    }
  }

  return 0;
}

/**
 * @brief draws a pattern with rectangles in screen, matrix of [no_rectangles x no_rectangles] 
 * @param first that specifies the first color
 * @param step sepcifies color change step
 * @param mode the vbe mode to be initialized with
 * @param no_rectangles number of rectangles to draw in screen
 */
int(draw_rectangle_pattern)(uint32_t first, uint8_t step, uint16_t mode, uint8_t no_rectangles) { //pattern of matrix of n x n rectangles
  int width = h_res / no_rectangles;
  int height = v_res / no_rectangles;
  uint32_t color = 0;

  int x = 0, y = 0;
  for (int row = 0; row < no_rectangles; row++) {
    for (int col = 0; col < no_rectangles; col++) {

      if (mode == 0x105) { //if color is indexed
        color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
      }
      else { //if color is direct
        uint8_t red = ((first >> red_field_position) + col * step) % (1 << red_mask_size);
        uint8_t green = ((first >> green_field_position) + row * step) % (1 << green_mask_size);
        uint8_t blue = ((first >> blue_field_position) & ((1 << blue_mask_size) - 1) + (col + row) * step) % (1 << blue_mask_size);

        color = (red << red_field_position) | (green << green_field_position) | (blue << blue_field_position);
      }
      vg_draw_rectangle(x + (width * col), y + (height * row), width, height, color);
    }
  }
  return 0;
}
