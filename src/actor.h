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
        float range; /* range the item is powerful at */
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
struct actor *get_closest_monster(struct engine *engine, int x, int y, float range);

struct container *init_container(int capacity);
struct pickable *init_pickable(float power, float range, bool (*use)(struct engine *engine, struct actor *actor, struct actor *item));
bool inventory_add(struct container *container, struct actor *actor);
void inventory_remove(struct container *container, struct actor *actor);
bool pick(struct engine *engine, struct actor *actor, struct actor *item);
bool drop(struct engine *engine, struct actor *actor, struct actor *item);
/*
 * Deals a huge damage to the nearest monster.
 */
bool lightning_wand_use(struct engine *engine, struct actor *actor, struct actor *item);
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
 * A common function to all usable items. All item-specific *_use
 * functions should call this as the last statemunt.
 */
bool potion_of_poison_use(struct engine *engine, struct actor *actor, struct actor *item);
bool use(struct actor *actor, struct actor *item);
bool eat(struct engine *engine, struct actor *actor, struct actor *food);
struct message get_hunger_status(struct actor *actor);
void warn_about_hunger(struct engine *engine, struct actor *actor);
bool make_hungry(struct actor *actor, float amount);
struct actor *make_food(int x, int y);
struct actor *make_healer_potion(int x, int y);
struct actor *make_curing_potion(int x, int y);
struct actor *make_lightning_wand(int x, int y);

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
