#ifndef ACTOR_H
#define ACTOR_H

#include "libtcod.h"
#include "engine.h"
#include "map.h"

struct Engine;
struct Actor;

struct Attacker{
        float power;
        void (*attack)(struct Actor *dealer, struct Actor *target);
};

struct Destructible{
        float max_hp; /* maximum health points */
        float hp; /* current health points */
        float defence; /* hit points deflected */
        const char *corpse_name; /* the actor's name once dead/destroyed */
        void (*attack)(struct Actor *dealer, struct Actor *target);
};

struct Actor{
        int x;
        int y;
        char ch;
        bool blocks; /* can we walk on this actor? */
        struct Attacker *attacker;
        const char *name;
        TCOD_color_t col;
        void (*update)(struct Engine *engine, struct Actor *actor);
        void (*render)(struct Actor *actor);
};

void init_actor(struct Actor **actor, int w, int h, int ch, const char *name, TCOD_color_t col, void (*render)(struct Actor *));
void render_actor(struct Actor *actor);
void player_update(struct Engine *engine, struct Actor *actor);
void actor_update(struct Engine *engine, struct Actor *actor);
bool move_or_attack(struct Engine *engine, struct Actor *actor, int x, int y);
void attack(struct Actor *dealer, struct Actor *target);
#endif
