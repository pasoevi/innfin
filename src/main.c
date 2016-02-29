#include "libtcod.h"
#include <stdlib.h>
#include "engine.h"

#define PROGRAM_NAME "Mysteries"

enum{
        WINDOW_W = 80,
        WINDOW_H = 50
};

struct Engine *engine;

void init(void){
        engine_init(&engine, WINDOW_W, WINDOW_H, PROGRAM_NAME);
}

int main() {
        init();

        while (!TCOD_console_is_window_closed()) {
                engine->update(engine);
                engine->render(engine);
        }

        free(engine->map->tiles);
        TCOD_bsp_delete(engine->map->bsp);
        free(engine->map);
        free(engine->player);
        /* TODO: free actors list! */
        
        return 0;
}
