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

#include "actor.h"
#include "engine.h"

#ifndef INNFIN_MONSTERS_H
#define INNFIN_MONSTERS_H

struct actor *mkmonster(int x, int y, const int ch, char *name,
                           TCOD_color_t col, double power, double max_hp,
                           double hp, double defence, char *corpse_name,
                           void (*update)(struct engine *engine,
                                          struct actor *actor));

struct actor *mkorc(int x, int y);

struct actor *mkgoblin(int x, int y);

struct actor *mktroll(int x, int y);

struct actor *mkdragon(int x, int y);

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
                            int target_x, int target_y);

void
monster_die(struct engine *engine, struct actor *actor, struct actor *killer);

#endif //INNFIN_MONSTERS_H
