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

#ifndef ENGINE_H
#define ENGINE_H

#include "libtcod.h"
#include "actor.h"
#include "map.h"
#include "gui.h"

enum {
    WINDOW_W = 80,
    WINDOW_H = 50
};

enum GameStatus {
    STARTUP,
    IDLE,
    NEW_TURN,
    VICTORY,
    DEFEAT
};

struct engine {
    TCOD_list_t *actors;
    struct actor *player;
    struct map *map;
    int fov_radius;
    bool compute_fov;
    TCOD_key_t key;
    TCOD_mouse_t mouse;
    enum GameStatus game_status;
    struct gui *gui;
    int window_w;
    int window_h;

    void (*update)(struct engine *);

    void (*render)(struct engine *);
};

struct engine *engine_init(int w, int h, const char *title);

void free_engine(struct engine *engine);

void send_to_back(struct engine *engine, struct actor *actor);

void engine_update(struct engine *engine);

void engine_render(struct engine *engine);

#endif
