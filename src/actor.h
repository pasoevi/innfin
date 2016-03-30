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

struct attacker{
        float power;
        void (*attack)(struct engine *engine, struct actor *dealer, struct actor *target);
};

struct destructible{
        float max_hp; /* maximum health points */
        float hp; /* current health points */
        float defence; /* hit points deflected */
        const char *corpse_name; /* the actor's name once dead/destroyed */
        float (*take_damage)(struct engine *engine, struct actor *target, float damage);
        void (*die)(struct engine *engine, struct actor *actor);
};

struct pickable{
        
};

/* Capacity is currently counted by the number of items */
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

struct container *init_container(int capacity);
struct pickable *init_pickable(void);
bool inventory_add(struct container *container, struct actor *actor);
void inventory_remove(struct container *container, struct actor *actor);

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
