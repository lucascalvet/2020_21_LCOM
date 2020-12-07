#include <lcom/lcf.h>
#include "video_gr.h"
#include "sprite.h"
#include <lcom/timer.h>
#include <lcom/proj.h>
#include <stdint.h>
#include <stdio.h>

//project headers
#include "xpm_levels.h"
#include "xpm_boal.h"

unsigned timer_counter = 0;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(proj_main_loop)(int argc, char *argv[]){
  printf("Welcome to FireMi & WaterNix!!!\n");
  //proj_demo(VBE_DIRECT_800_MODE, true, false, 1);

  vg_init(VBE_DIRECT_800_MODE);

  Sprite *level1 = create_sprite(xpm_level1, 0, 0);
  draw_sprite(level1);

  Sprite *boal = create_sprite(xpm_boal, 20, 520);
  //draw_sprite(boal);
  move_sprite(boal, 21, 300, 0, -1, level1);

  sleep(10);
  vg_exit();

  delete_sprite(level1);
  delete_sprite(boal);

  return 0;
}
