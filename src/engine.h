#ifndef ENGINE_H
#define ENGINE_H

#include "libtcod.h"
#include "actor.h"
#include "map.h"

enum GameStatus {
       STARTUP,
       IDLE,
       NEW_TURN,
       VICTORY,
       DEFEAT
};

struct Engine{
        TCOD_list_t *actors;
        struct Actor *player;
        struct Map *map;
        int fov_radius;
        bool compute_fov;
        TCOD_key_t key;
        enum GameStatus game_status;
        void (*update)(struct Engine *);
        void (*render)(struct Engine *);
};

void engine_init(struct Engine **engine, int w, int h, const char *title);
void send_to_back(struct Engine *engine, struct Actor *actor);
void engine_update(struct Engine *engine);
void engine_render(struct Engine *engine);

#endif
