#include <stdlib.h>
#include "engine.h"

void init_engine(struct Engine **engine, int w, int h, const char *title){
        TCOD_console_init_root(w, h, title, false, TCOD_RENDERER_OPENGL);

        *engine = malloc(sizeof (struct Engine));
        (*engine)->update = engine_update;                

        /* Add a map to the engine */
        struct Map *map;
        init_map(&map, 80, 45);
        (*engine)->map = map;
        
        /* Create a player */
        init_actor(&((*engine)->player), 40, 25, '@', TCOD_white, render_actor);
        
        /* Add a simple monster */
        struct Actor *orc;
        init_actor(&orc, 60, 25, 'O', TCOD_yellow, render_actor);

        (*engine)->actors = TCOD_list_new();
        TCOD_list_push((*engine)->actors, (const void *)(*engine)->player);
        TCOD_list_push((*engine)->actors, (const void *)orc);
}

void engine_update(struct Engine *engine){
        struct Actor *player = engine->player;
        
        TCOD_key_t key = TCOD_console_check_for_keypress(TCOD_EVENT_KEY_PRESS);
                
        switch(key.vk) {
        case TCODK_UP :
                if(!is_wall(engine->map, player->x, player->y - 1)){
                        player->y--;
                }
                break;
        case TCODK_DOWN :
                if(!is_wall(engine->map, player->x, player->y + 1)){
                        player->y++;
                }
                break;
        case TCODK_LEFT :
                if(!is_wall(engine->map, player->x - 1, player->y)){
                        player->x--;
                }
                break;
        case TCODK_RIGHT :
                if(!is_wall(engine->map, player->x + 1, player->y)){
                        player->x++;
                }
                break;
        default:break;
        }
}
