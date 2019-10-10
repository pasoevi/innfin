/*
    Copyright (C) 2016 Sergo Pasoevi.

    This program is free software: you can redistribute it and/or modify
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

#include <stdlib.h>
#include "engine.h"
#include "player.h"
#include "stairs.h"
#include "tiles.h"
#include "console.h"

extern void clean(void);

static const char *WELCOME_MSG =
        "Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.";

/*
 * Use Libtcod's configuration file parser to read monster, item, etc data
 * definitions.
static int parse_datafiles()
{
    TCOD_parser_t parser = TCOD_parser_new();
    TCOD_parser_struct_t actor = TCOD_parser_new_struct(parser, "item_type");

    return 0;
}
*/

/*
 * Move the dead actor at the back of the list, so that it doesn't
 * cover the living actors.
 */
void send_to_back(struct engine *engine, struct actor *actor)
{
    TCOD_list_remove(engine->actors, actor);
    TCOD_list_insert_before(engine->actors, actor, 0);
}

struct engine *mkengine(int w, int h, const char *title)
{

    TCOD_console_init_root(w, h, title, false, TCOD_RENDERER_SDL);
    struct engine *engine = malloc(sizeof *engine);
    TCOD_console_set_custom_font(
        "TiledFont.png",
        TCOD_FONT_TYPE_GREYSCALE  | TCOD_FONT_LAYOUT_TCOD,
        32,
        10
    );
    int a = 256;

    // The "y" is the row index, here we load the sixth row in the font file. Increase the "6" to load any new rows from the file
    for (int y = 5; y < 6; y++) {
        TCOD_console_map_ascii_codes_to_font(a, 32, 0, y);
        a += 32;
    }

    if (engine == NULL)
        return engine;

    engine->level = 1;

    engine->gui = mkgui(WINDOW_W, PANEL_H);
    engine->window_w = WINDOW_W;
    engine->window_h = WINDOW_H;
    engine->update = engine_update;
    engine->render = engine_render;

    engine->fov_radius = 10;
    engine->compute_fov = true;
    engine->game_status = STARTUP;

    engine->actors = TCOD_list_new();

    /* Create a player */
    engine->player = mkplayer(40, 25);
    engine->player->update = player_update;
    TCOD_list_push(engine->actors, (const void *) engine->player);

    /* Create stairs */
    engine->stairs = mkstairs(0, 0, STAIRSDOWN_TILE);
    TCOD_list_push(engine->actors, (const void *) engine->stairs);

    /* Add a map to the engine */
    mkmap(engine, 80, 43);
    engine->gui->message(engine, TCOD_red, WELCOME_MSG);

    return engine;
}

int load_level(struct engine *engine, int level_id)
{
    if (level_id < 0)
        return level_id;

    engine->level = level_id;

    free_map(engine->map);
    /* Delete all actors except the player and the stairs */
    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors);
         iter++) {
        struct actor *actor = *iter;
        if (actor != engine->player && actor != engine->stairs) {
            free_actor(actor);
            iter = (struct actor **) TCOD_list_remove_iterator(engine->actors,
                                                               (void **) iter);
        }
    }

    mkmap(engine, 80, 43);
    engine->game_status = STARTUP;

    /* Display the dungeon level */
    engine->gui->message(engine, TCOD_white, "Dungeon level %d\n",
                         engine->level);

    return engine->level;
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
        for (iterator = (struct actor **) TCOD_list_begin(engine->actors);
             iterator != (struct actor **) TCOD_list_end(engine->actors);
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

        if ((actor != engine->player) &&
            /* will be rendered later as the last item */
            ((!actor->fov_only &&
             is_explored(engine->map, actor->x, actor->y)) ||
            is_in_fov(engine->map, actor->x, actor->y)))
            actor->render(actor);
    }

    engine->player->render(engine->player);
}

/*
 * Free all memory directly or indirectly allocated by the
 * engine.
 */
void free_engine(struct engine *engine)
{
    free_gui(engine->gui);
    free_map(engine->map);
    free_actors(engine->actors);
    /* free(engine->player); */
    free(engine);
}
