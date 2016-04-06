#include "util.h"

void each_actor(struct engine *engine, TCOD_list_t lst, void (*action)(struct engine *engine, struct actor *actor))
{
        struct actor **iterator;
        for (iterator = (struct actor **)TCOD_list_begin(engine->actors);
             iterator != (struct actor **)TCOD_list_end(engine->actors);
             iterator++) 
                action(engine, *iterator);
}
