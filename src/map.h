#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include "libtcod.h"
#include "engine.h"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

struct engine;

struct tile{
        bool explored;
};

struct bsp_traverse{
	int room_num;
	int lastx;
	int lasty;
};

struct map{
        int w;
        int h;
        void (*render)(struct map *);
        struct tile tiles[80 * 45];
        TCOD_bsp_t *bsp;
        TCOD_map_t map;
	struct bsp_traverse bsp_traverse;
};

/* Initialise the map with w(idth) and h(eight) */
void init_map(struct engine *engine, int w, int h);
void free_map(struct map *map);

/* Draw the map on the screen */
void map_render(struct map *map);

void add_monster(struct engine* engine, int x, int y);
bool is_wall(struct map *map, int x, int y);

/* Check whether the point (x, y) is in the field of view on map. */
bool is_in_fov(struct map *map, int x, int y);
bool can_walk(struct engine *engine, int x, int y);
void set_wall(struct map *map, int x, int y);

/*
 * Compute the field of view for the map. You pass the engine
 * object. The field of view is computer for engine->map->map.
 */
void compute_fov(struct engine *engine);
#endif
