#include "gui.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static void init_message(struct message **message, const char *text, TCOD_color_t col){
        struct message *tmp = malloc(sizeof *tmp);
        tmp->text = strdup(text);
        tmp->col = col;
        *message = tmp;
}

void free_message(struct message *message){
        if(message != NULL){
                free(message->text);                     
                free(message);
        }
}

static void message(struct engine *engine, const TCOD_color_t col, const char *text, ...){
        /* Build the text */
        va_list ap;
        char buf[128];
        va_start(ap,text);
        vsprintf(buf,text,ap);
        va_end(ap);

        char *line_begin=buf;
        char *line_end;
        
        do {
                /* make room for the new message */
                if (TCOD_list_size(engine->gui->log) == MSG_HEIGHT ) {
                        struct message *to_remove = TCOD_list_get(engine->gui->log, 0);
                        TCOD_list_remove(engine->gui->log, to_remove);
                        free(to_remove);
                }
                // detect end of the line
                line_end = strchr(line_begin, '\n');
                if (line_end){
                        *line_end = '\0';
                }
                // add a new message to the log
                struct message *msg;
                init_message( &msg, line_begin, col);
                TCOD_list_push(engine->gui->log, msg);
                /* go to next line */
                line_begin=line_end + 1;
        }while(line_end);
}

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

static void render_mouse_look(struct engine *engine){
        if (!is_in_fov(engine->map, engine->mouse.cx, engine->mouse.cy)) {
                /* if mouse is out of fov, nothing to render */
                return;
        }

        char buf[128]={'\0'};
        bool first = true;

        struct actor **iterator;
        for (iterator = (struct actor **)TCOD_list_begin(engine->actors);
             iterator != (struct actor **)TCOD_list_end(engine->actors);
             iterator++) {
                struct actor *actor=*iterator;
                /* Find actors under the mouse cursor */
                if (actor->x == engine->mouse.cx && actor->y == engine->mouse.cy ) {
                        if (!first) {
                                strcat(buf,", ");
                        }else{
                                first = false;
                        }
                        
                        strcat(buf, actor->name);
                }
        }
        /*  display the list of actors under the mouse cursor */
        TCOD_console_set_default_foreground(engine->gui->con, TCOD_light_grey);
        TCOD_console_print(engine->gui->con, 1, 0, buf);
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
        engine->gui->render_mouse_look(engine);
        TCOD_console_blit(engine->gui->con, 0, 0, engine->window_w, PANEL_H,
                          NULL, 0, engine->window_h - PANEL_H, 1.f, 1.f);
}

void free_gui(struct gui *gui){
        TCOD_list_clear_and_delete(gui->log);
        free(gui);
}

struct gui *init_gui(int w, int h){
        struct gui *tmp = malloc(sizeof *tmp);
        tmp->con = TCOD_console_new(w, h);
        tmp->inventory_con = TCOD_console_new(INVENTORY_WIDTH, INVENTORY_HEIGHT);
        tmp->render_bar = render_bar;
        tmp->render_log = render_log;
        tmp->render_mouse_look = render_mouse_look;
        tmp->render = gui_render;
        tmp->message = message;
        tmp->log = TCOD_list_new();
        return tmp;
}
