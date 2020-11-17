#include <lcom/lcf.h>
#include "video_gr.h"
#include <minix/driver.h>
#include <sys/mman.h>

static void *video_mem;  

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

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes) vram_base;	
  mr.mr_limit = mr.mr_base + vram_size;  

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86));
  reg86.intno = 0x10;
  reg86.ah = VBE_AH;
  reg86.al = VBE_SET_MODE;
  reg86.bx = mode;
  if(sys_int86(&reg86) != OK) {
    printf("\tvg_init(): sys_int86() failed \n");
    return NULL;
  }
  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  return video_mem;
}
