#include <stdbool.h>

struct Tile{
        bool can_walk;
};

struct Map{
        int w;
        int h;
        bool (*is_wall)(int x, int y);
        void (*render)(struct Map);
        List *tiles;
        void (*set_wall)(int x, int y);
}
                
