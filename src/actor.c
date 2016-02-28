#include "actor.h"

void renderActor(struct Actor *actor){
        /* pass */
        TCOD_console_set_char(NULL, actor->x, actor->y, actor->ch);
        TCOD_console_set_char_foreground(NULL, actor->x, actor->y, actor->col);
}
