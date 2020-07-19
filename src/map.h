/*
  Copyright (C) 2016 Sergo Pasoevi.

  This pragram is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at
  your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

  Written by Sergo Pasoevi <spasoevi@gmail.com>

*/

#ifndef MAP_H
#define MAP_H

#include "engine.h"
#include "libtcod/libtcod.h"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

struct engine;

struct tile {
    bool explored;
};

struct bsp_traverse {
    int room_num;
    int lastx;
    int lasty;
};

struct map {
    int w;
    int h;

    void (*render)(struct map *);

    struct tile tiles[80 * 45];
    TCOD_bsp_t *bsp;
    TCOD_map_t map;
    struct bsp_traverse bsp_traverse;
};

/*
 * Initialise the map with w(idth) and h(eight)
 * Creates new map and assigns it to engine->map.
 * @param engine
 * @param w
 * @param h
 */
void create_map(struct engine *engine, int w, int h);

void free_map(struct map *map);

/* Draw the map on the screen */
void map_render(struct map *map);

void map_update(struct map *map);

void add_monster(struct engine *engine, int x, int y);

void add_item(struct engine *engine, int x, int y);

bool is_wall(struct map *map, int x, int y);

bool is_explored(struct map *map, int x, int y);
/* Check whether the point (x, y) is in the field of view on map. */
bool is_in_fov(struct map *map, int x, int y);

bool can_walk(struct engine *engine, int x, int y);

void set_wall(struct map *map, int x, int y);

bool pick_tile(struct engine *engine, int *x, int *y, double max_range);

/*
 * Compute the field of view for the map. You pass the engine
 * object. The field of view is computer for engine->map->map.
 */
void compute_fov(struct engine *engine);

#endif
