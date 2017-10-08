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

#include "libtcod/libtcod.h"
#include <stdlib.h>
#include "engine.h"

#define PROGRAM_NAME "Innfin"

struct engine *engine;

void clean()
{
    free_engine(engine);
    exit(EXIT_SUCCESS);
}

int main()
{
    engine = mkengine(WINDOW_W, WINDOW_H, PROGRAM_NAME);

    while (!TCOD_console_is_window_closed()) {
        engine->update(engine);
        engine->render(engine);
        TCOD_console_flush();
    }

    clean();
    return 0;
}
