#include <stdlib.h>
#include <stdio.h>
#include "engine.h"

extern void compute_fov(struct Engine *engine);
extern void init_map(struct Engine *engine, int w, int h);
extern bool is_in_fov(struct Map *map, int x, int y);
extern void clean(void);

void send_to_back(struct Engine *engine, struct Actor *actor){
        TCOD_list_remove(engine->actors, actor);
        TCOD_list_insert_before(engine->actors, actor, 0);
}

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
        make_player(&player, 40, 25);
        player->update = player_update;
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
        engine->game_status= IDLE ;
        
        TCOD_sys_check_for_event(TCOD_EVENT_KEY_PRESS, &(engine->key), NULL);
        player->update(engine, player);
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
	/* Rudimentary gui: print hp */
	TCOD_console_print(NULL, 1, WINDOW_H - 2, "HP: %d/%d",
			   (int)engine->player->destructible->hp,
			   (int)engine->player->destructible->max_hp);
        
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
