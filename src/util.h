#ifndef UTIL_H
#define UTIL_H
#include "libtcod.h"
#include "actor.h"

/* Iterate over lst and call `action` for each actor. */
void each_actor(struct engine *engine, TCOD_list_t lst, void (*action)(struct engine *engine, struct actor *actor));
#endif
