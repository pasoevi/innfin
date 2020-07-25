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

#ifndef ENGINE_H
#define ENGINE_H

#include "actor.h"
#include "gui.h"
#include "libtcod/libtcod.h"
#include "map.h"

enum {
    /* Configuration options */
    WINDOW_W = 32,
    WINDOW_H = 25,

    /* Levels, dungeons, portals, places */
    LVL_DUNGEON_1,
    LVL_DUNGEON_2,
    LVL_DUNGEON_3,
    LVL_DUNGEON_4,
    LVL_DUNGEON_5,
    LVL_DUNGEON_6,
    LVL_DUNGEON_7,
    LVL_DUNGEON_8,
    LVL_DUNGEON_9,
    LVL_DUNGEON_10,
    LVL_DUNGEON_11,
    LVL_DUNGEON_12,
    LVL_DUNGEON_13,
    LVL_DUNGEON_14,
    LVL_DUNGEON_15,
    LVL_DUNGEON_16,

    /* Monsters, items, spells, potions, weapons */
};

enum game_status {
    STARTUP,
    IDLE,
    NEW_TURN,
    VICTORY,
    DEFEAT
};

struct engine {
    TCOD_list_t actors;
    struct actor *player;
    struct actor *stairs;
    struct map *map;
    int fov_radius;
    bool compute_fov;
    int key;
    TCOD_mouse_t mouse;
    enum game_status game_status;
    struct gui *gui;
    int window_w;
    int window_h;
    int level;

    void (*update)(struct engine *);

    void (*render)(struct engine *);
};

struct engine *create_engine(int w, int h, const char *title);

int load_level(struct engine *engine, int level_id);

void free_engine(struct engine *engine);

// static int parse_datafiles();

void send_to_back(struct engine *engine, struct actor *actor);

void engine_update(struct engine *engine);

void engine_render(struct engine *engine);

#endif
