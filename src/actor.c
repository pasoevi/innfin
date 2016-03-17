#include "actor.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern void compute_fov(struct Engine *engine);

/* Common functions */

/* 
The common update function that calls the intelligent update function
if present
*/
void common_update(struct Engine *engine, struct Actor *actor){
        if(actor->ai){
                actor->ai->update(engine, actor);
        }
}


float take_damage(struct Engine *engine, struct Actor *target, float damage){
	damage -= target->destructible->defence;
	if(damage > 0){
		target->destructible->hp -= damage;
		if(target->destructible->hp <= 0){
			target->destructible->die(engine, target);
		}
	}else{
		damage = 0;
	}
	return damage;
}

void init_actor(struct Actor **actor, int x, int y, int ch, const char *name,
                TCOD_color_t col,
                void (*render)(struct Actor *)){
        *actor = malloc(sizeof (struct Actor));
        (*actor)->x = x;
        (*actor)->y = y;
        (*actor)->ch = ch;
        (*actor)->name = name;
        (*actor)->col = col;
        (*actor)->render = render;
        (*actor)->update = common_update;
        
        /* Init attacker */
        (*actor)->attacker = malloc(sizeof(struct Attacker));
        
        /* Init destructible */
        (*actor)->destructible = malloc(sizeof(struct Destructible));

	/* Artificial intelligence */
	(*actor)->ai = malloc(sizeof(struct AI));
}

void render_actor(struct Actor *actor){
        TCOD_console_set_char(NULL, actor->x, actor->y, actor->ch);
        TCOD_console_set_char_foreground(NULL, actor->x, actor->y, actor->col);
}

void attack(struct Engine *engine, struct Actor *dealer, struct Actor *target){
        float power = dealer->attacker->power;
        float defence = target->destructible->defence;
        if(target->destructible && !is_dead(target)){
                if(power - defence > 0){
                        printf("%s attacks %s for %f hit points.\n", dealer->name, target->name,
                               power - defence);
                }else{
                        printf("%s attacks %s but it has no effect!\n", dealer->name, target->name);            
                }
                target->destructible->take_damage(engine, target, power);
        }else{
                printf("%s attacks %s in vain.\n", dealer->name, target->name);
        }
}

bool is_dead(struct Actor *actor){
	if(actor->destructible != NULL){
		return actor->destructible->hp <= 0;
	}
	return false;
}

/* Transform the actor into a rotting corpse */
void die(struct Engine *engine, struct Actor *actor){
	actor->ch = '%';
	actor->col = TCOD_dark_red;
	actor->name = actor->destructible->corpse_name;
	actor->blocks = false;
	/* make sure corpses are drawn before living actors */
        send_to_back(engine, actor);
                
}

/* Player functions */
void make_player(struct Actor **actor, int x, int y){
        init_actor(actor, x, y, '@', "you", TCOD_white, render_actor);

        (*actor)->ai->update = player_update;
        (*actor)->ai->move_or_attack = player_move_or_attack;

        (*actor)->attacker->power = 10;
        (*actor)->attacker->attack = attack;

        (*actor)->destructible->die = player_die;
        (*actor)->destructible->defence = 4;
        (*actor)->destructible->corpse_name = "your cadaver";
        (*actor)->destructible->take_damage = take_damage;
        (*actor)->destructible->max_hp = 100;
        (*actor)->destructible->hp = (*actor)->destructible->max_hp;
        (*actor)->destructible->defence = 4;
}

bool player_move_or_attack(struct Engine *engine, struct Actor *actor, int targetx, int targety){
        if(is_wall(engine->map, targetx, targety)){
                return false;
        }

        /* Look for actors to attack */
        struct Actor **iter;
        for(iter = (struct Actor **)TCOD_list_begin(engine->actors);
            iter != (struct Actor **)TCOD_list_end(engine->actors);
            iter++){
                if((*iter)->destructible && !is_dead(*iter) &&
                   (*iter)->x == targetx && (*iter)->y == targety){
                        /* There is an actor there, cat't walk */
                        actor->attacker->attack(engine, actor, *iter);
                        return false;
                }
        }

        /* Look for corpses */
        for(iter = (struct Actor **)TCOD_list_begin(engine->actors);
            iter != (struct Actor **)TCOD_list_end(engine->actors);
            iter++){
                if((*iter)->destructible && is_dead(*iter) &&
                   (*iter)->x == targetx && (*iter)->y == targety){
                        printf ("There's a %s here\n", (*iter)->name);
                }
        }
        
        actor->x = targetx;
        actor->y = targety;
        
        return true;
}

void player_update(struct Engine *engine, struct Actor *actor){
	if(actor->destructible && is_dead(actor)){
		return;
	}

        int dx = 0, dy = 0;

        switch(engine->key.vk) {
        case TCODK_UP : dy=-1; break;
        case TCODK_DOWN : dy=1; break;
        case TCODK_LEFT : dx=-1; break;
        case TCODK_RIGHT : dx=1; break;
        default:break;
        }

        if (dx != 0 || dy != 0) {
                engine->game_status= NEW_TURN;
                if(actor->ai->move_or_attack(engine, actor, actor->x + dx, actor->y + dy)) {
                        compute_fov(engine);
                }
        }
}

/* Transform the actor into a rotting corpse */
void player_die(struct Engine *engine, struct Actor *actor){
        printf("You die.\n");
        /* Call the common die function */
        die(engine, actor);
        engine->game_status = DEFEAT;
}

/* Monster functions */
void make_orc(struct Actor **actor, int x, int y){
        init_actor(actor, x, y, 'o', "orc", TCOD_desaturated_green, render_actor);

        
        (*actor)->ai->update = monster_update;
        (*actor)->ai->move_or_attack = monster_move_or_attack;

        (*actor)->attacker->power = 5;
        (*actor)->attacker->attack = attack;

        (*actor)->destructible->die = monster_die;
        (*actor)->destructible->defence = 2;
        (*actor)->destructible->corpse_name = "dead orc";
        (*actor)->destructible->take_damage = take_damage;
        (*actor)->destructible->max_hp = 15;
        (*actor)->destructible->hp = (*actor)->destructible->max_hp;
        (*actor)->destructible->defence = 2;
}

void make_troll(struct Actor **actor, int x, int y){
        init_actor(actor, x, y, 'T', "troll", TCOD_darker_green, render_actor);

        (*actor)->ai->update = monster_update;
        (*actor)->ai->move_or_attack = monster_move_or_attack;

        (*actor)->attacker->power = 7;
        (*actor)->attacker->attack = attack;

        (*actor)->destructible->die = monster_die;
        (*actor)->destructible->defence = 3;
        (*actor)->destructible->corpse_name = "troll carcass";
        (*actor)->destructible->take_damage = take_damage;
        (*actor)->destructible->max_hp = 20;
        (*actor)->destructible->hp = (*actor)->destructible->max_hp;
        (*actor)->destructible->defence = 3;
}

bool monster_move_or_attack(struct Engine *engine, struct Actor *actor, int targetx, int targety){
        int dx = targetx - actor->x;
        int dy = targety - actor->y;
	int stepdx = (dx > 0 ? 1 : -1);
	int stepdy = (dy > 0 ? 1 : -1);
        float distance = sqrtf(dx * dx + dy * dy);

        if(distance >= 2){
                dx = (int)(round(dx / distance));
                dy = (int)(round(dy / distance));

                if(can_walk(engine, actor->x + dx, actor->y + dy)){
                        actor->x += dx;
                        actor->y += dy;
                }else if(can_walk(engine, actor->x + stepdx, actor->y)){
			actor->x += stepdx;
		}else if(can_walk(engine, actor->x, actor->y + stepdy)){
			actor->y += stepdy;
		}
        }else if(actor->attacker){
                actor->attacker->attack(engine, actor, engine->player);
                return false;
        }

        return true;
}

void monster_update(struct Engine *engine, struct Actor *actor){
	/* Check if the agent is alive */
        if(actor->destructible && is_dead(actor)){
                return;
        }
	
        if(is_in_fov(engine->map, actor->x, actor->y)){
                /* We can see the player, start tracking him */
		actor->ai->move_count = TRACKING_TURNS;
        }else{
		(actor->ai->move_count)--;
	}
	
	if(actor->ai->move_count > 0){
		actor->ai->move_or_attack(engine, actor, engine->player->x, engine->player->y);
	}
}

/* Transform a monster into a rotting corpse */
void monster_die(struct Engine *engine, struct Actor *actor){
        printf("%s is dead.\n", actor->name);
        /* Call the common die function */
        die(engine, actor);
}
