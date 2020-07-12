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

#ifndef GUI_H
#define GUI_H

#include "libtcod/libtcod.h"
#include "engine.h"

enum {
    PANEL_H = 7,
    BAR_W = 20,
    MSG_X = BAR_W + 2,
    MSG_HEIGHT = PANEL_H - 1,
    INVENTORY_WIDTH = 50,
    INVENTORY_HEIGHT = 28
};

struct message {
    char *text;
    TCOD_color_t col;
};

struct gui {
    TCOD_console_t con;
    TCOD_console_t inventory_con;

    void (*render)(struct engine *engine);

    void (*render_bar)(struct engine *engine, int x, int y, int w,
                       const char *name, const double value,
                       const double max_value,
                       const TCOD_color_t bar_col,
                       const TCOD_color_t back_col);

    void (*render_log)(struct engine *engine, int start_x,
                       int start_y);

    void (*message)(struct engine *engine, const TCOD_color_t col,
                    const char *text, ...);

    void (*render_mouse_look)(struct engine *engine);

    TCOD_list_t log;
};

void free_gui(struct gui *gui);

struct gui *create_gui(int w, int h);

#endif
