#include <stdlib.h>
#include <stdio.h>
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
        (*engine)->game_status = STARTUP;
        
        /* Create a player */
        struct Actor *player;
        init_actor(&player, 40, 25, '@', "player", TCOD_white, render_actor);
        /* player->update = player_update; */
        (*engine)->player = player;
        
        (*engine)->actors = TCOD_list_new();
        TCOD_list_push((*engine)->actors, (const void *)player);

        /* Add a map to the engine */
        init_map(*engine, 80, 45);
}

void engine_update(struct Engine *engine){
        struct Actor *player = engine->player;
        
        if(engine->game_status == STARTUP ){
                compute_fov(engine);
        }
        engine->game_status=IDLE;
        
        TCOD_key_t key;
        TCOD_sys_check_for_event(TCOD_EVENT_KEY_PRESS, &key, NULL);

        int dx=0,dy=0;
        switch(key.vk) {
        case TCODK_UP : dy= -1; break;
        case TCODK_DOWN : dy = 1; break;
        case TCODK_LEFT : dx = -1; break;
        case TCODK_RIGHT : dx= 1; break;
        default:break;
        }
        
        if ( dx != 0 || dy != 0 ) {
                engine->game_status= NEW_TURN;
                if(move_or_attack(engine, player, engine->player->x + dx, player->y + dy)){
                        compute_fov(engine);
                }
        }

        if (engine->game_status == NEW_TURN) {
                struct Actor **iterator;
                for (iterator = (struct Actor **)TCOD_list_begin(engine->actors);
                     iterator != (struct Actor **)TCOD_list_end(engine->actors);
                     iterator++) {
                        struct Actor *actor = *iterator;
                        if(actor != player ){
                                actor->update(engine, actor);
                        }
                }
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
