#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "actor.h"
#include "util.h"

extern void compute_fov(struct engine *engine);

/*** Common functions ***/
float take_damage(struct engine *engine, struct actor *target, float damage){
        /* Reduce the damage by the fraction that the target can deflect */
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

/* 
   The common update function that calls the intelligent update function
   if present
*/
void common_update(struct engine *engine, struct actor *actor){
        /* 
           Do things that are common to all actors.  nothing at the
           moment.
        */
        if(actor->ai){
                actor->ai->update(engine, actor);
        }
}

void free_actors(TCOD_list_t actors){
        TCOD_list_clear_and_delete(actors);
}

void init_actor(struct actor **actor, int x, int y, int ch, const char *name,
                TCOD_color_t col,
                void (*render)(struct actor *)){
        struct actor *tmp = malloc(sizeof (struct actor));
        
        tmp->x = x;
        tmp->y = y;
        tmp->ch = ch;
        tmp->name = name;
        tmp->col = col;
        tmp->render = render;
        tmp->update = common_update;
        
        *actor = tmp;
}

void init_ai(struct ai **ai, void (*update)(struct engine *engine, struct actor *actor),
             bool (*move_or_attack)(struct engine *engine, struct actor *actor, int targetx, int targety)){
        struct ai *tmp = malloc(sizeof *tmp);
        tmp->update = update;
        tmp->move_or_attack = move_or_attack;
        *ai = tmp;        
}

void init_attacker(struct attacker **attacker, float power, 
                   void (*attack)(struct engine *engine, struct actor *dealer, struct actor *target)){
        struct attacker *tmp = malloc(sizeof *tmp);
        tmp->attack = attack;
        tmp->power = power;
        *attacker = tmp;        
}

void init_destructible(struct destructible **destructible, float max_hp,
                       float hp, float defence, const char *corpse_name,
                       float (*take_damage)(struct engine *engine, struct actor *target, float damage),
                       void (*die)(struct engine *engine, struct actor *actor)){
        struct destructible *tmp = malloc(sizeof *tmp);
        
        tmp->die = monster_die;
        tmp->defence = 2;
        tmp->corpse_name = "dead orc";
        tmp->take_damage = take_damage;
        tmp->max_hp = 15;
        tmp->hp = tmp->max_hp;
        tmp->defence = 2;
        *destructible = tmp;
}

void render_actor(struct actor *actor){
        TCOD_console_set_char(NULL, actor->x, actor->y, actor->ch);
        TCOD_console_set_char_foreground(NULL, actor->x, actor->y, actor->col);
}

void attack(struct engine *engine, struct actor *dealer, struct actor *target){
        float power = dealer->attacker->power;
        float defence = target->destructible->defence;
        if(target->destructible && !is_dead(target)){
                if(power - defence > 0){
                        engine->gui->message(engine,
                                             dealer == engine->player ? TCOD_red : TCOD_light_grey,
                                             "%s attacks %s for %g hit points.\n",
                                             dealer->name, target->name, power - defence);
                }else{
                        engine->gui->message(engine,
                                             TCOD_light_grey,
                                             "%s attacks %s but it has no effect!\n",
                                             dealer->name, target->name);            
                }
                target->destructible->take_damage(engine, target, power);
        }else{
                engine->gui->message(engine, TCOD_light_grey, "%s attacks %s in vain.\n", dealer->name, target->name);
        }
}

bool is_dead(struct actor *actor){
	if(actor->destructible != NULL){
		return actor->destructible->hp <= 0;
	}
	return false;
}

/* Transform the actor into a corpse */
void die(struct engine *engine, struct actor *actor){
	actor->ch = '%';
	actor->col = TCOD_dark_red;
	actor->name = actor->destructible->corpse_name;
	actor->blocks = false;
	/* make sure corpses are drawn before living actors */
        send_to_back(engine, actor);
                
}

/*** Player functions ***/
void make_player(struct actor **actor, int x, int y){
        init_actor(actor, x, y, '@', "you", TCOD_white, render_actor);

        struct actor *tmp = *actor;
        /* Artificial intelligence */
        init_ai(&(tmp->ai), player_update, player_move_or_attack);
	
        /* Init attacker */
        init_attacker(&(tmp->attacker), 10, attack);

        /* Init destructible */
        init_destructible(&(tmp->destructible), 100, 100, 4, "your dead body", take_damage, player_die);
}

bool player_move_or_attack(struct engine *engine, struct actor *actor, int targetx, int targety){
        if(is_wall(engine->map, targetx, targety)){
                return false;
        }
       
        /* Look for actors to attack */
        struct actor **iter;
        for(iter = (struct actor **)TCOD_list_begin(engine->actors);
            iter != (struct actor **)TCOD_list_end(engine->actors);
            iter++){
                if((*iter)->destructible && !is_dead(*iter) &&
                   (*iter)->x == targetx && (*iter)->y == targety){
                        /* There is an actor there, cat't walk */
                        actor->attacker->attack(engine, actor, *iter);
                        return false;
                }
        }

        /* Look for corpses */
        for(iter = (struct actor **)TCOD_list_begin(engine->actors);
            iter != (struct actor **)TCOD_list_end(engine->actors);
            iter++){
                if((*iter)->destructible && is_dead(*iter) &&
                   (*iter)->x == targetx && (*iter)->y == targety){
                        engine->gui->message(engine, TCOD_light_gray, "There's a %s here\n", (*iter)->name);
                }
        }

        actor->x = targetx;
        actor->y = targety;

        return true;
}

void player_update(struct engine *engine, struct actor *actor){
	if(actor->destructible && is_dead(actor)){
		return;
	}

        int dx = 0, dy = 0;
        if(engine->key.pressed){
                switch(engine->key.vk) {
                case TCODK_UP : dy = -1; break;
                case TCODK_DOWN : dy = 1; break;
                case TCODK_LEFT : dx = -1; break;
                case TCODK_RIGHT : dx = 1; break;
                default:break;
                }
        }
        if (dx != 0 || dy != 0) {
                engine->game_status = NEW_TURN;
                if(actor->ai->move_or_attack(engine, actor, actor->x + dx, actor->y + dy)) {
                        compute_fov(engine);
                }
        }
}

/* Transform the actor into a decaying corpse */
void player_die(struct engine *engine, struct actor *actor){
        engine->gui->message(engine, TCOD_red, "You die.\n");
        /* Call the common death function */
        die(engine, actor);
        engine->game_status = DEFEAT;
}

/*** Monster functions ***/
void make_monster(struct actor **actor, int x, int y, const char ch, const char *name, TCOD_color_t col,
                  float power, float max_hp, float hp, float defence, const char *corpse_name){
        init_actor(actor, x, y, ch, name, col, render_actor);

        struct actor *tmp = *actor;
        /* Artificial intelligence */
        init_ai(&(tmp->ai), monster_update, monster_move_or_attack);

        /* Init attacker */
        init_attacker(&(tmp->attacker), power, attack);

        /* Init destructible */
        init_destructible(&(tmp->destructible), max_hp, hp, defence, corpse_name, take_damage, monster_die);
}

void make_orc(struct actor **actor, int x, int y){
        make_monster(actor, x, y, 'o', "orc", TCOD_desaturated_green, 5, 15, 15, 2, "dead orc");
}

void make_troll(struct actor **actor, int x, int y){
        make_monster(actor, x, y, 'T', "troll", TCOD_darker_green, 6, 20, 20, 3, "troll carcass");
}

bool monster_move_or_attack(struct engine *engine, struct actor *actor, int targetx, int targety){
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

void monster_update(struct engine *engine, struct actor *actor){
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
void monster_die(struct engine *engine, struct actor *actor){
        engine->gui->message(engine, TCOD_light_grey, "%s is dead.\n", actor->name);
        /* Call the common die function */
        die(engine, actor);
}
