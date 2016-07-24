#include <libtcod.h>
#include "monsters.h"
#include <math.h>

/*** Monster functions ***/

/** Factory functions **/
struct actor *make_monster(int x, int y, const char ch, const char *name,
                           TCOD_color_t col, float power, float max_hp,
                           float hp, float defence,
                           const char *corpse_name,
                           void (*update)(struct engine *engine,
                                          struct actor *actor))
{
    struct actor *monster = init_actor(x, y, ch, name, col, render_actor);

    /* Artificial intelligence */
    monster->ai = init_ai(update, monster_move_or_attack);
    if (monster->ai != NULL) {
        monster->ai->xp = 1;
        monster->ai->xp_level = 1;
    }

    /* Init attacker */
    monster->attacker = init_attacker(power, attack);

    /* Init life */
    monster->life = init_life(max_hp, hp, defence, corpse_name,
                      take_damage, monster_die);

    return monster;
}

struct actor *make_orc(int x, int y)
{
    return make_monster(x, y, 'o', "an orc", TCOD_desaturated_green, 8,
                        15, 15, 2, "a dead orc", monster_update);
}

struct actor *make_goblin(int x, int y)
{
    return make_monster(x, y, 'g', "a goblin", TCOD_green, 5, 14, 14,
                        3, "a dead goblin", monster_update);
}

struct actor *make_troll(int x, int y)
{
    return make_monster(x, y, 'T', "a troll", TCOD_darker_green, 10,
                        20, 20, 3, "a troll carcass", monster_update);
}

struct actor *make_dragon(int x, int y)
{
    struct actor *tmp = make_monster(x, y, 'D', "a dragon", TCOD_darkest_green,
                                     10,
                                     25, 25, 7, "dragon scales and flesh",
                                     dragon_update);
    tmp->fov_only = false;
    return tmp;
}

bool monster_move_or_attack(struct engine *engine, struct actor *actor,
                            int targetx, int targety)
{
    int dx = targetx - actor->x;
    int dy = targety - actor->y;
    int stepdx = (dx > 0 ? 1 : -1);
    int stepdy = (dy > 0 ? 1 : -1);
    float distance = sqrtf(dx * dx + dy * dy);

    if (distance >= 2) {
        dx = (int) (round(dx / distance));
        dy = (int) (round(dy / distance));

        if (can_walk(engine, actor->x + dx, actor->y + dy)) {
            actor->x += dx;
            actor->y += dy;
        } else if (can_walk(engine, actor->x + stepdx, actor->y)) {
            actor->x += stepdx;
        } else if (can_walk(engine, actor->x, actor->y + stepdy)) {
            actor->y += stepdy;
        }
    } else if (actor->attacker) {
        struct actor *target = get_actor(engine, targetx, targety);
        if (target) {
            actor->attacker->attack(engine, actor, target);
            return false;
        }
    }

    return true;
}

void monster_update(struct engine *engine, struct actor *actor)
{
    if (actor->life && is_dead(actor)) {
        return;
    }

    if (is_in_fov(engine->map, actor->x, actor->y)) {
        /* We can see the player, start tracking him */
        actor->ai->move_count = TRACKING_TURNS;
    } else {
        (actor->ai->move_count)--;
    }

    if (actor->ai->move_count > 0) {
        actor->ai->move_or_attack(engine, actor, engine->player->x,
                                  engine->player->y);
    }
}

void dragon_update(struct engine *engine, struct actor *actor)
{
    if (actor->life && is_dead(actor)) {
        return;
    }

    if (!is_in_fov(engine->map, actor->x, actor->y))
        return;

    struct actor *target = get_closest_actor(engine, actor, 0);
    if (target)
        actor->ai->move_count = TRACKING_TURNS;
    else
        actor->ai->move_count--;
    if (actor->ai->move_count > 0 && target) {
        actor->ai->move_or_attack(engine, actor, target->x,
                                  target->y);
    }
}

/*
 * Transform a monster into an edible corpse.
 */
void monster_die(struct engine *engine, struct actor *actor)
{
    engine->gui->message(engine, TCOD_light_grey, "%s is dead.\n",
                         actor->name);

    /* Transform this dead body into an edible corpse */
    actor->pickable = init_pickable(0, 0, eat);

    reward_kill(engine, actor, engine->player);

    /* Call the common die function */
    die(engine, actor);
}
