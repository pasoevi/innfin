#ifndef ACTOR_H
#define ACTOR_H

#include "libtcod.h"
#include "engine.h"
#include "map.h"

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

struct actor{
        int x;
        int y;
        char ch;
        bool blocks; /* can we walk on this actor? */
        struct attacker *attacker;
	struct destructible *destructible;
	struct ai *ai;
        const char *name;
        TCOD_color_t col;
        void (*update)(struct engine *engine, struct actor *actor);
        void (*render)(struct actor *actor);
};

/* void init_actor(struct actor **actor, int w, int h, int ch, const char *name, TCOD_color_t col, void (*render)(struct actor *)); */
void make_orc(struct actor **actor, int x, int y);
void make_troll(struct actor **actor, int x, int y);
void make_kobold(struct actor **actor, int x, int y);
void make_player(struct actor **player, int x, int y);
void render_actor(struct actor *actor);
void player_update(struct engine *engine, struct actor *actor);
void monster_update(struct engine *engine, struct actor *actor);
bool player_move_or_attack(struct engine *engine, struct actor *actor, int x, int y);
bool monster_move_or_attack(struct engine *engine, struct actor *actor, int x, int y);
void attack(struct engine *engine, struct actor *dealer, struct actor *target);
bool is_dead(struct actor *actor);
float take_damage(struct engine *engine, struct actor *target, float damage);
void die(struct engine *engine, struct actor *actor);
void player_die(struct engine *engine, struct actor *actor);
void monster_die(struct engine *engine, struct actor *actor);
#endif
