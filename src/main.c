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
        init_engine(&engine, WINDOW_W, WINDOW_H, PROGRAM_NAME);
}

int main() {
        init();
        
        struct Actor *player = engine->player;

        while (!TCOD_console_is_window_closed()) {
                engine->update(engine);
                TCOD_console_clear(NULL);
                player->render(player);
                map_render(engine->map);
                TCOD_console_flush(NULL);
        }

        free(engine->map->tiles);
        free(engine->map);
        free(engine->player);
        /* TODO: free actors list! */
        
        return 0;
}
