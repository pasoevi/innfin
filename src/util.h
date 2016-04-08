#ifndef UTIL_H
#define UTIL_H
#include "libtcod.h"
#include "actor.h"

float get_distance_btwn_points(int x1, int y1, int x2, int y2);
/* Iterate over lst and call `action` for each actor. */
void each_actor(struct engine *engine, TCOD_list_t lst, void (*action)(struct engine *engine, struct actor *actor));
#endif
