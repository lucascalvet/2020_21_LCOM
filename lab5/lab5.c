// IMPORTANT: you must include the following line in all your C files
#include "video_gr.h"
#include "sprite.h"
#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
//#include "video_gr.h"
//#include "sprite.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) { //Fully working all tests passed
  vg_init(mode);
  sleep(delay);
  if (vg_exit() != OK)
    return 1;
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  vg_init(mode);

  vg_draw_rectangle(x, y, width, height, color);

  if (kbd_interrupt_esc() == 1)
    return 1;

  if (vg_exit() != OK)
    return 1;
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  vg_init(mode);

  draw_rectangle_pattern(first, step, mode, no_rectangles);

  kbd_interrupt_esc();

  if (vg_exit() != OK)
    return 1;
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  vg_init(VBE_INDEXED_1024_MODE);                                            

  xpm_image_t img;
  uint8_t *map;

  //gets the pixmap from the XPM
  map = xpm_load(xpm, XPM_INDEXED, &img); //only working for indexed so colors only have 1 byte, therefore the color_assembler here it's not actually necessary

  int map_index = 0;

  //draws pixmap
  for (int row = 0; row < img.height; row++) {
    for (int col = 0; col < img.width; col++) {
      draw_pixel(x + col, y + row, color_assembler(map, &map_index));
    }
  }

  kbd_interrupt_esc();

  if (vg_exit() != OK)
    return 1;
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) { //exit criteria (final x and y or esc) possible use functions pointers too
  vg_init(VBE_INDEXED_1024_MODE);

  //just a test to sprite file
  Sprite *sp = create_sprite(xpm, 0, 0);

  move_sprite(sp, 100, 100, 0, 1);

  kbd_interrupt_esc();

  if (vg_exit() != OK)
    return 1;
  return 0;
}

int(video_test_controller)() {
  //test to get_vbe_current_mode()
  /*
  vg_init(0x105);

  unsigned short cur = get_vbe_current_mode(); 

  vg_exit();

  printf("Current mode: %x ", cur);
  */

  //actual video_test_controller function
  vg_vbe_contr_info_t controller_info;

  get_vbe_controller_info(&controller_info);

  vg_display_vbe_contr_info(&controller_info);  //prints controller information to the screen

  return 0;
}
