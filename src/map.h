#include <stdbool.h>
#include "libtcod.h"

struct Tile{
        bool can_walk;
};

struct Map{
        int w;
        int h;
        void (*render)(struct Map);
        struct Tile *tiles;
};

void init_map(struct Map **map, int w, int h);
void map_render(struct Map *map);
bool is_wall(struct Map *map, int x, int y);
void set_wall(struct Map *map, int x, int y);

        
                
