#include "video_gr.h"
#include "i8042.h"
#include "keyboard.h"
#include <lcom/lcf.h>
#include <minix/driver.h>
#include <sys/mman.h>

static void *video_mem;

static unsigned h_res;          // Horizontal resolution in pixels
static unsigned v_res;          // Vertical resolution in pixels
static unsigned bits_per_pixel; // Number of VRAM bits per pixel
uint8_t red_mask_size = 0;
uint8_t green_mask_size = 0;
uint8_t blue_mask_size = 0;
uint8_t red_field_position = 0, green_field_position = 0, blue_field_position = 0;

/**
  * @brief initiates the graphical controller with a video mode, using vbe 
  * @param mode the video mode to be set in the graphics controller
  * @return the VRAM's virtual address (of the first physical memory position) on which the physical address range was mapped
  */
void*(vg_init)(uint16_t mode) {

  struct minix_mem_range mr;
  unsigned int vram_base; // VRAM's physical addresss
  unsigned int vram_size; // VRAM's size, but you can use the frame-buffer size, instead
  int r;

  vbe_mode_info_t info;
  vbe_get_mode_info(mode, &info);
  vram_base = info.PhysBasePtr;
  vram_size = info.XResolution * info.YResolution * info.BitsPerPixel / 8;

  //TODO: change all this to a new get vbe mode function
  h_res = info.XResolution;
  v_res = info.YResolution;
  bits_per_pixel = info.BitsPerPixel;
  red_mask_size = info.RedMaskSize;
  green_mask_size = info.RedMaskSize;
  blue_mask_size = info.BlueMaskSize;
  red_field_position = info.RedFieldPosition;
  green_field_position = info.GreenFieldPosition;
  blue_field_position = info.BlueFieldPosition;

  //Allow memory mapping
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  //Mapping memory
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86)); //cleanning registers previous to function call to avoid errors
  reg86.ax = VBE_SET_MODE_FUNCTION;
  reg86.bx = 1 << 14 | mode; //bit 14: linear frame buffer model set
  reg86.intno = VBE_INTERRUPT_INSTRUCTION;

  //making kernell call in real mode (momentaneous switch from minix protected mode)
  if (sys_int86(&reg86) != OK) {
    printf("\tvg_init(): sys_int86() failed \n");
    return NULL;
  }

  if (video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  return video_mem;
}

/**
 * @brief changes a pixel color in coordinates x and y of screen
 * @param x the x coordinate of screen from left to right
 * @param y the y coordinate of screen from top to bottom
 * @param color the color to be set in pixel
 * @return
 * 
 */
void(draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

  char *pointer = video_mem; //pointer to video memory adress

  pointer += x + h_res * y * (bits_per_pixel / 8); // gets correct position of memory map to change according to x, y and bits_per_pixel

  if (x < h_res && y < v_res)
    *pointer = color; //if x and y exceeds the window size doesn't change anything
}

/**
 * @brief draws an horizontal line in screen
 * @param init_x the starting x coordinate of the line
 * @param init_y the starting x coordinate of the line
 * @param final_x the final x coordinate of the line
 * @param final_y the final x coordinate of the line
 * @param color the color of the line
 * @return 0 if everything ok, 1 otherwise
 */
int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (int i = 0; i < len; i++) {
    ///if(x + i  > len) return 1;
    draw_pixel(x + i, y, color);
  }
  return 0;
}

/**
 * @brief draws an rectangle in screen
 * @param init_x the upper corner x coordinate of rectangle 
 * @param init_y the upper corner y coordinate of the rectangle
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @param color the color of the rectangle
 * @return 0 if everything ok, 1 otherwise
 */
int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (int i = 0; i < height; i++) {
    if (vg_draw_hline(x, y + i, width, color) == 1)
      return 1;
  }

  return 0;
}

int(draw_rectangle_pattern)(uint32_t first, uint8_t step, uint16_t mode, uint8_t no_rectangles) { //pattern of matrix of n x n rectangles
  int width = h_res / no_rectangles;
  int height = v_res / no_rectangles;
  int color = 0;

  int x = 0, y = 0;
  for (int row = 0; row < no_rectangles; row++) {
    for (int col = 0; col < no_rectangles; col++) {

      if (mode == 0x105) { //if color is indexed
        color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
      }
      else { //if color is direct
        uint8_t red = ((first >> red_field_position) & ((1 << red_mask_size) - 1) + col * step) % (1 << red_mask_size);
        uint8_t green = ((first >> green_field_position) & ((1 << green_mask_size) - 1) + row * step) % (1 << green_mask_size);
        uint8_t blue = ((first >> blue_field_position) & ((1 << blue_mask_size) - 1) + (col + row) * step) % (1 << blue_mask_size);

        color = red | green << green_field_position | blue << blue_field_position;
      }

      vg_draw_rectangle(x + (width * col), y + (height * row), width, height, color);
    }
  }
  return 0;
}

/**
 * @brief waits for user to input ESC key
 * @return 0 when no erros ocurred
 * 
 */
int(kbd_interrupt_esc)() {
  int ipc_status;
  message msg;

  uint8_t bit_no2;
  keyboard_subscribe_int(&bit_no2);

  uint64_t kbd_int_bit = BIT(bit_no2);

  int r;

  while (data != ESC) {

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
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
