#include "gui.h"
#include <stdlib.h>
#include <stdarg.h>
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

static void render_log(struct engine *engine, int startx , int starty){
        /* draw the message log */
        struct message **iter;
        float color_coef = 0.4f;
        int y = starty;
        for(iter = (struct message **)TCOD_list_begin(engine->gui->log);
            iter != (struct message **)TCOD_list_end(engine->gui->log);
            iter++){
                TCOD_console_set_default_foreground(engine->gui->con, TCOD_color_multiply_scalar((*iter)->col, color_coef));
                TCOD_console_print(engine->gui->con, startx, y, (*iter)->text);
                y++;
                if(color_coef < 1.0f) {
                        color_coef += 0.3f;
                }
        }
}

static void gui_render(struct engine *engine){
        /* Clear the gui console */
        TCOD_console_set_default_background(engine->gui->con, TCOD_black);
        TCOD_console_clear(engine->gui->con);
        engine->gui->render_bar(engine, 1, 1, BAR_W,"HP",
                                engine->player->destructible->hp,
                                engine->player->destructible->max_hp,
                                TCOD_light_red, TCOD_darker_red);
        engine->gui->render_log(engine, MSG_X, 1);
        TCOD_console_blit(engine->gui->con, 0, 0, engine->window_w, PANEL_H,
                          NULL, 0, engine->window_h - PANEL_H, 1.f, 1.f);
}

void free_gui(struct gui *gui){
        /* TCOD_list_delete(gui->log); */
        TCOD_list_clear_and_delete(gui->log);
        free(gui);
}

void init_gui(struct gui **gui, int w, int h){
        struct gui *tmp = malloc(sizeof *tmp);
        tmp->con = TCOD_console_new(w, h);
        tmp->render_bar = render_bar;
        tmp->render_log = render_log;
        tmp->render = gui_render;
        tmp->log = TCOD_list_new();
        *gui = tmp;
}
                      
void message(const TCOD_color_t col, const char *text, ...){
        
}
