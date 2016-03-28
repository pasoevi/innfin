#include "libtcod.h"
#include "actor.h"

void list_do(TCOD_list_t lst, void (*action)(struct engine *engine, struct actor *actor));
