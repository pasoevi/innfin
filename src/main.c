#include "libtcod.h"
#include "actor.h"

#define PROGRAM_NAME "Mysteries"

enum{
        WINDOW_W = 80,
        WINDOW_H = 50
};

void init(void){
        TCOD_console_init_root(WINDOW_W, WINDOW_H, PROGRAM_NAME, false, TCOD_RENDERER_OPENGL);
}

int main() {
        init();
        
        struct Actor player = {40, 25, '@', TCOD_white, renderActor};

        while (!TCOD_console_is_window_closed()) {
                TCOD_key_t key = TCOD_console_check_for_keypress(TCOD_EVENT_KEY_PRESS);
                
                switch(key.vk) {
                case TCODK_UP : player.y--; break;
                case TCODK_DOWN : player.y++; break;
                case TCODK_LEFT : player.x--; break;
                case TCODK_RIGHT : player.x++; break;
                default:break;
                }

                TCOD_console_clear(NULL);
                player.render(&player);
                TCOD_console_flush(NULL);
        }
        
        return 0;
}
