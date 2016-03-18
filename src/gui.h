#ifndef GUI_H
#define GUI_H
#include "libtcod.h"
#include "engine.h"

enum{
        PANEL_H = 7,
        BAR_W = 20
};

struct gui{
        TCOD_console_t *con;
        void (*render)(struct engine *engine);
        void (*render_bar)(struct engine *engine, int x, int y, int w,
                           const char *name, const float value,
                           const float max_value, const TCOD_color_t bar_col,
                           const TCOD_color_t back_col);
};

void init_gui(struct gui **gui, int w, int h);
#endif
