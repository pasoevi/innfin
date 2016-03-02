#include "actor.h"
#include <stdlib.h>

void init_actor(struct Actor **actor, int x, int y, int ch, TCOD_color_t col, void (*render)(struct Actor *)){
        *actor = malloc(sizeof (struct Actor));
        (*actor)->x = x;
        (*actor)->y = y;
        (*actor)->ch = ch;
        (*actor)->col = col;
        (*actor)->render = render;
}

void render_actor(struct Actor *actor){
        TCOD_console_set_char(NULL, actor->x, actor->y, actor->ch);
        TCOD_console_set_char_foreground(NULL, actor->x, actor->y, actor->col);
}
