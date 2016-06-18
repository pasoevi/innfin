/*
  Copyright (C) 2016 Sergi Pasoev.

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

  Written by Sergi Pasoev <s.pasoev@gmail.com>

*/

#include <stdlib.h>
#include "engine.h"

extern void clean(void);

static const char *WELCOME_MESSAGE =
        "Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.";

/*
 * Move the dead actor at the back of the list, so that it doesn't
 * cover the living actors.
 */
void send_to_back(struct engine *engine, struct actor *actor)
{
    TCOD_list_remove(engine->actors, actor);
    TCOD_list_insert_before(engine->actors, actor, 0);
}

struct engine *engine_init(int w, int h, const char *title)
{
    TCOD_console_init_root(w, h, title, false, TCOD_RENDERER_OPENGL);
    struct engine *engine = malloc(sizeof *engine);

    engine->gui = init_gui(WINDOW_W, PANEL_H);
    engine->window_w = WINDOW_W;
    engine->window_h = WINDOW_H;
    engine->update = engine_update;
    engine->render = engine_render;

    engine->fov_radius = 10;
    engine->compute_fov = true;
    engine->game_status = STARTUP;

    engine->actors = TCOD_list_new();
    /* Create a player */
    engine->player = make_player(40, 25);
    engine->player->update = player_update;
    TCOD_list_push(engine->actors, (const void *) engine->player);


    /* Add a map to the engine */
    init_map(engine, 80, 43);
    engine->gui->message(engine, TCOD_red, WELCOME_MESSAGE);
    return engine;
}

void engine_update(struct engine *engine)
{
    struct actor *player = engine->player;

    if (engine->game_status == STARTUP)
        compute_fov(engine);

    engine->game_status = IDLE;

    TCOD_sys_check_for_event(TCOD_EVENT_ANY, &engine->key, &engine->mouse);
    player->update(engine, player);

    map_update(engine->map);

    if (engine->game_status == NEW_TURN) {
        struct actor **iterator;
        for (iterator =
                     (struct actor **) TCOD_list_begin(engine->actors);
             iterator !=
             (struct actor **) TCOD_list_end(engine->actors);
             iterator++) {
            struct actor *actor = *iterator;
            if (actor != player)
                actor->update(engine, actor);
        }
    }
}

void engine_render(struct engine *engine)
{
    TCOD_console_clear(NULL);
    map_render(engine->map);

    engine->gui->render(engine);

    /* draw the actors */
    struct actor **iter;

    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors);
         iter++) {
        struct actor *actor = *iter;
        if (!actor->fov_only &&
            is_explored(engine->map, actor->x, actor->y)
            || is_in_fov(engine->map, actor->x, actor->y))
            actor->render(actor);
    }

    engine->player->render(engine->player);
}

void free_engine(struct engine *engine)
{
    /*
     * Free all memory directly or indirectly allocated by the
     * engine.
     */

    free_gui(engine->gui);
    free_map(engine->map);
    free_actors(engine->actors);
    /* free(engine->player); */
    free(engine);
}
