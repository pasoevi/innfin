#include "map.h"
#include <stdlib.h>

void init_map(struct Map **map, int w, int h){
        *map = malloc(sizeof(*map));
        (*map)->w = w;
        (*map)->h = h;
        (*map)->tiles = calloc(w*h, sizeof(struct Tile));
}

bool is_wall(struct Map *map, int x, int y){
        return !map->tiles[(x+y)*(map->w)].can_walk;
}
void set_wall(struct Map *map, int x, int y){
        map->tiles[x+y*map->w].can_walk = false;
}

void map_render(struct Map *map){
        const TCOD_color_t dark_wall = {0, 0, 100};
        const TCOD_color_t dark_ground = {50, 50, 100};

        int x, y;
        for(x=0; x < map->w; x++) {
                for(y = 0; y < map->h; y++) {
                        TCOD_console_set_char_background(NULL, x, y, is_wall(map, x, y) ? dark_wall : dark_ground, TCOD_BKGND_SET);
                }
        }
}

