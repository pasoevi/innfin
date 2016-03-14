#include "actor.h"
#include <stdlib.h>
#include <stdio.h>

void init_actor(struct Actor **actor, int x, int y, int ch, const char *name, TCOD_color_t col, void (*render)(struct Actor *)){
        *actor = malloc(sizeof (struct Actor));
        (*actor)->x = x;
        (*actor)->y = y;
        (*actor)->ch = ch;
        (*actor)->name = name;
        (*actor)->col = col;
        (*actor)->render = render;
        (*actor)->update = actor_update;
}

void render_actor(struct Actor *actor){
        TCOD_console_set_char(NULL, actor->x, actor->y, actor->ch);
        TCOD_console_set_char_foreground(NULL, actor->x, actor->y, actor->col);
}

bool move_or_attack(struct Engine *engine, struct Actor *actor, int x, int y){
        if(is_wall(engine->map, x, y)){
                return false;
        }

        struct Actor **iter;
        for(iter = (struct Actor **)TCOD_list_begin(engine->actors);
            iter != (struct Actor **)TCOD_list_end(engine->actors);
            iter++){
                if((*iter)->x == x && (*iter)->y == y){
                        /* There is an actor there, cat't walk */
                        printf("The %s laughs at your puny efforts to attack him!\n",
                               (*iter)->name);
                        
                        return false;
                }
        }
        
        actor->x = x;
        actor->y = y;
        
        return true;
}

void actor_update(struct Engine *engine, struct Actor *actor){
        printf("The %s growls\n", actor->name);
}

/* Not used yet */
void player_update(struct Engine *engine, struct Actor *actor){
        printf("The %s plays\n", actor->name);
}

bool is_dead(struct Actor *actor){
	if(actor->destructible != NULL){
		return actor->destructible->hp <= 0;
	}
	return false;
}
