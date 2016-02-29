#include <stdlib.h>
#include "engine.h"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

void dig(struct Map *map, int x1, int y1, int x2, int y2){
        if(x2 < x1){
                int tmp = x2;
                x2 = x1;
                x1 = tmp;
        }

        if(y2 < y1){
                int tmp = y2;
                y2 = y1;
                y1 = tmp;
        }

        int tilex, tiley;
        for(tilex = x1; tilex <= x2; tilex++){
                for(tiley = y1; tiley <= y2; tiley++){
                        map->tiles[tilex+tiley*map->w].can_walk=true;
                }
        }
}

void create_room(struct Engine *engine, bool first, int x1, int y1, int x2, int y2){
        dig (engine->map, x1, y1, x2, y2);
        if ( first ) {
                // put the player in the first room
                engine->player->x=(x1+x2)/2;
                engine->player->y=(y1+y2)/2;
        } else {
                TCOD_random_t *rng = TCOD_random_get_instance();
                if (TCOD_random_get_int(rng, 0, 3) == 0) {
                        struct Actor *actor;// push(new Actor((x1+x2)/2,(y1+y2)/2,'@', TCODColor::yellow));
                        init_actor(&actor, (x1+x2)/2,(y1+y2)/2,'@', TCOD_yellow, render_actor);
                        TCOD_list_push(engine->actors, actor);
                }
        }
}

void init_map(struct Map **map, int w, int h){
        *map = malloc(sizeof(*map));
        (*map)->w = w;
        (*map)->h = h;
        (*map)->tiles = calloc(w*h, sizeof(struct Tile));
        set_wall(*map, 30,22);
        set_wall(*map, 50,22);
        (*map)->render = map_render;
}



bool is_wall(struct Map *map, int x, int y){
        return !map->tiles[(x+y)*(map->w)].can_walk;
}

void set_wall(struct Map *map, int x, int y){
        map->tiles[(x+y)*(map->w)].can_walk = false;
}

void map_render(struct Map *map){
        const TCOD_color_t dark_wall = {0, 0, 100};
        const TCOD_color_t dark_ground = {70, 70, 80};

        int x, y;
        for(x=0; x < map->w; x++) {
                for(y = 0; y < map->h; y++) {
                        TCOD_console_set_char_background(NULL, x, y,
                                                         is_wall(map, x, y) ? dark_wall : dark_ground,
                                                         TCOD_BKGND_SET);
                }
        }
}

