#include "libtcod.h"
#include <stdlib.h>
#include "engine.h"

#define PROGRAM_NAME "Mysteries"

struct Engine *engine;

void init(void){
        engine_init(&engine, WINDOW_W, WINDOW_H, PROGRAM_NAME);
}

void clean(){
	/* free(engine->map->tiles); */
        TCOD_bsp_delete(engine->map->bsp);
        free(engine->map);
        free(engine->player);
	exit(EXIT_SUCCESS);
        /* TODO: free actors list! */	
}

int main() {
        init();

        while (!TCOD_console_is_window_closed()) {
                engine->update(engine);
                engine->render(engine);
        }

	clean();

        return 0;
}
