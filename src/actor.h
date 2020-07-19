/*
  Copyright (C) 2016 Sergo Pasoevi.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at
  your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

  Written by Sergo Pasoevi <spasoevi@gmail.com>

*/

#ifndef ACTOR_H
#define ACTOR_H

#include <stdbool.h>

#include "engine.h"
#include "libtcod/libtcod.h"
#include "quest.h"

enum {
    TRACKING_TURNS = 3,
    MAX_XP_LEVEL = 25
};

struct engine;
struct actor;

/* Skills */
#define SKILL_STRENGTH 0
#define SKILL_INTELL 1
#define SKILL_AGILITY 2
#define SKILL_PROVIDENCE 3
#define SKILL_PRUDENCE 4
#define SKILL_FIGHTING 5
#define SKILL_DODGING 6
#define MAX_SKILS 11

struct skill {
    char *name;
    double val;
};

/*
struct skills {
    double strength;
    double intelligence;
    double agility;
    double providence;
    double prudence;
    double fighting;
};
*/

struct ai {
    TCOD_list_t quests;
    int move_count;
    /* allow monsters to track the player */
    struct skill skills[11];
    int xp_level;
    double xp;

    bool (*level_up)(struct engine *engine, struct actor *actor);

    void (*update)(struct engine *engine, struct actor *actor);

    bool (*move_or_attack)(struct engine *engine, struct actor *actor,
                           int target_x, int target_y);

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
    double power;

    struct actor *current_target;

    double (*calc_hit_power)(struct engine *engine, struct actor *dealer,
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
    double stomach;
    /* number of food units eaten */
    double max_stomach;
    /* max number of units the actor can eat */
    double max_hp;
    /* maximum health points */
    double hp;
    /* current health points */
    double defense;
    /* hit points deflected */
    char *corpse_name;

    /* the actor's name once dead/destroyed */
    double (*take_damage)(struct engine *engine, struct actor *dealer,
                          struct actor *target, double damage);

    double (*regen)(struct engine *engine, struct actor *actor);

    void (*die)(struct engine *engine, struct actor *actor, struct actor *killer);
};

/*
 * A structure used to represent wands, potions and all other usable
 * items.
 */
struct pickable {
    bool auto_pickup;
    /* desired by every actor, they pick it without pressing 'g' */
    double targeting_range;

    /* range at which the target can be selected */
    double (*calc_food_cost)(struct actor *actor,
                             struct actor *item);

    /*
     * The amount by which hunger increases. *DO NOT* access this directly,
     * use the calculate_food_cost function
     **/
    double default_food_cost;
    /* range the item has effect starting from the target tile */
    double range;
    /* damage dealt if an attacker item, hit_points restored if a healer */
    double power;

    bool (*use)(struct engine *engine, struct actor *actor, struct actor *item);
    /*
     * Some items have lasting effect, so you don't just use them once, but
     * "wear" (in case of clothes, jewelry, etc), or "wield" them (in case of
     * weapons, etc). Use this function to "stop using" such items.
     */
    bool (*unuse)(struct engine *engine, struct actor *actor, struct actor *item);

    bool (*blow)(struct engine *engine, struct actor *actor, struct actor *weapon,
                 struct actor *target);
};

/*
 * Capacity is currently counted by the number of items and *not* by
 * weight or other qualities of items.
 */
struct container {
    int capacity;
    /* The maximum number of items (actors) in it. */
    TCOD_list_t items; /* */
};

struct actor {
    int x;
    int y;
    int ch;
    /* can we walk on this actor? */
    bool blocking;
    /* display only when in field of view */
    bool fov_only;
    struct attacker *attacker;
    struct life *life;
    struct ai *ai;
    struct pickable *pickable;
    struct container *inventory;
    char *name;
    TCOD_color_t col;

    void (*update)(struct engine *engine, struct actor *actor);

    /* Draw an actor to the screen */
    void (*render)(struct actor *actor);
};

/***************** Actor creation & destruction functions *********************/

/*
   Initialise the actor by the values given as parameters.
   Not all values are provided as arguments.

   NOTE: this is a low level function, intended to be used ONLY by
   wrapper functions e.g. make_orc, make_player, etc.
*/
struct actor *create_actor(int w, int h, int ch, char *name,
                           TCOD_color_t col);

void free_actor(struct actor *actor);

void free_actors(TCOD_list_t actors);

struct ai *create_ai(void (*update)(struct engine *engine, struct actor *actor),
                     bool (*move_or_attack)(struct engine *engine,
                                            struct actor *actor, int target_x,
                                            int target_y));

void free_ai(struct ai *ai);

struct life *create_life(double max_hp, double hp, double defence,
                         char *corpse_name,
                         double (*take_damage)(struct engine *engine,
                                               struct actor *dealer,
                                               struct actor *target, double damage),
                         void (*die)(struct engine *engine, struct actor *actor,
                                     struct actor *killer));

void free_life(struct life *life);

struct attacker *create_attacker(double power,
                                 void (*attack)(struct engine *engine,
                                                struct actor *dealer,
                                                struct actor *target));

void free_attacker(struct attacker *attacker);

struct container *create_container(int capacity);

struct pickable *init_pickable(double power, double range,
                               bool (*use)(struct engine *engine,
                                           struct actor *actor,
                                           struct actor *item));

struct actor *make_item(int x, int y, double power, double range,
                        const int ch, char *name, TCOD_color_t col,
                        bool (*use)(struct engine *engine,
                                    struct actor *actor,
                                    struct actor *item));

/* Food */
struct actor *make_food(int x, int y);

/* Potions */
struct actor *make_healer_potion(int x, int y);

struct actor *make_curing_potion(int x, int y);

struct actor *make_posioning_potion(int x, int y);

/* Wands */
struct actor *make_lightning_wand(int x, int y);

struct actor *make_fireball_wand(int x, int y);

struct actor *make_confusion_wand(int x, int y);

struct actor *make_transfiguration_wand(int x, int y);

/* Weapons */
struct actor *make_kindzal(int x, int y);

/* AI */
struct ai *make_confused_ai(struct actor *actor, int num_turns);

/************************** Actor identifying functions ***********************/
bool is_dead(struct actor *actor);

bool is_edible(struct actor *actor);

bool is_drinkable(struct actor *actor);

bool is_wieldable(struct actor *actor);

bool is_usable(struct actor *actor);

/***************************** Utility functions ******************************/

/* Get distance between the actor and the point specified by x and y. */
double get_distance(struct actor *actor, int x, int y);

/* Get actor at specified location */
struct actor *get_actor(struct engine *engine, int x, int y);
/*** Two related functions. ***/
/* Get monster closest to the (x, y) point within range. Excluding player. */
struct actor *get_closest_monster(struct engine *engine, int x, int y,
                                  double range);

/* Get actor closest to the *actor* within range, including player. */
struct actor *get_closest_actor(struct engine *engine, struct actor *actor,
                                double range);

/*
 * Calculate the amount by which to increase hunger upon using a certain
 * item. Many items have their specific calculating functions, if so,
 * an item pickable will have a pointer to his own calculating
 * function. Call that function instead of this.
 */
double calc_food_cost(struct actor *actor, struct actor *item);

/* Returns -1 if inedible */
double calc_food_value(struct actor *food);

/*
 * Returns message structure that contains the text (hungry, starving,
 * etc) and the appropriate color with which to display in status bar.
 */
struct message get_hunger_status(struct actor *actor);

/*
 * Calculate the actual damage dealt to the target.
 */
double calc_hit_power(struct engine *engine, struct actor *dealer,
                      struct actor *target);

/*
 * Calculate the experience points earned by killing a monster.
 **/
double calc_kill_reward(struct engine *engine, struct actor *actor,
                        struct actor *target);

double calc_next_level_xp(struct engine *engine, struct actor *actor);

bool should_level_up(struct engine *engine, struct actor *actor);

/****************** Functions that act as actions ****************/

bool inventory_add(struct container *container, struct actor *actor);

void inventory_remove(struct container *container, struct actor *actor);
/*
 * Examine the location of the player and try picking the items.
 */
bool try_pick(struct engine *engine);

bool pick(struct engine *engine, struct actor *actor, struct actor *item);

bool drop(struct engine *engine, struct actor *actor, struct actor *item);

bool drop_last(struct engine *engine, struct actor *actor);

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
 * Confuses a target for a few turns, making him walk into and attack
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
bool wield_weapon(struct engine *engine, struct actor *actor,
                  struct actor *weapon);

bool unwield_current_weapon(struct engine *engine, struct actor *actor);

bool blow_kindzal(struct engine *engine, struct actor *actor,
                  struct actor *item, struct actor *target);
/*
 * A generic eating function. Units eaten will be equal to the 50% of
 * the max_hp of the corpse, *not* of the actor eating it.
 */
bool eat(struct engine *engine, struct actor *actor, struct actor *food);

bool make_hungry(struct actor *actor, double amount);

double reward_kill(struct engine *engine, struct actor *actor,
                   struct actor *target);

bool level_up(struct engine *engine, struct actor *actor);

void common_attack(struct engine *engine, struct actor *dealer,
                   struct actor *target);

void attack(struct engine *engine, struct actor *dealer,
            struct actor *target);

double take_damage(struct engine *engine, struct actor *dealer,
                   struct actor *target, double damage);

double heal(struct actor *actor, double amount);

/*
   A common function that is called when ANY actor dies. NOTE: Do not
   use this function directly. Use functions assigned with specific
   actor structures instead, like monster_die, player_die. They call
   this function.
*/
void die(struct engine *engine, struct actor *actor, struct actor *killer);

#endif
