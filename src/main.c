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

#include <BearLibTerminal.h>
#include <stdlib.h>

#include "engine.h"
#include "libtcod/libtcod.h"

#define PROGRAM_NAME "Innfin"

struct engine *engine;

void cleanup() {
    free_engine(engine);
    exit(EXIT_SUCCESS);
}

void show_welcome_screen() {
    int old_color = terminal_state(TK_COLOR);
    int old_bg_color = terminal_state(TK_BKCOLOR);
    terminal_bkcolor(color_from_name("yellow"));
    terminal_color(color_from_name("light green"));
    terminal_print(19, 10, "Hello");
    terminal_color(old_color);
    terminal_bkcolor(old_bg_color);
}

void start_game() {
    engine = create_engine(WINDOW_W, WINDOW_H, PROGRAM_NAME);
    terminal_clear();
    terminal_refresh();
}

void restart_game(struct engine *engine) {
    free_engine(engine);
    start_game();
}

int main() {
    start_game();
    do {
        printf("%d: Here\n", engine->key);
        if (engine->key == TK_0) {
            restart_game(engine);
        }
        /*
            terminal_read() is a blocking operation. Render
           the current state first before waiting for input
        */
        engine->render(engine);
        engine->update(engine);
    } while (engine->key != TK_CLOSE && engine->key != TK_ESCAPE);

    cleanup();
    return 0;
}
