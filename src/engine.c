#include <stdlib.h>
#include <stdio.h>
#include "engine.h"
#include "map.h"

extern void clean(void);

static const char *WELCOME_MESSAGE =
        "Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.";

/*
 * Move the dead actor at the back of the list, so that it doesn't
 * cover the living actors.
 */
void send_to_back(struct engine *engine, struct actor *actor){
        TCOD_list_remove(engine->actors, actor);
        TCOD_list_insert_before(engine->actors, actor, 0);
}

struct engine *engine_init(int w, int h, const char *title){
        TCOD_console_init_root(w, h, title, false, TCOD_RENDERER_OPENGL);
        struct engine *tmp = malloc(sizeof *tmp);

        tmp->gui = init_gui(WINDOW_W, PANEL_H);
        tmp->window_w = WINDOW_W;
        tmp->window_h = WINDOW_H;
        tmp->update = engine_update;
        tmp->render = engine_render;

        tmp->fov_radius = 10;
        tmp->compute_fov = true;
        tmp->game_status = STARTUP;
        
        /* Create a player */

        tmp->player = make_player(40, 25);
        tmp->player->update = player_update;
        
        tmp->actors = TCOD_list_new();
        TCOD_list_push(tmp->actors, (const void *)tmp->player);

        /* Add a map to the engine */
        init_map(tmp, 80, 43);
        tmp->gui->message(tmp, TCOD_red, WELCOME_MESSAGE);
        return tmp;
}

void engine_update(struct engine *engine){
        struct actor *player = engine->player;
        
        if(engine->game_status == STARTUP ){
                compute_fov(engine);
        }
        
        engine->game_status = IDLE ;
        
        TCOD_sys_check_for_event(TCOD_EVENT_ANY, &(engine->key), &(engine->mouse));
        player->update(engine, player);
        if(engine->game_status == NEW_TURN){
                struct actor **iterator;
                for (iterator = (struct actor **)TCOD_list_begin(engine->actors);
                     iterator != (struct actor **)TCOD_list_end(engine->actors);
                     iterator++) {
                        struct actor *actor = *iterator;
                        if(actor != player ){ 
                                actor->update(engine, actor);                
                        }
                }
        }
}

void engine_render(struct engine *engine){
        TCOD_console_clear(NULL);
        map_render(engine->map);
        engine->player->render(engine->player);
        engine->gui->render(engine);
        
        /* draw the actors */
        struct actor **iter;

        for(iter = (struct actor **)TCOD_list_begin(engine->actors);
            iter != (struct actor **)TCOD_list_end(engine->actors);
            iter++){
                if(is_in_fov(engine->map, (*iter)->x, (*iter)->y)){
                        (*iter)->render(*iter);
                }
        }
        
        TCOD_console_flush(NULL); 
}

void free_engine(struct engine *engine){
        /* free all memory directly or indirectly allocated by the
         * engine 
         */
        
        free_gui(engine->gui);
        free_map(engine->map);
        free_actors(engine->actors);
        /* free(engine->player); */
        free(engine);
}
