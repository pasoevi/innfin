#ifndef ACTOR_H
#define ACTOR_H

#include "libtcod.h"
#include "engine.h"
#include "map.h"

struct Engine;

struct Actor{
        int x;
        int y;
        char ch;
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
#endif
