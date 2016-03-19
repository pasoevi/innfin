#include "gui.h"
#include <stdlib.h>
#include <stdio.h>

static void render_bar(struct engine *engine, int x, int y, int w,
                       const char *name, const float value,
                       const float max_value, const TCOD_color_t bar_col,
                       const TCOD_color_t back_col){
        
        TCOD_console_set_default_background(engine->gui->con, back_col);
        TCOD_console_rect(engine->gui->con, x, y, BAR_W, 1, false, TCOD_BKGND_SET);
        TCOD_console_print_ex(engine->gui->con, x + w/2, y, TCOD_BKGND_NONE, TCOD_CENTER,
                              "%s : %g/%g", name, value, max_value);
}

static void gui_render(struct engine *engine){
        /* Clear the gui console */
        TCOD_console_set_default_background(engine->gui->con, TCOD_black);
        TCOD_console_clear(engine->gui->con);
        engine->gui->render_bar(engine, 1, 1, BAR_W,"HP",
                                engine->player->destructible->hp,
                                engine->player->destructible->max_hp,
                                TCOD_light_red, TCOD_darker_red);

        TCOD_console_blit(engine->gui->con, 0, 0, engine->window_w, PANEL_H,
                          NULL, 0, engine->window_h - PANEL_H, 1.f, 1.f);
        printf("window_w = %d\nwindow_h = %d\npanel_h = %d\n", engine->window_w,
               engine->window_h, PANEL_H);
}

void init_gui(struct gui **gui, int w, int h){
        struct gui *tmp = malloc(sizeof *tmp);
        tmp->con = TCOD_console_new(w, h);
        tmp->render_bar = render_bar;
        tmp->render = gui_render;
        *gui = tmp;
}
                      
void message(const TCOD_color_t col, const char *text, ...){
        
}
