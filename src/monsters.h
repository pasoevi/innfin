#include <stdbool.h>
#include "actor.h"
#include "engine.h"

#ifndef INNFIN_MONSTERS_H
#define INNFIN_MONSTERS_H

struct actor *make_monster(int x, int y, const char ch, const char *name,
                           TCOD_color_t col, float power, float max_hp,
                           float hp, float defence,
                           const char *corpse_name,
                           void (*update)(struct engine *engine,
                                          struct actor *actor));

struct actor *make_orc(int x, int y);

struct actor *make_goblin(int x, int y);

struct actor *make_troll(int x, int y);

struct actor *make_dragon(int x, int y);

void monster_update(struct engine *engine, struct actor *actor);

/*
 * Attacks player and other monsters as well.
 */
void dragon_update(struct engine *engine, struct actor *actor);

/*
 * Temporary AI for actors that are confused. They move to random
 * directions attacking everything in their way.
 */
void confused_update(struct engine *engine, struct actor *actor);

/*
 * Monsters that wander do not immediately attack anybody, but they
 * start behaving as typical attacker intelligences once attacked.
 */
void wandering_update(struct engine *engine, struct actor *actor);

bool monster_move_or_attack(struct engine *engine, struct actor *actor,
                            int x, int y);

void monster_die(struct engine *engine, struct actor *actor);

#endif //INNFIN_MONSTERS_H