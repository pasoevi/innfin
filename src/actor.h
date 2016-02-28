#include "libtcod.h"

struct Actor{
        int x;
        int y;
        char ch;
        TCOD_color_t col;
        void (*render)(struct Actor *actor);
};

void renderActor(struct Actor *actor);
