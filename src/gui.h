#ifndef GUI_H
#define GUI_H
#include "libtcod.h"
#include "engine.h"

enum{
        PANEL_H = 7,
        BAR_W = 20,
        MSG_X = BAR_W + 2,
        MSG_HEIGHT = PANEL_H - 1
};

struct message{
        char *text;
        TCOD_color_t col;
};

struct gui{
        TCOD_console_t *con;
        void (*render)(struct engine *engine);
        void (*render_bar)(struct engine *engine, int x, int y, int w,
                           const char *name, const float value,
                           const float max_value, const TCOD_color_t bar_col,
                           const TCOD_color_t back_col);
        void (*render_log)(struct engine *engine, int startx, int starty);
        void (*message)(struct engine *engine, const TCOD_color_t col, const char *text, ...);
        TCOD_list_t *log;
};

void free_gui(struct gui *gui);
void init_gui(struct gui **gui, int w, int h);
#endif
