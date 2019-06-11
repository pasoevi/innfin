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

    Written by Sergi Pasoev <s.pasoev@gmail.com>

*/

#include "gui.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static void mkmessage(struct message **message, const char *text,
                         TCOD_color_t col)
{
    struct message *tmp = malloc(sizeof *tmp);

    if (tmp) {
        tmp->text = malloc(strlen(text) + 1);
        if (tmp->text) {
            strcpy(tmp->text, text); /* TODO: free every message->text! */
            tmp->col = col;
            *message = tmp;
        }
    }
}

void free_message(struct message *message)
{
    if (message != NULL) {
        free(message->text);
        free(message);
    }
}

static void message(struct engine *engine, const TCOD_color_t col,
                    const char *text, ...)
{
    TCOD_list_t log = engine->gui->log;
    /* Build the text */
    va_list ap;
    char buf[128];
    va_start(ap, text);
    vsprintf(buf, text, ap);
    va_end(ap);

    char *line_begin = buf;
    char *line_end;

    do {
        /* make room for the new message */
        if (TCOD_list_size(log) == MSG_HEIGHT) {
            struct message *to_remove =
                    TCOD_list_get(log, 0);
            TCOD_list_remove(log, to_remove);
            free(to_remove);
        }
        // detect end of the line
        line_end = strchr(line_begin, '\n');
        if (line_end)
            *line_end = '\0';
        // add a new message to the log
        struct message *msg;
        mkmessage(&msg, line_begin, col);
        TCOD_list_push(log, msg);
        /* go to next line */
        line_begin = line_end + 1;
    } while (line_end);
}

static void render_bar(struct engine *engine, int x, int y, int w,
                       const char *name, double value, const double max_value,
                       TCOD_color_t bar_col, TCOD_color_t back_col)
{
    TCOD_console_set_default_background(engine->gui->con, back_col);
    TCOD_console_rect(engine->gui->con, x, y, w, 1, false,
                      TCOD_BKGND_SET);

    int bar_w = (int) (value / max_value * w);
    if (bar_w > 0) {
        /* Draw the bar */
        TCOD_console_set_default_background(engine->gui->con, bar_col);
        TCOD_console_rect(engine->gui->con, x, y, bar_w, 1, false,
                          TCOD_BKGND_SET);
    }

    /* Print text on top of a bar */
    TCOD_color_t hp_color = TCOD_black;
    TCOD_console_set_default_foreground(engine->gui->con, hp_color);
    TCOD_console_print_ex(engine->gui->con, x + w / 2, y,
                          TCOD_BKGND_NONE, TCOD_CENTER, "%s : %.0f/%.0f",
                          name, value, max_value);
}

static void render_fixed_text(TCOD_console_t con, int x, int y,
                              TCOD_color_t col, const char *text)
{
    TCOD_console_set_default_foreground(con, col);
    TCOD_console_print_ex(con, x, y, TCOD_BKGND_NONE, TCOD_LEFT, "%s", text);
}

static void render_status(TCOD_console_t con, int x, int y,
                          struct actor *actor)
{
    struct message status = get_hunger_status(actor);
    if (strlen(status.text) > 0)
        render_fixed_text(con, x, y, status.col, status.text);
}

static void render_log(struct engine *engine, int start_x, int start_y)
{
    /* draw the message log */
    double color_coef = 0.4f;
    int y = start_y;

    TCOD_list_t log = engine->gui->log;

    struct message **iter;
    for (iter = (struct message **) TCOD_list_begin(log);
         iter != (struct message **) TCOD_list_end(log);
         iter++) {
        struct message *message = *iter;
        // TCOD_color_t col = TCOD_color_multiply_scalar(message->col, color_coef);
        TCOD_console_set_default_foreground(engine->gui->con, message->col);
        TCOD_console_print(engine->gui->con, start_x, y,
                           message->text);
        y++;
        if (color_coef < 1.0f)
            color_coef += 0.3f;
    }
}

static void render_mouse_look(struct engine *engine)
{
    if (!is_in_fov(engine->map, engine->mouse.cx, engine->mouse.cy))
        /* if mouse is out of fov, nothing to render */
        return;

    char buf[128] = {'\0'};
    bool first = true;

    struct actor **iterator;
    for (iterator = (struct actor **) TCOD_list_begin(engine->actors);
         iterator != (struct actor **) TCOD_list_end(engine->actors);
         iterator++) {
        struct actor *actor = *iterator;
        /* Find actors under the mouse cursor */
        if (actor->x == engine->mouse.cx
            && actor->y == engine->mouse.cy) {
            if (!first)
                strcat(buf, ", ");
            else
                first = false;

            strcat(buf, actor->name);
        }
    }
    /*  display the list of actors under the mouse cursor */
    TCOD_console_set_default_foreground(engine->gui->con,
                                        TCOD_light_grey);
    TCOD_console_print(engine->gui->con, 1, 0, buf);
}

static void gui_render(struct engine *engine)
{
    /* Clear the gui console */
    TCOD_console_set_default_background(engine->gui->con, TCOD_black);
    TCOD_console_clear(engine->gui->con);

    /* Render health bar */
    engine->gui->render_bar(engine, 1, 1, BAR_W, "HP",
                            engine->player->life->hp,
                            engine->player->life->max_hp,
                            TCOD_white, TCOD_lighter_gray);

    /* Render XP bar */
    double next_level_xp = calc_next_level_xp(engine, engine->player);
    char xp_txt[20];
    sprintf(xp_txt, "level %d XP", engine->player->ai->xp_level);
    engine->gui->render_bar(engine, 1, 2, BAR_W, xp_txt, engine->player->ai->xp,
                            next_level_xp, TCOD_white, TCOD_lighter_gray);


    render_status(engine->gui->con, 1, 4, engine->player);

    engine->gui->render_log(engine, MSG_X, 1);
    engine->gui->render_mouse_look(engine);

    TCOD_console_blit(engine->gui->con, 0, 0, engine->window_w,
                      PANEL_H, NULL, 0, engine->window_h - PANEL_H,
                      1.f, 1.f);
}

void free_log(TCOD_list_t log)
{
    struct message **iter;
    for (iter = (struct message **) TCOD_list_begin(log);
         iter != (struct message **) TCOD_list_end(log);
         iter++) {
        struct message *message = *iter;
        free_message(message);
    }

    TCOD_list_delete(log);
}

void free_gui(struct gui *gui)
{
    free_log(gui->log);
    free(gui);
}

struct gui *mkgui(int w, int h)
{
    struct gui *gui = malloc(sizeof *gui);
    if (gui) {
        gui->con = TCOD_console_new(w, h);
        gui->inventory_con =
                   TCOD_console_new(INVENTORY_WIDTH, INVENTORY_HEIGHT);
        gui->render_bar = render_bar;
        gui->render_log = render_log;
        gui->render_mouse_look = render_mouse_look;
        gui->render = gui_render;
        gui->message = message;
        gui->log = TCOD_list_new();
    }

    return gui;
}
