#ifndef ENGINE_H
#define ENGINE_H

#include "libtcod.h"
#include "actor.h"
#include "map.h"

struct Engine{
        TCOD_list_t *actors;
        struct Actor *player;
        struct Map *map;
        void (*update)(struct Engine *);
        void (*render)(struct Engine *);
};

void engine_init(struct Engine **engine, int w, int h, const char *title);
void engine_update(struct Engine *engine);
void engine_render(struct Engine *engine);

#endif
