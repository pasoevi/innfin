#include "stairs.h"
#include "actor.h"

struct actor *init_stairs(int x, int y, char ch)
{
    struct actor *actor = init_actor(x, y, ch, "stairs", TCOD_red,
                                     render_actor);
    actor->blocking = false;
    actor->fov_only = false;
}