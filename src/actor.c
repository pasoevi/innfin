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
        if(actor->destructible && is_dead(actor)){
                return;
        }
        int dx = 0, dy = 0;
        
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

float take_damage(struct Actor *target, float damage){
	damage -= target->destructible->defence;
	if(damage > 0){
		target->destructible->hp -= damage;
		if(target->destructible->hp <= 0){
			die(target);
		}
	}else{
		damage = 0;
	}
	return damage;
}

/* Transform the actor into a rotting corpse */
void die(struct Actor *actor){
	actor->ch = '%';
	actor->col = TCOD_dark_red;
	actor->name = actor->destructible->corpse_name;
	actor->blocks = false;
	/* make sure corpses are drawn before living actors */
	/* engine.sendToBack(actor); */
}

/* Transform the actor into a rotting corpse */
void player_die(struct Actor *actor){
        printf("You die.\n");
        die(actor);
}

void attack(struct Actor *dealer, struct Actor *target){
        float power = dealer->attacker->power;
        float defence = target->destructible->defence;
        if(target->destructible && !is_dead(target)){
                if(power - defence > 0){
                        printf("%s attacks %s for %f hit points.\n", dealer->name, target->name,
                               power - defence);
                }else{
                        printf("%s attacks %s but it has no effect!\n", dealer->name, target->name);            
                }
                target->destructible->take_damage(target, power);
        }else{
                printf("%s attacks %s in vain.\n", dealer->name, target->name);
        }
}
