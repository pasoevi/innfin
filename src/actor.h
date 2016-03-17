#ifndef ACTOR_H
#define ACTOR_H

#include "libtcod.h"
#include "engine.h"
#include "map.h"

static const int TRACKING_TURNS = 3;

struct Engine;
struct Actor;

struct AI{
	int move_count; /* allow monsters to track the player */
};

struct Attacker{
        float power;
        void (*attack)(struct Engine *engine, struct Actor *dealer, struct Actor *target);
};

struct Destructible{
        float max_hp; /* maximum health points */
        float hp; /* current health points */
        float defence; /* hit points deflected */
        const char *corpse_name; /* the actor's name once dead/destroyed */
        float (*take_damage)(struct Engine *engine, struct Actor *target, float damage);
        void (*die)(struct Engine *engine, struct Actor *actor);
};

struct Actor{
        int x;
        int y;
        char ch;
        bool blocks; /* can we walk on this actor? */
        struct Attacker *attacker;
	struct Destructible *destructible;
	struct AI *ai;
        const char *name;
        TCOD_color_t col;
        bool (*move_or_attack)(struct Engine *engine, struct Actor *actor, int targetx, int targety);
        void (*update)(struct Engine *engine, struct Actor *actor);
        void (*render)(struct Actor *actor);
};

void init_actor(struct Actor **actor, int w, int h, int ch, const char *name, TCOD_color_t col, void (*render)(struct Actor *));
void make_orc(struct Actor **actor, int x, int y);
void make_troll(struct Actor **actor, int x, int y);
void make_kobold(struct Actor **actor, int x, int y);
void make_player(struct Actor **player, int x, int y);
void render_actor(struct Actor *actor);
void player_update(struct Engine *engine, struct Actor *actor);
void monster_update(struct Engine *engine, struct Actor *actor);
bool player_move_or_attack(struct Engine *engine, struct Actor *actor, int x, int y);
bool monster_move_or_attack(struct Engine *engine, struct Actor *actor, int x, int y);
void attack(struct Engine *engine, struct Actor *dealer, struct Actor *target);
bool is_dead(struct Actor *actor);
float take_damage(struct Engine *engine, struct Actor *target, float damage);
void die(struct Engine *engine, struct Actor *actor);
void player_die(struct Engine *engine, struct Actor *actor);
void monster_die(struct Engine *engine, struct Actor *actor);
#endif
