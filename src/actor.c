#include <stdlib.h>
#include <stdbool.h>
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

void free_actor(struct actor *actor){
        free(actor);
}

void free_actors(TCOD_list_t actors){
        TCOD_list_clear_and_delete(actors);
}

struct actor *init_actor(int x, int y, int ch, const char *name,
                         TCOD_color_t col,
                         void (*render)(struct actor *)){
        struct actor *tmp = malloc(sizeof *tmp);
        
        tmp->x = x;
        tmp->y = y;
        tmp->ch = ch;
        tmp->name = name;
        tmp->col = col;
        tmp->render = render;
        tmp->update = common_update;

        tmp->ai = NULL;
        tmp->destructible = NULL;
        tmp->attacker = NULL;
        tmp->pickable = NULL;
        tmp->inventory = NULL;
        
        return tmp;
}

struct ai *init_ai(void (*update)(struct engine *engine, struct actor *actor),
                   bool (*move_or_attack)(struct engine *engine, struct actor *actor, int targetx, int targety)){
        struct ai *tmp = malloc(sizeof *tmp);
        tmp->update = update;
        tmp->move_or_attack = move_or_attack;
        return tmp;
}

struct attacker *init_attacker(float power, 
                               void (*attack)(struct engine *engine, struct actor *dealer, struct actor *target)){
        struct attacker *tmp = malloc(sizeof *tmp);
        tmp->attack = attack;
        tmp->power = power;
        return tmp;
}

struct destructible *init_destructible(float max_hp,
                                       float hp, float defence, const char *corpse_name,
                                       float (*take_damage)(struct engine *engine, struct actor *target, float damage),
                                       void (*die)(struct engine *engine, struct actor *actor)){
        struct destructible *tmp = malloc(sizeof *tmp);
        
        tmp->die = monster_die;
        tmp->defence = defence;
        tmp->corpse_name = corpse_name;
        tmp->take_damage = take_damage;
        tmp->max_hp = max_hp;
        tmp->hp = hp;
        return tmp;
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
                                             dealer == engine->player ? TCOD_light_grey : TCOD_red,
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

float heal(struct actor *actor, float amount){
        actor->destructible->hp += amount;
        if(actor->destructible->hp > actor->destructible->max_hp){
                amount -= actor->destructible->hp - actor->destructible->max_hp;
                actor->destructible->hp = actor->destructible->max_hp;                
        }
        return amount;
}

/*** Player functions ***/
struct actor *make_player(int x, int y){
        struct actor *tmp = init_actor(x, y, '@', "you", TCOD_white, render_actor);

        /* Artificial intelligence */
        tmp->ai = init_ai(player_update, player_move_or_attack);
	
        /* Init attacker */
        tmp->attacker = init_attacker(10, attack);

        /* Init destructible */
        tmp->destructible = init_destructible(50, 50, 4, "your dead body", take_damage, player_die);

        /* Init inventory */
        tmp->inventory = init_container(26);

        return tmp;
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

        /* Look for corpses or pickable items */
        for(iter = (struct actor **)TCOD_list_begin(engine->actors);
            iter != (struct actor **)TCOD_list_end(engine->actors);
            iter++){
                struct actor *actor = *iter;
                bool corpse_or_item = (actor->destructible && is_dead(actor))
                        || actor->pickable;
                if(corpse_or_item &&
                   actor->x == targetx && actor->y == targety){
                        engine->gui->message(engine, TCOD_light_gray,
                                             "There's a %s here\n", (*iter)->name);
                }
        }

        actor->x = targetx;
        actor->y = targety;

        return true;
}

struct actor *choose_from_inventory(struct engine *engine, struct actor *actor){
        /* Display the inventory frame */
        TCOD_console_t *con = engine->gui->inventory_con;
        TCOD_color_t color = (TCOD_color_t){200, 180, 50};
        TCOD_console_set_default_foreground(con, color);
        TCOD_console_print_frame(con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true,
                                 TCOD_BKGND_DEFAULT, "inventory");

        /* Display the items with their respective shortcuts */
        TCOD_console_set_default_foreground(con, TCOD_white);
        int shortcut = 'a';
        int y = 1;
        struct actor **iter;
        for(iter = (struct actor **)TCOD_list_begin(actor->inventory->inventory);
            iter != (struct actor **)TCOD_list_end(actor->inventory->inventory);
            iter++){
                struct actor *item = *iter;
                TCOD_console_print(con, 2, y, "(%c) %s", shortcut, item->name);
                y++;
                shortcut++;
        }

        /* Blit the inventory console to the root console. */
        TCOD_console_blit(con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT,
                          NULL, engine->window_w / 2 - INVENTORY_WIDTH / 2,
                          engine->window_h / 2 - INVENTORY_HEIGHT / 2, 1.f, 1.f);
        TCOD_console_flush(NULL);

        /* wait for a key press */
        TCOD_key_t key;
        TCOD_sys_wait_for_event(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
        if (key.vk == TCODK_CHAR ) {
                int actor_index = key.c - 'a';
                if (actor_index >= 0 && actor_index < TCOD_list_size(actor->inventory->inventory)) {
                        return TCOD_list_get(actor->inventory->inventory, actor_index);
                }
        }
        return NULL;
}

void handle_action_key(struct engine *engine, struct actor *actor){
        /* */
        switch(engine->key.c){
        case 'g':{
                bool found = false;
                /* Check for existing items on this loction */
                struct actor **iter;
                for(iter = (struct actor **)TCOD_list_begin(engine->actors);
                    iter != (struct actor **)TCOD_list_end(engine->actors);
                    iter++){
                        struct actor *actor = *iter;
                        if(actor->pickable && actor->x == engine->player->x && actor->y == engine->player->y){
                                /* Try picking up the item */
                                if(pick(engine, engine->player, actor)){
                                        found = true;
                                        engine->gui->message(engine, TCOD_green, "You pick up %s.\n", actor->name);
                                        break;
                                }else if(!found){
                                        found = true;
                                        engine->gui->message(engine, TCOD_green, "You tried to pick up %s. Inventory is full.\n", actor->name);
                                        
                                }
                                
                        }
                }
                
                if(!found){
                        engine->gui->message(engine, TCOD_grey, "There is nothing to pick up here here.\n");                        
                }
                engine->game_status = NEW_TURN;
                break;
        }
        case 'e':
                /* Eat */
                break;
        case 'i' : // display inventory
                {
                        struct actor *item = choose_from_inventory(engine, actor);
                        if (item) {
                                item->pickable->use(actor, item);
                                engine->game_status = NEW_TURN;
                        }
                }
                break;
        default:
                engine->gui->message(engine, TCOD_grey, "Unknown command: %c.\n", engine->key.c);
                break;
        }
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
                case TCODK_CHAR : handle_action_key(engine, actor); break;
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


/* Writes a memorial file */
static void make_memorial(struct actor *actor){
        printf("The program should have written a memorial file in the user's home directory. NOT IMPLEMENTED\n"); 
}

/* Transform the actor into a decaying corpse */
void player_die(struct engine *engine, struct actor *actor){
        engine->gui->message(engine, TCOD_red, "You die.\n");
        /* Call the common death function */
        die(engine, actor);
        make_memorial(actor);
        engine->game_status = DEFEAT;
        
}

/*** Monster functions ***/
struct actor *make_monster(int x, int y, const char ch, const char *name, TCOD_color_t col,
                           float power, float max_hp, float hp, float defence, const char *corpse_name){
        struct actor *tmp = init_actor(x, y, ch, name, col, render_actor);

        /* Artificial intelligence */
        tmp->ai = init_ai(monster_update, monster_move_or_attack);

        /* Init attacker */
        tmp->attacker = init_attacker(power, attack);

        /* Init destructible */
        tmp->destructible = init_destructible(max_hp, hp, defence, corpse_name, take_damage, monster_die);

        return tmp;
}

struct actor *make_orc(int x, int y){
        return make_monster(x, y, 'o', "orc", TCOD_desaturated_green, 8, 15, 15, 2, "dead orc");
}

struct actor *make_troll(int x, int y){
        return make_monster(x, y, 'T', "troll", TCOD_darker_green, 10, 20, 20, 3, "troll carcass");
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

/** Inventory functions **/
struct container *init_container(int capacity){
        struct container *tmp = malloc(sizeof *tmp);
        tmp->capacity = capacity;
        tmp->inventory = TCOD_list_new();

        return tmp;
}

struct pickable *init_pickable(void){
        struct pickable *tmp = malloc(sizeof(*tmp));
        return tmp;
}

struct actor *make_item(int x, int y, const char ch, const char *name, TCOD_color_t col,
                        bool (*use)(struct actor *actor, struct actor *item)){
        struct actor *tmp = init_actor(x, y, ch, name, col, render_actor);
        tmp->pickable = init_pickable();
        tmp->pickable->use = use;
        tmp->blocks = false;
        
        return tmp;
}

struct actor *make_healer_potion(int x, int y){
        return make_item(x, y, '!', "a health potion", TCOD_violet, healer_use);
}

struct actor *make_curing_potion(int x, int y){
        return make_item(x, y, '~', "a curing potion", TCOD_light_green, curing_use);
}

void free_container(struct container *container){
        TCOD_list_clear_and_delete(container->inventory);
        free(container);
}

bool pick(struct engine *engine, struct actor *actor, struct actor *item){
        if(actor->inventory && inventory_add(actor->inventory, item)){
                TCOD_list_remove(engine->actors, item);
                return true;
        }
        return false;
}

bool healer_use(struct actor *actor, struct actor *item){
        /* heal the actor */
        if(actor->destructible){
                float amount_healed = heal(actor, 20);
                if(amount_healed > 0){
                        /* Call the common use function */
                        return use(actor, item);
                }
        }
        return false;
}

/* TODO: At the moment does the same as the HEALTH POTION (See above) */
bool curing_use(struct actor *actor, struct actor *item){
        /* heal the actor */
        if(actor->destructible){
                float amount_healed = heal(actor, 20);
                if(amount_healed > 0){
                        /* Call the common use function */
                        return use(actor, item);
                }
        }
        return false;
}

bool use(struct actor *actor, struct actor *item){
        if(actor->inventory){
                inventory_remove(actor->inventory, item);
                free_actor(item);
                return true;
        }
        return false;
}

void free_pickable(struct pickable *pickable){
        
}

bool inventory_add(struct container *container, struct actor *actor){
        if(container->capacity > 0 && TCOD_list_size(container->inventory) > container->capacity){
                return false;
        }

        TCOD_list_push(container->inventory, actor);
        return true;
}

void inventory_remove(struct container *container, struct actor *actor){
        TCOD_list_remove(container->inventory, actor);
}
