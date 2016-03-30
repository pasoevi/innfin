#include "libtcod.h"
#include <stdlib.h>
#include "engine.h"

#define PROGRAM_NAME "Mysteries"

struct engine *engine;

void init(void){
        engine = engine_init(WINDOW_W, WINDOW_H, PROGRAM_NAME);
}

void clean(){
        free_engine(engine);
	exit(EXIT_SUCCESS);
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
