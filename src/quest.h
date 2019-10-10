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

#ifndef ACTOR_H
#define ACTOR_H
#include "actor.h"

enum operation_type {
    OPERATION_KILL,
    OPERATION_FIND,
    OPERATION_TALK,
    OPERATION_CAPTURE,
    OPERATION_GIVE,
};

struct quest {
    char *name;
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    struct actor *target_actor;
    enum operation_type operation_type;

};

#endif
