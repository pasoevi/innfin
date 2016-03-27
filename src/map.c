#include "map.h"
#include <stdlib.h>
#include <stdio.h>

static const int MAX_ROOM_MONSTERS = 3;

void dig(struct map *map, int x1, int y1, int x2, int y2){
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
                        TCOD_map_set_properties(map->map, tilex, tiley, true, true);
                        /* map->tiles[tilex+tiley*map->w].can_walk = true; */
                }
        }
}

void create_room(struct engine *engine, bool first, int x1, int y1, int x2, int y2){
        dig(engine->map, x1, y1, x2, y2);
        if(first){
                /* put the player in the first room */
                engine->player->x = (x1 + x2) / 2;
                engine->player->y = (y1 + y2) / 2;
        }else{
                TCOD_random_t *rng = TCOD_random_get_instance();
                int num_monsters = TCOD_random_get_int(rng, 0, MAX_ROOM_MONSTERS);
                while(num_monsters > 0) {
                        int x = TCOD_random_get_int(rng, x1, x2);
                        int y = TCOD_random_get_int(rng, y1, y2);
                        if(can_walk(engine, x, y)){
                                add_monster(engine, x, y);
                        }
                        num_monsters--;
                }
        }

}

bool visit_node(TCOD_bsp_t *node, void *user_data) {
        struct engine *engine = (struct engine *)user_data;

        /* struct BSPTraverse trv = engine->map->bsp_traverse; */
        static int lastx;
        static int lasty;
        static int room_num;

        if(TCOD_bsp_is_leaf(node)) {
                int x, y, w, h;
                /* dig a room */
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
                }

                lastx = x+w/2;
                lasty = y+h/2;
                room_num++;
        }
        return true;
}

void init_map(struct engine *engine, int w, int h){
        engine->map = malloc(sizeof(struct map));
        engine->map->w = w;
        engine->map->h = h;

        int i;
	for(i = 0; i < w * h ; i++){
                engine->map->tiles[i].explored = false;
        }
        
        engine->map->map = TCOD_map_new(w, h);
                
        engine->map->render = map_render;
        engine->map->bsp = TCOD_bsp_new_with_size(0, 0, w, h);
	engine->map->bsp_traverse.lastx = 0;
	engine->map->bsp_traverse.lasty = 0;
	engine->map->bsp_traverse.room_num = 0;
        TCOD_bsp_split_recursive(engine->map->bsp, NULL, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
        TCOD_bsp_traverse_inverted_level_order(engine->map->bsp, visit_node, engine);
}

void free_map(struct map *map){
        TCOD_bsp_delete(map->bsp);
        free(map);
}

bool is_wall(struct map *map, int x, int y){
        return !TCOD_map_is_walkable(map->map, x, y);
}

bool can_walk(struct engine *engine, int x, int y){
        if(is_wall(engine->map, x, y)){
                return false;
        }

        struct actor **actor;
        for(actor = (struct actor **)TCOD_list_begin(engine->actors);
            actor != (struct actor **)TCOD_list_end(engine->actors);
            actor++){
                if((*actor)->blocks && (*actor)->x == x && (*actor)->y == y){
                        /* There is a blocking actor there, cat't
			   walk */
                        return false;
                }
        }
        return true;
}

bool is_in_fov(struct map *map, int x, int y){
        if(TCOD_map_is_in_fov(map->map, x, y)){
                map->tiles[x+y*(map->w)].explored = true;
                return true;
        }
        return false;
}

bool is_explored(struct map *map, int x, int y){
        return map->tiles[x+y*(map->w)].explored;
}

void compute_fov(struct engine *engine){
        TCOD_map_compute_fov(engine->map->map,
                             engine->player->x,
                             engine->player->y,
                             engine->fov_radius,
                             true,
                             FOV_BASIC);
}

void add_monster(struct engine* engine, int x, int y){
        TCOD_random_t *rng = TCOD_random_get_instance();
        struct actor *actor;
        if (TCOD_random_get_int(rng, 0, 100) < 80) {
                /* Create an orc */
                make_orc(&actor, x, y);
        }else{
                /* Create an orc */
                make_troll(&actor, x, y);
        }
        TCOD_list_push(engine->actors, actor);
}

void map_render(struct map *map){
        const TCOD_color_t dark_wall = {0, 0, 100};
        const TCOD_color_t dark_ground = {50, 50, 150};

        const TCOD_color_t light_wall = {130, 110, 50};
        const TCOD_color_t light_ground = {200, 180, 50};

        int x, y;
        for(x = 0; x < map->w; x++) {
                for(y = 0; y < map->h; y++) {
                        if(is_in_fov(map, x, y)){
                                TCOD_console_set_char_background(NULL, x, y,
                                                                 is_wall(map, x, y) ? light_wall : light_ground,
                                                                 TCOD_BKGND_SET);
                        }else{
                                TCOD_console_set_char_background(NULL, x, y,
                                                                 is_wall(map, x, y) ? dark_wall : dark_ground,
                                                                 TCOD_BKGND_SET);
                        }
                }
        }
}
