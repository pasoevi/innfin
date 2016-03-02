#include <stdlib.h>
#include "engine.h"

extern void compute_fov(struct Engine *engine);
extern void init_map(struct Engine *engine, int w, int h);
extern bool is_in_fov(struct Map *map, int x, int y);

void engine_init(struct Engine **engine, int w, int h, const char *title){
        TCOD_console_init_root(w, h, title, false, TCOD_RENDERER_OPENGL);

        *engine = malloc(sizeof (struct Engine));
        (*engine)->update = engine_update;
        (*engine)->render = engine_render;

        (*engine)->fov_radius = 10;
        (*engine)->compute_fov = true;
        
        /* Create a player */
        struct Actor *player;
        init_actor(&player, 40, 25, '@', TCOD_white, render_actor);
        (*engine)->player = player;
        
        (*engine)->actors = TCOD_list_new();
        TCOD_list_push((*engine)->actors, (const void *)player);

        /* Add a map to the engine */
        init_map(*engine, 80, 45);
}

void engine_update(struct Engine *engine){
        struct Actor *player = engine->player;
        
        TCOD_key_t key = TCOD_console_check_for_keypress(TCOD_EVENT_KEY_PRESS);
                
        switch(key.vk) {
        case TCODK_UP :
                if(!is_wall(engine->map, player->x, player->y - 1)){
                        player->y--;
                        engine->compute_fov = true;
                }
                break;
        case TCODK_DOWN :
                if(!is_wall(engine->map, player->x, player->y + 1)){
                        player->y++;
                        engine->compute_fov = true;
                }
                break;
        case TCODK_LEFT :
                if(!is_wall(engine->map, player->x - 1, player->y)){
                        player->x--;
                        engine->compute_fov = true;
                }
                break;
        case TCODK_RIGHT :
                if(!is_wall(engine->map, player->x + 1, player->y)){
                        player->x++;
                        engine->compute_fov = true;
                }
                break;
        default:break;
        }
        if(engine->compute_fov){
                compute_fov(engine);
                engine->compute_fov = false;
        }
}

void engine_render(struct Engine *engine){
        TCOD_console_clear(NULL);
        map_render(engine->map);
        engine->player->render(engine->player);
        
        /* draw the actors */
        struct Actor **iter;

        for(iter = (struct Actor **)TCOD_list_begin(engine->actors);
             iter != (struct Actor **)TCOD_list_end(engine->actors);
             iter++){
                if(is_in_fov(engine->map, (*iter)->x, (*iter)->y)){
                        (*iter)->render(*iter);
                }
        }
        
        TCOD_console_flush(NULL); 
}
