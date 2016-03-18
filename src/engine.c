#include <stdlib.h>
#include <stdio.h>
#include "engine.h"
#include "map.h"

extern void clean(void);

/*
 * Move the dead actor at the back of the list, so that it doesn't
 * cover the living actors.
 */
void send_to_back(struct engine *engine, struct actor *actor){
        TCOD_list_remove(engine->actors, actor);
        TCOD_list_insert_before(engine->actors, actor, 0);
}

void engine_init(struct engine **engine, int w, int h, const char *title){
        TCOD_console_init_root(w, h, title, false, TCOD_RENDERER_OPENGL);
        struct engine *tmp = malloc(sizeof (struct engine));

        struct gui *gui = malloc(sizeof (struct gui));
        init_gui(&gui, WINDOW_W, PANEL_H);
        tmp->gui = gui;
        tmp->window_w = WINDOW_W;
        tmp->window_h = WINDOW_H;
        tmp->update = engine_update;
        tmp->render = engine_render;

        tmp->fov_radius = 10;
        tmp->compute_fov = true;
        tmp->game_status = STARTUP;
        
        /* Create a player */
        struct actor *player;
        make_player(&player, 40, 25);
        player->update = player_update;
        tmp->player = player;
        
        tmp->actors = TCOD_list_new();
        TCOD_list_push(tmp->actors, (const void *)player);

        /* Add a map to the engine */
        init_map(tmp, 80, 43);
        *engine = tmp;
}

void engine_update(struct engine *engine){
        struct actor *player = engine->player;
        
        if(engine->game_status == STARTUP ){
                compute_fov(engine);
        }
        engine->game_status= IDLE ;
        
        TCOD_sys_check_for_event(TCOD_EVENT_KEY_PRESS, &(engine->key), NULL);
        player->update(engine, player);
        if (engine->game_status == NEW_TURN) {
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
	/* Rudimentary gui: print hp */
        /*
	TCOD_console_print(NULL, 1, WINDOW_H - 2, "HP: %d/%d",
			   (int)engine->player->destructible->hp,
			   (int)engine->player->destructible->max_hp);
        */

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

void free_engine(struct engine engine){
        /* free all memory directly or indirectly allocated by the
         * engine 
         */
}
