#include <stdlib.h>
#include "engine.h"

extern void init_map(struct Engine *engine, int w, int h);

void engine_init(struct Engine **engine, int w, int h, const char *title){
        TCOD_console_init_root(w, h, title, false, TCOD_RENDERER_OPENGL);

        *engine = malloc(sizeof (struct Engine));
        (*engine)->update = engine_update;
        (*engine)->render = engine_render;

        /* Add a map to the engine */
        init_map(*engine, 80, 45);
                
        /* Create a player */
        init_actor(&((*engine)->player), 40, 25, '@', TCOD_white, render_actor);
        
        (*engine)->actors = TCOD_list_new();
        TCOD_list_push((*engine)->actors, (const void *)(*engine)->player);
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

void engine_render(struct Engine *engine){
        TCOD_console_clear(NULL);
        map_render(engine->map);
        engine->player->render(engine->player);
        /* map_render(engine->map); */

        /* draw the actors */
        struct Actor **iter;

        for(iter = (struct Actor **)TCOD_list_begin(engine->actors);
             iter != (struct Actor **)TCOD_list_end(engine->actors);
             iter++){
                (*iter)->render(*iter);
        }
        
        TCOD_console_flush(NULL); 
}
