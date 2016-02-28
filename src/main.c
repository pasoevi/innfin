#include "libtcod.h"
#include <stdlib.h>
#include "actor.h"
#include "map.h"

#define PROGRAM_NAME "mysteries"

enum{
        window_w = 80,
        window_h = 50
};

void init(void){
        TCOD_console_init_root(window_w,
                               window_h,
                               PROGRAM_NAME,
                               false,
                               TCOD_RENDERER_OPENGL);
}

int main() {
        init();
        
        struct Actor player = {40, 25, '@', TCOD_white, renderActor};
        struct Map *map;
        init_map(&map, 80, 45);

        while (!TCOD_console_is_window_closed()) {
                TCOD_key_t key = TCOD_console_check_for_keypress(TCOD_EVENT_KEY_PRESS);
                
                switch(key.vk) {
                case TCODK_UP :
                        if(!is_wall(map, player.x, player.y - 1)){
                                player.y--;
                        }
                        break;
                case TCODK_DOWN :
                        if(!is_wall(map, player.x, player.y + 1)){
                                player.y++;
                        }
                        break;
                case TCODK_LEFT :
                        if(!is_wall(map, player.x - 1, player.y)){
                                player.x--;
                        }
                        break;
                case TCODK_RIGHT :
                        if(!is_wall(map, player.x + 1, player.y)){
                                player.x++;
                        }
                        break;
                default:break;
                }

                TCOD_console_clear(NULL);
                player.render(&player);
                map_render(map);
                TCOD_console_flush(NULL);
        }

        free(map->tiles);
        free(map);
        
        return 0;
}
