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

enum {
    TRACKING_TURNS = 3,
    MAX_XP_LEVEL = 25
};

struct engine;
struct actor;

struct skills {
    int strength;
    int intelligence;
};

struct ai {
    int move_count;
    /* allow monsters to track the player */
    struct skills *skills;
    int xp_level;
    float xp;

    bool (*level_up)(struct engine *engine, struct actor *actor);

    void (*update)(struct engine *engine, struct actor *actor);

    bool(*move_or_attack)(struct engine *engine,
                          struct actor *actor, int targetx,
                          int targety);

    struct ai *old_ai;
    /* confused actors have their previous minds saved here. */
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
struct attacker {
    struct actor *weapon;
    /* */
    float power;

    float (*calc_hit_power)(struct engine *engine, struct actor *dealer,
                            struct actor *target);

    void (*attack)(struct engine *engine, struct actor *dealer,
                   struct actor *target);
};

/*
 * An actor that has this structure allocated is vulnerable to
 * attacks. As an added effect, this actor will require food and rest
 * to sustain himself.
 */
struct life {
    float stomach;
    /* number of food units eaten */
    float max_stomach;
    /* max number of units the actor can eat */
    float max_hp;
    /* maximum health points */
    float hp;
    /* current health points */
    float defence;
    /* hit points deflected */
    const char *corpse_name;

    /* the actor's name once dead/destroyed */
    float (*take_damage)(struct engine *engine, struct actor *dealer,
                         struct actor *target, float damage);

    void (*die)(struct engine *engine, struct actor *actor);
};

/*
 * A structure used to represent wands, potions and all other usable
 * items.
 */
struct pickable {
    bool auto_pickup;
    /* desired by every actor, they pick it without pressing 'g' */
    float targetting_range;

    /* range at which the target can be selected */
    float (*calc_food_cost)(struct actor *actor,
                            struct actor *item);

    float default_food_cost;
    /* the amount by which hunger
                       increases. *DO NOT* access this
                       directly, use the
                       calculate_food_cost function */
    /* range the item has effect starting from the target tile */
    float range;
    /* damage dealt if an attacker item, hit_points restored if a healer */
    float power;
    bool(*use)(struct engine *engine, struct actor *actor,
               struct actor *item);

    bool(*blow)(struct engine *engine, struct actor *actor,
                struct actor *weapon, struct actor *target);
};

/* 
 * Capacity is currently counted by the number of items and *not* by
 * weight or other quality of the items.
 */
struct container {
    int capacity;
    /* The maximum number of items (actors) in it. */
    TCOD_list_t inventory;    /* */
};

struct actor {
    int x;
    int y;
    char ch;
    bool blocks;        /* can we walk on this actor? */
    bool fov_only;
    /* display only when in field of view */
    struct attacker *attacker;
    struct life *life;
    struct ai *ai;
    struct pickable *pickable;
    struct container *inventory;
    const char *name;
    TCOD_color_t col;

    void (*update)(struct engine *engine, struct actor *actor);
    void (*render)(struct actor *actor);    /* Draw an actor to the screen */
};

/* 
   Initialise the actor by the values given as parameters. 
   Not all values are provided as arguments. 
   
   NOTE: this is a low level function, intended to be used ONLY by
   wrapper functions e.g. make_orc, make_player, etc.
*/
struct actor *init_actor(int w, int h, int ch, const char *name,
                         TCOD_color_t col,
                         void (*render)(struct actor *));

void free_actor(struct actor *actor);

void free_actors(TCOD_list_t actors);

/* Get distance between the actor and the point specified by x and y. */
float get_distance(struct actor *actor, int x, int y);

/* Get actor at specified location */
struct actor *get_actor(struct engine *engine, int x, int y);
/*** Two related functions. ***/
/* Get monster closest to the (x, y) point within range. Excluding player. */
struct actor *get_closest_monster(struct engine *engine, int x, int y,
                                  float range);

/* Get actor closest to the *actor* within range, including player. */
struct actor *get_closest_actor(struct engine *engine, struct actor *actor,
                                float range);

struct container *init_container(int capacity);

struct pickable *init_pickable(float power, float range,
                               bool(*use)(struct engine *engine,
                                          struct actor *actor,
                                          struct actor *item));

bool inventory_add(struct container *container, struct actor *actor);

void inventory_remove(struct container *container, struct actor *actor);
/*
 * Examine the location of the player and try picking the items.
 */
bool try_pick(struct engine *engine);

bool pick(struct engine *engine, struct actor *actor, struct actor *item);

bool drop(struct engine *engine, struct actor *actor, struct actor *item);

bool drop_last(struct engine *engine, struct actor *actor);

/*** Item factory functions ***/

struct actor *make_item(int x, int y, float power, float range,
                        const char ch, const char *name, TCOD_color_t col,
                        bool(*use)(struct engine *engine,
                                   struct actor *actor,
                                   struct actor *item));

/* Food */
struct actor *make_food(int x, int y);

/* Potions */
struct actor *make_healer_potion(int x, int y);

struct actor *make_curing_potion(int x, int y);

/* Wands */
struct actor *make_lightning_wand(int x, int y);

struct actor *make_fireball_wand(int x, int y);

struct actor *make_confusion_wand(int x, int y);

struct actor *make_transfiguration_wand(int x, int y);

/* Weapons */
struct actor *make_kindzal(int x, int y);

/* Other tools */

/*
 * Calculate the amount by which to increase hunger upon using an
 * item. Many items have their specific calculating functions, if so,
 * an item pickable will have a pointer to his own calculating
 * function. Call that function instead of this.
 */
float calc_food_cost(struct actor *actor, struct actor *item);

/* Returns -1 if inedible */
float calc_food_value(struct actor *food);
/* 
 * A common function to all usable items. All item-specific *_use
 * functions should call this as the last statemunt.
 */
bool use(struct actor *actor, struct actor *item);
/*
 * Deals huge damage to the nearest monster. 
 */
bool lightning_wand_use(struct engine *engine, struct actor *actor,
                        struct actor *item);
/*
 * Confuses a target for a few turns, making him walki into and attack
 * anything in random directions.
 */
bool confusion_wand_use(struct engine *engine, struct actor *actor,
                        struct actor *item);
/*
 * Transforms the target into a random animal.
 */
bool transfiguration_wand_use(struct engine *engine, struct actor *actor,
                              struct actor *item);
/*
 * Deals huge damage to monsters within certain range.
 */
bool fireball_wand_use(struct engine *engine, struct actor *actor,
                       struct actor *item);

/* 
 * Heals with a fixed amount of hit points.
 */
bool healer_use(struct engine *engine, struct actor *actor,
                struct actor *item);
/* 
 * Cures poisoning and similar effects of ill health and then heals by
 * a random number of hit points. Usually heals by less number than
 * the health potion.
 */
bool curing_use(struct engine *engine, struct actor *actor,
                struct actor *item);
/*
 * Poisons the target for a good number of turns. Deals damage over
 * each turn until cured.
 */
bool potion_of_poison_use(struct engine *engine, struct actor *actor,
                          struct actor *item);

/* Weapons */
bool weapon_wield(struct engine *engine, struct actor *actor,
                  struct actor *weapon);

bool kindzal_blow(struct engine *engine, struct actor *actor,
                  struct actor *item, struct actor *target);
/* 
 * A generic eating function. Units eaten will be equal to the 50% of
 * the max_hp of the corpse, *not* of the actor eating it.
 */
bool eat(struct engine *engine, struct actor *actor, struct actor *food);

/*
 * Returns message structure that contains the text (hungry, starving,
 * etc) and the appropriate color with which to display in status bar.
 */
struct message get_hunger_status(struct actor *actor);

bool make_hungry(struct actor *actor, float amount);

float calc_hit_power(struct engine *engine, struct actor *dealer, struct
        actor *target);

/* */
float calc_kill_reward(struct engine *engine, struct actor *actor,
                       struct actor *target);

float reward_kill(struct engine *engine, struct actor *actor,
                  struct actor *target);

bool should_level_up(struct engine *engine, struct actor *actor);

bool level_up(struct engine *engine, struct actor *actor);

/*** Monster factory functions ***/
struct actor *make_player(int x, int y);

struct actor *make_orc(int x, int y);

struct actor *make_goblin(int x, int y);

struct actor *make_troll(int x, int y);

struct actor *make_dragon(int x, int y);

void render_actor(struct actor *actor);

void player_update(struct engine *engine, struct actor *actor);

void monster_update(struct engine *engine, struct actor *actor);

/*
 * Attacks player and other monsters as well.
 */
void dragon_update(struct engine *engine, struct actor *actor);

/*
 * Temporary AI for actors that are confused. They move to random
 * directions attacking everything in their way. 
 */
void confused_update(struct engine *engine, struct actor *actor);

/*
 * Monsters that wander do not immediately attack anybody, but they
 * start behaving as typical attacker intelligences once attacked.
 */
void wandering_update(struct engine *engine, struct actor *actor);

bool player_move_or_attack(struct engine *engine, struct actor *actor,
                           int x, int y);

bool monster_move_or_attack(struct engine *engine, struct actor *actor,
                            int x, int y);

/*
 * Ally update function, behaves like typical monsters except that it
 * attacks and tracks everybody except you. When no monsters are
 * around, it follows you.
 */
void ally_update(struct engine *engine, struct actor *actor);

void attack(struct engine *engine, struct actor *dealer,
            struct actor *target);

bool is_dead(struct actor *actor);

float take_damage(struct engine *engine, struct actor *dealer,
                  struct actor *target, float damage);

/* 
   A common function that is called when ANY actor dies. NOTE: Do not
   use this function directly. Use functions assigned with specific
   actor structures instead, like monster_die, player_die. They call
   this function.
*/
void die(struct engine *engine, struct actor *actor);

/* 
 * Called when the player hit points equal to zero. It first calls the
 * common die function. 
 *
 * Also calls the function that creates character memorial file and
 * deletes saved game if present.
 */
void player_die(struct engine *engine, struct actor *actor);

void monster_die(struct engine *engine, struct actor *actor);

#endif
