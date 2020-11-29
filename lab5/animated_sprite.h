#include "sprite.h"

typedef struct {
  Sprite *sp;     //standard sprite
  int speed;     //number of frames per pixmap
  int cur_aspeed; //number of frames left to next change
  int num_fig;    //number of pixmaps
  int cur_fig;    //current pixmap
  char **map;     //array of pointers to pixmaps
} AnimatedSprite;

//creates an AnimatedSprite
AnimatedSprite* (create_animated_sprite)(Sprite *sp, int num_fig, int speed, char **map[]);

//deletes AnimatedSprite
void(delete_animated_sprite)();

//erases AnimatedSprite from screen
void(erase_animated_sprite)();

//moves sprite in screen
void(move_animated_sprite)();

//animates the AnimatedSprite
void(animate_sprite)();

//stops AnimatedSprite from beeing animate, staying at standard Sprite
void(stop_animated_sprite)();