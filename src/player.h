//
// Created by Sergi on 18.07.2016.
//

#ifndef INNFIN_PLAYER_H
#define INNFIN_PLAYER_H

#include <stdbool.h>
#include "actor.h"

struct actor *make_player(int x, int y);


void player_update(struct engine *engine, struct actor *actor);

bool player_move_or_attack(struct engine *engine, struct actor *actor,
                           int x, int y);

/*
 * Called when the player hit points equal to zero. It first calls the
 * common die function.
 *
 * Also calls the function that creates character memorial file and
 * deletes saved game if present.
 */
void player_die(struct engine *engine, struct actor *actor);

#endif //INNFIN_PLAYER_H
