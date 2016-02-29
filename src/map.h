#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include "libtcod.h"
#include "engine.h"

struct Tile{
        bool can_walk;
};

struct Map{
        int w;
        int h;
        void (*render)(struct Map *);
        struct Tile *tiles;
        TCOD_bsp_t *bsp;
};

/* void init_map(struct Engine *engine, int w, int h); */
void map_render(struct Map *map);
bool is_wall(struct Map *map, int x, int y);
void set_wall(struct Map *map, int x, int y);

#endif
