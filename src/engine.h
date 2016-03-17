#ifndef ENGINE_H
#define ENGINE_H

#include "libtcod.h"
#include "actor.h"
#include "map.h"

enum{
        WINDOW_W = 80,
        WINDOW_H = 50
};

enum GameStatus {
       STARTUP,
       IDLE,
       NEW_TURN,
       VICTORY,
       DEFEAT
};

struct engine{
        TCOD_list_t *actors;
        struct actor *player;
        struct map *map;
        int fov_radius;
        bool compute_fov;
        TCOD_key_t key;
        enum GameStatus game_status;
        void (*update)(struct engine *);
        void (*render)(struct engine *);
};

void engine_init(struct engine **engine, int w, int h, const char *title);
void send_to_back(struct engine *engine, struct actor *actor);
void engine_update(struct engine *engine);
void engine_render(struct engine *engine);

#endif
