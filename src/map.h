#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include "libtcod.h"
#include "engine.h"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

struct Engine;

struct Tile{
        bool explored;
};

/* 
   TODO: Isn't used at the moment. The same functionality is
   accomplished by the static variables in the function
*/
struct BSPTraverse{
	int room_num;
	int lastx;
	int lasty;
};
		

struct Map{
        int w;
        int h;
        void (*render)(struct Map *);
        struct Tile tiles[80 * 45];
        TCOD_bsp_t *bsp;
        TCOD_map_t map;
	struct BSPTraverse bsp_traverse;
};

/* void init_map(struct Engine *engine, int w, int h); */
void map_render(struct Map *map);
void add_monster(struct Engine* engine, int x, int y);
bool is_wall(struct Map *map, int x, int y);
bool can_walk(struct Engine *engine, int x, int y);
void set_wall(struct Map *map, int x, int y);

#endif
