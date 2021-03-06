/*
    Copyright (C) 2016 Sergo Pasoevi.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at
    your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

    Written by Sergo Pasoevi <spasoevi@gmail.com>

*/

#include "libtcod/libtcod.h"
#include "monsters.h"
#include "tiles.h"
#include <math.h>

/** Factory functions **/
struct actor *mkmonster(int x, int y, const int ch, char *name,
                           TCOD_color_t col, double power, double max_hp,
                           double hp, double defence, char *corpse_name,
                           void (*update)(struct engine *engine,
                                          struct actor *actor))
{
    struct actor *monster = mkactor(x, y, ch, name, col);
    monster->blocking = true;

    /* Artificial intelligence */
    monster->ai = mkai(update, monster_move_or_attack);
    if (monster->ai != NULL) {
        monster->ai->xp = max_hp / 2;
        monster->ai->xp_level = 1;
        monster->ai->skills[SKILL_STRENGTH].val = 10;
        monster->ai->skills[SKILL_FIGHTING].val = 4;
    }

    /* Init attacker */
    monster->attacker = mkattacker(power, attack);

    /* Init life */
    monster->life = mklife(max_hp, hp, defence, corpse_name, take_damage,
                              monster_die);

    return monster;
}

struct actor *mkorc(int x, int y)
{
    struct actor *orc = mkmonster(x, y, ORC_TILE, "an orc",
                                     TCOD_green, 11,
                                     15, 15, 4, "a dead orc", monster_update);
    orc->ai->xp_level = 2;
    orc->ai->skills[SKILL_STRENGTH].val = 8;
    orc->ai->skills[SKILL_FIGHTING].val = 13;
    return orc;
}

struct actor *mkgoblin(int x, int y)
{
    struct actor *goblin = mkmonster(x, y, ORC_TILE, "a goblin", TCOD_gray, 10,
                                        14, 14, 2, "a dead goblin",
                                        monster_update);
    goblin->ai->xp_level = 1;
    goblin->ai->skills[SKILL_STRENGTH].val = 5;
    goblin->ai->skills[SKILL_FIGHTING].val = 20;
    return goblin;
}

struct actor *mktroll(int x, int y)
{
    struct actor *troll = mkmonster(x, y, TROLL_TILE, "a troll", TCOD_lighter_green,
                                       12, 20, 20, 3, "a troll carcass",
                                       monster_update);
    troll->ai->xp_level = 2;
    troll->ai->skills[SKILL_STRENGTH].val = 20;
    troll->ai->skills[SKILL_FIGHTING].val = 7;
    return troll;
}

struct actor *mkdragon(int x, int y)
{
    struct actor *dragon = mkmonster(x, y, 'D', "a dragon", TCOD_dark_green,
                                        16, 25, 25, 10,
                                        "dragon scales and flesh",
                                        dragon_update);
    dragon->fov_only = false;
    dragon->ai->xp_level = 4;
    dragon->ai->skills[SKILL_STRENGTH].val = 25;
    dragon->ai->skills[SKILL_FIGHTING].val = 5;

    return dragon;
}

bool monster_move_or_attack(struct engine *engine, struct actor *actor,
                            int target_x, int target_y)
{
    int dx = target_x - actor->x;
    int dy = target_y - actor->y;
    int step_dx = (dx > 0 ? 1 : -1);
    int step_dy = (dy > 0 ? 1 : -1);
    double distance = sqrt(dx * dx + dy * dy);

    if (distance >= 2) {
        dx = (int) (round(dx / distance));
        dy = (int) (round(dy / distance));

        if (can_walk(engine, actor->x + dx, actor->y + dy)) {
            actor->x += dx;
            actor->y += dy;
        } else if (can_walk(engine, actor->x + step_dx, actor->y)) {
            actor->x += step_dx;
        } else if (can_walk(engine, actor->x, actor->y + step_dy)) {
            actor->y += step_dy;
        }
    } else if (actor->attacker) {
        struct actor *target = get_actor(engine, target_x, target_y);
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

    struct actor *target;

    if (actor->attacker->current_target) {
        /* We have been attacked by somebody. Attack him in turn! */
        target = actor->attacker->current_target;
        actor->ai->move_count = TRACKING_TURNS;
    } else if (is_in_fov(engine->map, actor->x, actor->y)) {
        /* We can see the player, start tracking him */
        target = engine->player;
        actor->ai->move_count = TRACKING_TURNS;
    } else {
        target = NULL;
        actor->ai->move_count--;
    }

    if (actor->ai->move_count > 0 && target)
        actor->ai->move_or_attack(engine, actor, target->x, target->y);
}

void dragon_update(struct engine *engine, struct actor *actor)
{
    if (actor->life && is_dead(actor))
        return;


    if (!is_in_fov(engine->map, actor->x, actor->y))
        return;

    struct actor *target = get_closest_actor(engine, actor, 0);
    if (target)
        actor->ai->move_count = TRACKING_TURNS;
    else
        actor->ai->move_count--;

    if (actor->ai->move_count > 0 && target)
        actor->ai->move_or_attack(engine, actor, target->x, target->y);
}

/*
 * Transform a monster into an edible corpse.
 */
void
monster_die(struct engine *engine, struct actor *actor, struct actor *killer)
{
    /* Transform this dead body into an edible corpse */
    actor->pickable = init_pickable(0, 0, eat);

    reward_kill(engine, killer, actor);

    /* Call the common die function */
    die(engine, actor, killer);
}
