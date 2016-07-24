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

#include <stdlib.h>
#include <stdbool.h>
#include "monsters.h"
#include "util.h"

extern void compute_fov(struct engine *engine);

void common_update(struct engine *engine, struct actor *actor);

/***************** Actor creation & destruction functions *********************/
struct actor *init_actor(int x, int y, char ch, const char *name,
                         TCOD_color_t col, void (*render)(struct actor *))
{
    struct actor *tmp = malloc(sizeof *tmp);

    tmp->x = x;
    tmp->y = y;
    tmp->ch = ch;
    tmp->name = name;
    tmp->col = col;
    tmp->render = render;
    tmp->update = common_update;

    tmp->fov_only = true;

    tmp->ai = NULL;
    tmp->life = NULL;
    tmp->attacker = NULL;
    tmp->pickable = NULL;
    tmp->inventory = NULL;

    return tmp;
}

void free_actor(struct actor *actor)
{
    if (actor == NULL)
        return;
    /*
     * TODO: Free all items in the inventory
     */
    if (actor->attacker)
        free_attacker(actor->attacker);

    if (actor->life)
        free_life(actor->life);

    if (actor->ai)
        free_ai(actor->ai);

    free(actor);
}

void free_actors(TCOD_list_t *actors)
{
    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(actors);
         iter != (struct actor **) TCOD_list_end(actors); iter++) {
        struct actor *actor = *iter;
        free_actor(actor);
    }

    TCOD_list_clear(actors);
}

struct ai *init_ai(void (*update)(struct engine *engine, struct actor *actor),
                   bool(*move_or_attack)(struct engine *engine,
                                         struct actor *actor, int target_x,
                                         int target_y))
{
    struct ai *tmp = malloc(sizeof *tmp);
    tmp->update = update;
    tmp->move_or_attack = move_or_attack;
    tmp->level_up = level_up;
    tmp->xp_level = 1;
    tmp->xp = 0.f;
    tmp->skills.strength = 10;
    tmp->skills.intelligence = 10;
    return tmp;
}

void free_ai(struct ai *ai)
{
    if (ai != NULL) {
        free(ai);
    }
}

struct attacker *init_attacker(float power,
                               void (*attack)(struct engine *engine,
                                              struct actor *dealer,
                                              struct actor *target))
{
    struct attacker *attacker = malloc(sizeof *attacker);
    attacker->attack = attack;
    attacker->power = power;
    attacker->calc_hit_power = calc_hit_power;
    attacker->weapon = NULL;
    return attacker;
}

void free_attacker(struct attacker *attacker)
{
    if (attacker != NULL)
        free(attacker);
}

struct life *init_life(
        float max_hp,
        float hp, float defence,
        const char *corpse_name,
        float (*take_damage)(struct engine *engine, struct actor *dealer,
                             struct actor *target, float damage),
        void (*die)(struct engine *engine, struct actor *actor, struct actor *killer))
{
    struct life *life = malloc(sizeof *life);
    life->die = die;
    life->defence = defence;
    life->corpse_name = corpse_name;
    life->take_damage = take_damage;
    life->max_hp = max_hp;
    life->hp = hp;
    return life;
}

void free_life(struct life *life)
{
    if (life != NULL)
        free(life);
}

/*** Common functions ***/
float take_damage(struct engine *engine, struct actor *dealer,
                  struct actor *target, float damage)
{
    /* Reduce the damage by the fraction that the target can deflect */
    damage -= target->life->defence;
    if (damage > 0) {
        target->life->hp -= damage;
        if (target->life->hp <= 0) {
            target->life->hp = 0; /* prevent hp from goint below zero */
            target->life->die(engine, target, dealer);
        }
    } else {
        damage = 0;
    }
    return damage;
}

/* 
   The common update function that calls the intelligent update function
   if present
*/
void common_update(struct engine *engine, struct actor *actor)
{
    /*
       Do things that are common to all actors.  nothing at the
       moment.
    */
    if (actor->ai)
        actor->ai->update(engine, actor);
}

void confused_update(struct engine *engine, struct actor *actor)
{
    /* Check if the actor is alive */
    if (actor->life && is_dead(actor)) {
        return;
    }

    TCOD_random_t *rng = TCOD_random_get_instance();
    int dx = TCOD_random_get_int(rng, -1, 1);
    int dy = TCOD_random_get_int(rng, -1, 1);
    if (dx != 0 || dy != 0) {
        int destx = actor->x + dx;
        int desty = actor->y + dy;
        if (can_walk(engine, destx, desty)) {
            actor->x = destx;
            actor->y = desty;
        } else {
            struct actor *target =
                    get_actor(engine, destx, desty);
            if (target) {
                actor->attacker->attack(engine, actor,
                                        target);
            }
        }
    }

    actor->ai->num_turns--;
    if (actor->ai->num_turns == 0) {
        struct ai *tmp = actor->ai;
        actor->ai = actor->ai->old_ai;
        free(tmp);
    }
}

void render_actor(struct actor *actor)
{
    TCOD_console_set_char(NULL, actor->x, actor->y, actor->ch);
    TCOD_console_set_char_foreground(NULL, actor->x, actor->y, actor->col);
}

/*
 *
 *
 */
void common_attack(struct engine *engine, struct actor *dealer,
                   struct actor *target)
{
    float power = 0.f, defence = 0.f;
    power = dealer->attacker->calc_hit_power(engine, dealer, target);
    defence = target->life->defence;

    if (target->life && !is_dead(target)) {
        if (power - defence > 0) {
            bool is_player = dealer == engine->player;
            engine->gui->message(
                    engine,
                    is_player ? TCOD_light_grey : TCOD_red,
                    "%s %s %s for %g hit points.\n",
                    dealer->name, is_player ? "attack" : "attacks",
                    target->name, power - defence);
        } else {
            engine->gui->message(
                    engine, TCOD_light_grey,
                    "%s attacks %s but it has no effect!\n",
                    dealer->name, target->name);
        }
        target->life->take_damage(engine, dealer, target, power);
    } else {
        engine->gui->message(engine, TCOD_light_grey,
                             "%s attacks %s in vain.\n",
                             dealer->name, target->name);
    }
}

void attack(struct engine *engine, struct actor *dealer,
            struct actor *target)
{
    /* Determine the attack type */
    if (dealer->attacker->weapon) {
        /* Attack using a weapon */
        struct actor *weapon = dealer->attacker->weapon;
        weapon->pickable->blow(engine, dealer, weapon, target);
    } else {
        /* Make a barehanded attack */
        common_attack(engine, dealer, target);
    }
}

bool is_dead(struct actor *actor)
{
    if (actor->life != NULL)
        return actor->life->hp <= 0;
    return false;
}

/* Transform an actor into a corpse */
void die(struct engine *engine, struct actor *actor, struct actor *killer)
{
    actor->ch = '%';
    actor->col = TCOD_dark_red;
    actor->name = actor->life->corpse_name;
    actor->blocking = false;
    /* make sure corpses are drawn before living actors */
    send_to_back(engine, actor);
}

float heal(struct actor *actor, float amount)
{
    actor->life->hp += amount;
    if (actor->life->hp > actor->life->max_hp) {
        amount -=
                actor->life->hp - actor->life->max_hp;
        actor->life->hp = actor->life->max_hp;
    }
    return amount;
}

/* Get distance between the actor and the (x, y) point on map */
float get_distance(struct actor *actor, int x, int y)
{
    return get_distance_btwn_points(actor->x, actor->y, x, y);
}

struct actor *get_closest_actor(struct engine *engine, struct actor *actor,
                                float range)
{
    struct actor *closest = NULL;
    float best_distance = 1E6f;

    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors);
         iter++) {
        struct actor *tmp = *iter;
        if (tmp != actor && tmp->life && !is_dead(tmp)) {
            float distance =
                    get_distance(tmp, actor->x, actor->y);
            if (distance < best_distance
                && (distance <= range || range == 0.0f)) {
                best_distance = distance;
                closest = tmp;
            }
        }
    }
    return closest;
}

/* Get the closest monster to the point (x, y) within range */
struct actor *get_closest_monster(struct engine *engine, int x, int y,
                                  float range)
{
    struct actor *closest = NULL;
    float best_distance = 1E6f;

    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors);
         iter++) {
        struct actor *actor = *iter;
        if (actor != engine->player && actor->life
            && !is_dead(actor)) {
            float distance = get_distance(actor, x, y);
            if (distance < best_distance
                && (distance <= range || range == 0.0f)) {
                best_distance = distance;
                closest = actor;
            }
        }
    }
    return closest;
}

struct actor *get_actor(struct engine *engine, int x, int y)
{
    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors);
         iter++) {
        struct actor *actor = *iter;
        if (actor->x == x && actor->y == y && actor->life
            && !is_dead(actor))
            return actor;
    }
    return NULL;
}

float calc_hit_power(struct engine *engine, struct actor *dealer, struct
        actor *target)
{
    float power = 0.f;
    if (dealer->attacker->weapon)
        power = dealer->attacker->weapon->pickable->power;
    else
        power = dealer->attacker->power;
    return power;
}

float calc_kill_reward(struct engine *engine, struct actor *actor,
                       struct actor *target)
{
    return 10 + actor->ai->xp;
}

float reward_kill(struct engine *engine, struct actor *actor,
                  struct actor *target)
{
    if (!actor->ai)
        return -1;

    
    float reward = calc_kill_reward(engine, actor, target);

    if (reward > 0) {
        actor->ai->xp += reward;
        engine->gui->message(engine, TCOD_light_grey, "%s is dead. You gain %.0f xp.\n", actor->name, reward);
    }

    return reward;
}

bool should_level_up(struct engine *engine, struct actor *actor)
{
    if (!actor->ai || actor->ai->xp_level == MAX_XP_LEVEL)
        return false;

    float required_xp = 30.f + actor->ai->xp_level * 30.f;

    if (actor->ai->xp > required_xp)
        return true;

    return false;
}

bool level_up(struct engine *engine, struct actor *actor)
{
    if (!actor->ai)
        return false;

    if (actor->ai->xp_level < MAX_XP_LEVEL) {
        actor->ai->xp_level++;
        actor->ai->xp = 0;

        /* increase strength stat */
        actor->ai->skills.strength += 1;
        engine->gui->message(engine, TCOD_light_grey,
                             "You advance to level %d!", actor->ai->xp_level);
        return true;
    }

    return false;
}

bool is_edible(struct actor *actor)
{
    bool is_edible = false;
    if (actor->life && is_dead(actor))
        is_edible = true;
    return is_edible;
}

bool is_drinkable(struct actor *actor)
{
    bool is_drinkable = false;
    if (actor->pickable && !actor->life && actor->ch == '!')
        is_drinkable = true;
    return is_drinkable;
}

bool is_wieldable(struct actor *actor)
{
    bool is_wieldable = false;
    if (actor->pickable && !actor->life && actor->ch == '|')
        is_wieldable = true;
    return is_wieldable;
}

/* A dummy function to return true for all actors */
bool is_usable(struct actor *actor)
{
    bool usable = false;

    if (actor->pickable)
        usable = true;

    return usable;
}

struct container *init_container(int capacity)
{
    struct container *container = malloc(sizeof *container);
    container->capacity = capacity;
    container->inventory = TCOD_list_new();

    return container;
}

struct pickable *init_pickable(float power, float range,
                               bool(*use)(struct engine *engine,
                                          struct actor *actor,
                                          struct actor *item))
{
    struct pickable *pickable = malloc(sizeof *pickable);
    pickable->power = power;
    pickable->range = range;
    pickable->use = use;
    return pickable;
}

struct ai *make_confused_ai(struct actor *actor, int num_turns)
{
    struct ai *tmp = malloc(sizeof *tmp);
    tmp->update = confused_update;
    tmp->move_or_attack = actor->ai->move_or_attack;
    tmp->num_turns = num_turns;
    tmp->old_ai = actor->ai;
    return tmp;
}

/* 
 * Generic item-maker function. Weapon items are made by a different
 * function 
 */
struct actor *make_item(int x, int y, float power, float range,
                        const char ch, const char *name, TCOD_color_t col,
                        bool(*use)(struct engine *engine,
                                   struct actor *actor,
                                   struct actor *item))
{
    struct actor *tmp = init_actor(x, y, ch, name, col, render_actor);
    tmp->pickable = init_pickable(power, range, use);
    tmp->pickable->calc_food_cost = calc_food_cost;
    tmp->blocking = false;

    return tmp;
}

/* Wands */
struct actor *make_lightning_wand(int x, int y)
{
    struct actor *item =
            make_item(x, y, 30, 10, '/', "a lightning wand", TCOD_white,
                      lightning_wand_use);
    item->pickable->default_food_cost = 13;
    return item;
}

struct actor *make_fireball_wand(int x, int y)
{
    struct actor *item = make_item(x, y, 10, 3, '/', "a fireball wand",
                                   TCOD_dark_orange,
                                   fireball_wand_use);
    item->pickable->targeting_range = 8;
    item->pickable->default_food_cost = 15;
    return item;
}

struct actor *make_confusion_wand(int x, int y)
{
    struct actor *item =
            make_item(x, y, 0, 1, '/', "a wand of confusion",
                      TCOD_light_green, confusion_wand_use);
    item->pickable->targeting_range = 8;
    item->pickable->default_food_cost = 8;
    return item;
}

/** Potions **/
struct actor *make_potion_of_posion(int x, int y)
{
    return make_item(x, y, 10, 0, '6', "a potion of poisoning",
                     TCOD_violet, potion_of_poison_use);
}

struct actor *make_healer_potion(int x, int y)
{
    return make_item(x, y, 10, 0, '!', "a health potion", TCOD_violet,
                     healer_use);
}

struct actor *make_curing_potion(int x, int y)
{
    float amount = 5;    /* TODO: this needs to be made random. */
    return make_item(x, y, amount, 0, '!', "a curing potion", TCOD_violet,
                     curing_use);
}

/** Food **/

struct actor *make_food(int x, int y)
{
    struct actor *food =
            make_monster(x, y, '%', "food", TCOD_orange, 8, 50, 0, 2,
                         "food", monster_update);
    food->pickable = init_pickable(0, 0, eat);
    return food;
}

/** Weapons **/

struct actor *make_weapon(int x, int y, float power,
                          const char ch, const char *name, TCOD_color_t col,
                          bool(*wield)(struct engine *engine,
                                       struct actor *actor,
                                       struct actor *item),
                          bool(*blow)(struct engine *engine,
                                      struct actor *actor,
                                      struct actor *item,
                                      struct actor *target))
{
    struct actor *tmp = make_item(x, y, power, 0, ch, name, col, weapon_wield);
    tmp->pickable->blow = kindzal_blow;
    return tmp;

}

struct actor *make_kindzal(int x, int y)
{
    return make_weapon(x, y, 20, '|', "a Kindzal", TCOD_silver, weapon_wield,
                       kindzal_blow);
}

void free_container(struct container *container)
{
    TCOD_list_clear_and_delete(container->inventory);
    free(container);
}

bool try_pick(struct engine *engine)
{
    bool found = false;
    /* Check for existing items on this loction */
    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors); iter++) {
        struct actor *actor = *iter;
        if (actor->pickable && actor->x == engine->player->x &&
            actor->y == engine->player->y) {
            /* Try picking up the item */
            if (pick(engine, engine->player, actor)) {
                found = true;
                engine->gui->message(engine, TCOD_green, "You pick up %s.\n",
                                     actor->name);
                break;
            } else if (!found) {
                found = true;
                engine->gui->message(
                        engine, TCOD_green,
                        "You tried to pick up %s. Inventory is full.\n",
                        actor->name);
            }
        }
    }

    if (!found)
        engine->gui->message(engine, TCOD_grey,
                             "There is nothing to pick up here here.\n");
    engine->game_status = NEW_TURN;
    return found;
}

bool pick(struct engine *engine, struct actor *actor, struct actor *item)
{
    if (actor->inventory && inventory_add(actor->inventory, item)) {
        TCOD_list_remove(engine->actors, item);
        return true;
    }
    return false;
}

bool drop(struct engine *engine, struct actor *actor,
          struct actor *item)
{
    if (actor->inventory) {
        inventory_remove(actor->inventory, item);
        TCOD_list_push(engine->actors, item);
        item->x = actor->x;
        item->y = actor->y;
        engine->gui->message(engine, TCOD_light_grey,
                             "%s drops a %s.\n", actor->name,
                             item->name);
        return true;
    }
    return false;
}

bool drop_last(struct engine *engine, struct actor *actor)
{
    struct actor **last_item =
            (struct actor **) TCOD_list_end(actor->inventory->inventory);
    last_item--;
    engine->game_status = NEW_TURN;
    return drop(engine, actor, *last_item);
}

/** Item use functions **/

bool lightning_wand_use(struct engine *engine, struct actor *actor,
                        struct actor *item)
{
    struct actor *closest =
            get_closest_monster(engine, actor->x, actor->y,
                                item->pickable->range);
    if (!closest) {
        engine->gui->message(engine, TCOD_light_grey,
                             "No monsters in range to strike.\n");
        return false;
    }

    /* Make sure you aren't too hungry to invoke that wand. */
    if (make_hungry
            (actor, item->pickable->calc_food_cost(actor, item))) {
        /*
         * Store the target monster name as it will be changed to the
         * corpse name upon fatal impact.
         */
        const char *name = closest->name;
        float dmg_dealt =
                closest->life->take_damage(engine, actor, closest,
                                           item->pickable->power);
        engine->gui->message(engine, TCOD_light_yellow,
                             "A lightning bolt strikes %s with the damage of %g.\n",
                             name, dmg_dealt);
        return use(actor, item);
    } else {
        engine->gui->message(engine, TCOD_light_grey,
                             "You are too hungry to invoke that wand.\n");
        return false;
    }
}

bool fireball_wand_use(struct engine *engine, struct actor *dealer,
                       struct actor *item)
{
    engine->gui->message(engine, TCOD_cyan,
                         "Left-click a target tile for the fireball,\nor right-click to cancel.");
    int x = 0, y = 0;
    if (!pick_tile(engine, &x, &y, item->pickable->targeting_range))
        return false;

    if (make_hungry(dealer, item->pickable->calc_food_cost(dealer, item))) {
        engine->gui->message(engine, TCOD_orange,
                             "the fireball explodes, burning everything within %g tiles.",
                             item->pickable->range);
        struct actor **iter;
        for (iter = (struct actor **) TCOD_list_begin(engine->actors);
             iter != (struct actor **) TCOD_list_end(engine->actors);
             iter++) {
            struct actor *actor = *iter;
            if (actor->life && !is_dead(actor) &&
                get_distance(actor, x, y) <= item->pickable->range) {
                engine->gui->message(engine, TCOD_orange,
                                     "%s gets burned for %g hit points.",
                                     actor->name, item->pickable->power);
                actor->life->take_damage(engine, dealer, actor,
                                         item->pickable->power);
            }
        }
        return use(dealer, item);
    } else {
        engine->gui->message(engine, TCOD_light_grey,
                             "You are too hungry to invoke that wand.\n");
        return false;
    }
}

bool confusion_wand_use(struct engine *engine, struct actor *actor,
                        struct actor *item)
{
    engine->gui->message(engine, TCOD_cyan,
                         "Left-click an enemy to confuse it,\nor right-click to cancel.");
    int x = 0, y = 0;
    if (!pick_tile(engine, &x, &y, item->pickable->targeting_range))
        return false;

    if (make_hungry
            (actor, item->pickable->calc_food_cost(actor, item))) {
        struct actor *target = get_actor(engine, x, y);
        if (!target)
            return false;

        struct ai *confused_ai = make_confused_ai(target, 5);
        target->ai = confused_ai;
        engine->gui->message(
                engine,
                TCOD_light_green,
                "The eyes of %s look vacant,\nas he starts to stumble around!",
                target->name
        );
        return use(actor, item);
    } else {
        engine->gui->message(engine, TCOD_light_grey,
                             "You are too hungry to invoke that wand.\n");
        return false;
    }

}

bool potion_of_poison_use(struct engine *engine, struct actor *actor,
                          struct actor *item)
{
    /* heal the actor */
    if (actor->life) {
        float amount_healed = heal(actor, item->pickable->power);
        if (amount_healed > 0)
            /* Call the common use function */
            return use(actor, item);
    }
    return false;
}

bool use(struct actor *actor, struct actor *item)
{
    if (actor->inventory) {
        inventory_remove(actor->inventory, item);
        free_actor(item);
        return true;
    }
    return false;
}

bool eat(struct engine *engine, struct actor *actor, struct actor *food)
{
    bool used = false;
    if (food->life && is_dead(food)) {
        float can_eat =
                actor->life->max_stomach -
                actor->life->stomach;
        float food_value = calc_food_value(food);

        if (food_value <= can_eat) {
            actor->life->stomach += food_value;
            engine->gui->message(engine, TCOD_green,
                                 "You finish eating %s.\n",
                                 food->
                                         life->corpse_name);
            used = use(actor, food);
        }
    }

    if (!used)
        engine->gui->message(engine, TCOD_green,
                             "You aren't hungry enough to eat.\n");

    return used;
}

bool healer_use(struct engine *engine, struct actor *actor,
                struct actor *item)
{
    /* heal the actor */
    if (actor->life) {
        float amount_healed = heal(actor, item->pickable->power);
        if (amount_healed > 0) {
            /* Call the common use function */
            engine->gui->message(engine, TCOD_green,
                                 "You finish drinking %s.\n",
                                 item->name);
            engine->gui->message(engine, TCOD_green,
                                 "You feel somewhat better.\n");
            return use(actor, item);
        }
    }
    return false;
}

/*
 * TODO: At the moment does the same as the HEALTH POTION (See above)
 **/
bool curing_use(struct engine *engine, struct actor *actor,
                struct actor *item)
{
    /* Cure the poisoning. NOT YET IMPLEMENTED */

    /*
     * Then heal the actor. Same as health potion but restores hp
     * by a random, usually lower amount.
     */
    return healer_use(engine, actor, item);
}

/* TODO: Add message to log */
bool weapon_wield(struct engine *engine, struct actor *actor,
                  struct actor *weapon)
{
    bool did_replace = false;
    /* Unwield the previous weapon and put it back into the inventory */
    if (actor->attacker->weapon)
        did_replace = inventory_add(actor->inventory, actor->attacker->weapon);

    /* Wield the new weapon */
    actor->attacker->weapon = weapon;
    return did_replace;
}

/*
 * Unlike potion_use functions, the weapon_blow functions are called
 * every time you hit the enemy with them.
 */
bool kindzal_blow(struct engine *engine, struct actor *actor,
                  struct actor *weapon, struct actor *target)
{
    if (target->life)
        common_attack(engine, actor, target);
    return false;
}

struct message get_hunger_status(struct actor *actor)
{
    struct message status;
    status.text = "";

    if (actor->life->stomach < 10) {
        status.col = TCOD_lightest_red;
        status.text = "fainting";
    } else if (actor->life->stomach < 40) {
        status.col = TCOD_light_red;
        status.text = "starving";
    } else if (actor->life->stomach < 80) {
        status.col = TCOD_red;
        status.text = "very hungry";
    } else if (actor->life->stomach < 100) {
        status.col = TCOD_dark_red;
        status.text = "hungry";
    } else if (actor->life->stomach >
               actor->life->max_stomach - 10) {
        status.col = TCOD_green;
        status.text = "full";
    }
    return status;
}

float calc_food_value(struct actor *food)
{
    float value = 0;
    if (!food->life)
        value = -1;
    else
        value = food->life->max_hp;
    return value;
}

/* 
 * Actions that require energy make the actor hungry by amount. Return
 * -1 if the user will starve to death.
 */
bool make_hungry(struct actor *actor, float amount)
{
    if (actor->life
        && actor->life->stomach - amount >= 0) {
        actor->life->stomach -= amount;
        return true;
    }
    return false;

}

float calc_food_cost(struct actor *actor, struct actor *item)
{
    return item->pickable->default_food_cost;
}

void free_pickable(struct pickable *pickable)
{
    if (pickable != NULL)
        free(pickable);
}

bool inventory_add(struct container *container, struct actor *actor)
{
    if (container->capacity > 0
        && TCOD_list_size(container->inventory) > container->capacity)
        return false;

    TCOD_list_push(container->inventory, actor);
    return true;
}

void inventory_remove(struct container *container, struct actor *actor)
{
    TCOD_list_remove(container->inventory, actor);
}
