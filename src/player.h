/*
  Copyright (C) 2016 Sergi Pasoev.

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

  Written by Sergi Pasoev <s.pasoev@gmail.com>

*/

#ifndef INNFIN_PLAYER_H
#define INNFIN_PLAYER_H

#include <stdbool.h>
#include "actor.h"

struct actor *make_player(int x, int y);

bool descend(struct engine *engine, struct actor *actor, struct actor *stairs);

void player_update(struct engine *engine, struct actor *actor);

float regen_hp(struct engine *engine, struct actor *actor);

bool player_move_or_attack(struct engine *engine, struct actor *actor,
                           int target_x, int target_y);

/*
 * Called when the player hit points equal to zero. It first calls the
 * common die function.
 *
 * Also calls the function that creates character memorial file and
 * deletes saved game if present.
 */
void player_die(struct engine *engine, struct actor *actor, struct actor *killer);

#endif //INNFIN_PLAYER_H
