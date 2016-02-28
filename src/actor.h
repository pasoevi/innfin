#include "libtcod.h"

struct Actor{
        int x;
        int y;
        char ch;
        TCOD_color_t col;
        void (*render)(struct Actor *actor);
};

void init_actor(struct Actor **actor, int w, int h, int ch, TCOD_color_t col, void (*render)(struct Actor *));
void render_actor(struct Actor *actor);
