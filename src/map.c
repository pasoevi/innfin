#include "map.h"
#include <stdlib.h>
#include <stdio.h>

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

int ar_length(struct Tile ar[]){
        return (int)( sizeof(ar) / sizeof(ar[0]));
}

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
                        map->tiles[tilex+tiley*map->w].can_walk = true;
                }
        }
}

void create_room(struct Engine *engine, bool first, int x1, int y1, int x2, int y2){
        dig(engine->map, x1, y1, x2, y2);
        if(first){
                /* put the player in the first room */
                engine->player->x = (x1 + x2) / 2;
                engine->player->y = (y1 + y2) / 2;
        }else{
                TCOD_random_t *rng = TCOD_random_get_instance();
                if (TCOD_random_get_int(rng, 0, 3) == 0) {
                        struct Actor *actor;
                        init_actor(&actor, (x1+x2)/2,(y1+y2)/2,'@', TCOD_yellow, render_actor);
                        TCOD_list_push(engine->actors, actor);
                }
        }
}

bool visit_node(TCOD_bsp_t *node, void *user_data) {
        struct Engine *engine = (struct Engine *)user_data;
        static int lastx = 0;
        static int lasty = 0;
        static int room_num = 0;

        if(TCOD_bsp_is_leaf(node)) {
                int x, y, w, h;
                
                // dig a room
                TCOD_random_t *rng = TCOD_random_get_instance();
                w = TCOD_random_get_int(rng, ROOM_MIN_SIZE, node->w - 2);
                h = TCOD_random_get_int(rng, ROOM_MIN_SIZE, node->h - 2);
                x = TCOD_random_get_int(rng, node->x + 1, node->x + node->w - w - 1);
                y = TCOD_random_get_int(rng, node->y+1, node->y+node->h-h-1);
                create_room(engine, room_num == 0, x, y, x + w - 1, y + h - 1);

                if(room_num != 0){
                        /* dig a corridor from last room */
                        dig(engine->map, lastx,lasty,x+w/2,lasty);
                        dig(engine->map, x + w / 2,lasty, x + w / 2, y + h / 2);

                        lastx = x+w/2;
                        lasty = y+h/2;
                        room_num++;
                }
                return true;
        }
        
        return false;
}

void init_map(struct Engine *engine, int w, int h){
        engine->map = malloc(sizeof(struct Map));
        engine->map->w = w;
        engine->map->h = h;
        struct Tile tiles[w*h];//  = calloc(w * h, sizeof(engine->map->tiles[0]));
        printf("w = %d, h = %d, number of tiles = %d\n", w, h, ar_length(engine->map->tiles));
        engine->map->tiles = tiles;
        printf("w = %d, h = %d, number of tiles = %d\n", w, h, ar_length(engine->map->tiles));
        engine->map->render = map_render;
        
        engine->map->bsp = TCOD_bsp_new_with_size(0, 0, w, h);
        TCOD_bsp_split_recursive(engine->map->bsp, NULL, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
        TCOD_bsp_traverse_inverted_level_order(engine->map->bsp, visit_node, engine);
        
}

bool is_wall(struct Map *map, int x, int y){
        return !(map->tiles[(x+y)*(map->w)].can_walk);
}

void map_render(struct Map *map){
        const TCOD_color_t dark_wall = {0, 0, 100};
        const TCOD_color_t dark_ground = {50, 50, 150};

        int x, y;
        for(x = 0; x < map->w; x++) {
                for(y = 0; y < map->h; y++) {
                        TCOD_console_set_char_background(NULL, x, y,
                                                         is_wall(map, x, y) ? dark_wall : dark_ground,
                                                         TCOD_BKGND_SET);
                }
        }
}
