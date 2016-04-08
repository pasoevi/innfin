#include "util.h"
#include <math.h>

/* Geometry helper functions */
float get_distance_btwn_points(int x1, int y1, int x2, int y2){
        int dx = x1 - x2;
        int dy = y1 - y2;
        return sqrtf(dx * dx + dy * dy);
}

void each_actor(struct engine *engine, TCOD_list_t lst, void (*action)(struct engine *engine, struct actor *actor))
{
        struct actor **iterator;
        for (iterator = (struct actor **)TCOD_list_begin(engine->actors);
             iterator != (struct actor **)TCOD_list_end(engine->actors);
             iterator++) 
                action(engine, *iterator);
}
