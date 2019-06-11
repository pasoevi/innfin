/*
  Copyright (C) 2016 Sergo Pasoevi.

  This pragram is free software: you can redistribute it and/or modify
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

#ifndef UTIL_H
#define UTIL_H

#include "libtcod/libtcod.h"
#include "actor.h"

const char *generate_name(const char *filename);

double distance(int x1, int y1, int x2, int y2);

/* Iterate over lst and call `action` for each actor. */
void each_actor(struct engine *engine, TCOD_list_t lst,
                void (*action)(struct engine *engine,
                               struct actor *actor));

#endif
