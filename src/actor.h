/*
  Copyright (C) 2016 Sergi Pasoev.

  This pragram is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at
  your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

  Written by Sergi Pasoev <s.pasoev@gmail.com>

*/

#ifndef ACTOR_H
#define ACTOR_H

#include "libtcod.h"
#include "engine.h"

static const int TRACKING_TURNS = 3;

struct engine;
struct actor;

struct ai{
	int move_count; /* allow monsters to track the player */
        void (*update)(struct engine *engine, struct actor *actor);
        bool (*move_or_attack)(struct engine *engine, struct actor *actor, int targetx, int targety);
        struct ai *old_ai; /* confused actors have their previous minds saved here. */
        int num_turns;
};

/*
 * An actor that has this structure allocated is able to make attacks
 * that are defined by:
 *
 * - power - how mach damage is dealt.
 * - attack - the function that calculates and deals the damage considering
 * the power parameter as a seed.
 */
struct attacker{
        struct actor *weapon; /* */
        float power;
        void (*attack)(struct engine *engine, struct actor *dealer, struct actor *target);
};

/*
 * An actor that has this structure allocated is vulnerable to
 * attacks. As an added effect, this actor will require food and rest
 * to sustain himself.
 */
struct destructible{
        float stomach; /* number of food units eaten */
        float max_stomach; /* max number of units the actor can eat */
        float max_hp; /* maximum health points */
        float hp; /* current health points */
        float defence; /* hit points deflected */
        const char *corpse_name; /* the actor's name once dead/destroyed */
        float (*take_damage)(struct engine *engine, struct actor *target, float damage);
        void (*die)(struct engine *engine, struct actor *actor);
};

/*
 * A structure used to represent wands, potions and all other usable
 * items.
 */
struct pickable{
        bool auto_pickup; /* desired by every actor, they pick it without pressing 'g'*/
        float targetting_range; /* range at which the target can be selected */
        float (*calculate_food_cost)(struct actor *actor, struct actor *item);
        float default_food_cost; /* the amount by which hunger
                                    increases. *DO NOT* access this
                                    directly, use the
                                    calculate_food_cost function */
        float range; /* range the item has effect starting from the target tile */
        float power; /* damage dealt if an attacker item, hit_points restored if a healer */
        bool (*use)(struct engine *engine, struct actor *actor, struct actor *item);
};

/* 
 * NOTE: Not used yet. Use the *pickable* structure instead and use
 * the method *use*
 */
struct edible{
        bool (*eat)(struct actor *actor, struct actor *food);
};

/* 
 * Capacity is currently counted by the number of items and *not* by
 * weight or other quality of the items.
 */
struct container{
        int capacity; /* The maximum number of items (actors) in it. */
        TCOD_list_t inventory; /* */
};

struct actor{
        int x;
        int y;
        char ch;
        bool blocks; /* can we walk on this actor? */
        struct attacker *attacker;
	struct destructible *destructible;
	struct ai *ai;
        struct pickable *pickable;
        struct container *inventory;
        const char *name;
        TCOD_color_t col;
        void (*update)(struct engine *engine, struct actor *actor);
        void (*render)(struct actor *actor); /* Draw an actor to the screen */
};

/* 
   Initialise the actor by the values given as parameters. 
   Not all values are provided as arguments. 
   
   NOTE: this is a low level function, intended to be used ONLY by
   wrapper functions e.g. make_orc, make_player, etc.
*/
struct actor * init_actor(int w, int h, int ch, const char *name, TCOD_color_t col, void (*render)(struct actor *));
void free_actor(struct actor *actor);
void free_actors(TCOD_list_t actors);

float get_distance(struct actor *actor, int x, int y);
struct actor *get_actor(struct engine *engine, int x, int y);
struct actor *get_closest_monster(struct engine *engine, int x, int y, float range);

struct container *init_container(int capacity);
struct pickable *init_pickable(float power, float range, bool (*use)(struct engine *engine, struct actor *actor, struct actor *item));
bool inventory_add(struct container *container, struct actor *actor);
void inventory_remove(struct container *container, struct actor *actor);
bool pick(struct engine *engine, struct actor *actor, struct actor *item);
bool drop(struct engine *engine, struct actor *actor, struct actor *item);

/*
 * Calculate the amount by which to increase hunger upon using the
 * item.
 */
float calculate_food_cost(struct actor *actor, struct actor *item);
/* 
 * A common function to all usable items. All item-specific *_use
 * functions should call this as the last statemunt.
 */
bool use(struct actor *actor, struct actor *item);
/*
 * Deals huge damage to the nearest monster. 
 */
bool lightning_wand_use(struct engine *engine, struct actor *actor, struct actor *item);
/*
 * Confuses a target for a few turns, making him walki into and attack
 * anything in random directions.
 */
bool confusion_wand_use(struct engine *engine, struct actor *actor, struct actor *item);
/*
 * Deals huge damage to monsters within certain range.
 */
bool fireball_wand_use(struct engine *engine, struct actor *actor, struct actor *item);

/* 
 * Heals with a fixed amount of hit points.
 */
bool healer_use(struct engine *engine, struct actor *actor, struct actor *item);
/* 
 * Cures poisoning and similar effects of ill health and then heals by
 * a random number of hit points.
 */
bool curing_use(struct engine *engine, struct actor *actor, struct actor *item);
/*
 * Poisons the target for a good number of turns. Deals damage over
 * each turn until cured.
 */
bool potion_of_poison_use(struct engine *engine, struct actor *actor, struct actor *item);
/* 
 * A generic eat function. Units eaten will be equal to the 50% of the
 * max_hp of the corpse, *not* of the actor eating it.
 */
bool eat(struct engine *engine, struct actor *actor, struct actor *food);

struct message get_hunger_status(struct actor *actor);
bool make_hungry(struct actor *actor, float amount);
struct actor *make_food(int x, int y);
struct actor *make_healer_potion(int x, int y);
struct actor *make_curing_potion(int x, int y);
struct actor *make_lightning_wand(int x, int y);
struct actor *make_fireball_wand(int x, int y);
struct actor *make_confusion_wand(int x, int y);

struct actor *make_orc(int x, int y);
struct actor *make_troll(int x, int y);
void make_kobold(struct actor **actor, int x, int y);
struct actor *make_player(int x, int y);
void render_actor(struct actor *actor);
void player_update(struct engine *engine, struct actor *actor);
void monster_update(struct engine *engine, struct actor *actor);
bool player_move_or_attack(struct engine *engine, struct actor *actor, int x, int y);
bool monster_move_or_attack(struct engine *engine, struct actor *actor, int x, int y);
void attack(struct engine *engine, struct actor *dealer, struct actor *target);
bool is_dead(struct actor *actor);
float take_damage(struct engine *engine, struct actor *target, float damage);

void confused_update(struct engine *engine, struct actor *actor);

/* 
   A common function that is called when ANY actor dies. NOTE: Do not
   use this function directly. Use functions associated with specific
   actor types instead, like monster_die, player_die. They call this
   function.
*/
void die(struct engine *engine, struct actor *actor);

/* 
   Called when the player hit points equal to zero. It first calls the
   common die function.
*/
void player_die(struct engine *engine, struct actor *actor);
void monster_die(struct engine *engine, struct actor *actor);
#endif
