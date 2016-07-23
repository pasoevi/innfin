//
// Created by Sergi on 18.07.2016.
//

#include "player.h"

/* Writes a memorial file */
static void make_memorial(struct actor *actor)
{

}

/* Transform the actor into a decaying corpse */
void player_die(struct engine *engine, struct actor *actor)
{
    engine->gui->message(engine, TCOD_red, "You die.\n");
    /* Call the common death function */
    die(engine, actor);
    make_memorial(actor);
    engine->game_status = DEFEAT;

}

struct actor *make_player(int x, int y)
{
    struct actor *tmp =
            init_actor(x, y, '@', "you", TCOD_white, render_actor);

    /* Artificial intelligence */
    tmp->ai = init_ai(player_update, player_move_or_attack);
    tmp->ai->skills->strength = 15;
    tmp->ai->skills->intelligence = 9;

    /* Init attacker */
    tmp->attacker = init_attacker(10, attack);

    /* Init life */
    tmp->life =
            init_life(100, 100, 6, "your dead body", take_damage,
                      player_die);
    tmp->life->max_stomach = 500;
    tmp->life->stomach = tmp->life->max_stomach;

    /* Init inventory */
    tmp->inventory = init_container(26);

    return tmp;
}

bool player_move_or_attack(struct engine *engine, struct actor *actor,
                           int targetx, int targety)
{
    /* Consume energy from stomach and kill the player if beyond
     * starvation.
     */
    if (!make_hungry(actor, 1)) {
        engine->gui->message(engine, TCOD_light_grey,
                             "You starve to death.\n");
        actor->life->die(engine, actor);
        return false;
    }

    if (is_wall(engine->map, targetx, targety))
        return false;

    /* Look for actors to attack */
    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors);
         iter++) {
        if ((*iter)->life && !is_dead(*iter) &&
            (*iter)->x == targetx && (*iter)->y == targety) {
            /* There is an actor there, cat't walk */
            actor->attacker->attack(engine, actor, *iter);
            return false;
        }
    }

    /* Look for corpses or pickable items */
    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors);
         iter++) {
        struct actor *actor = *iter;
        bool corpse_or_item = (actor->life
                               && is_dead(actor))
                              || actor->pickable;
        if (corpse_or_item && actor->x == targetx
            && actor->y == targety)
            engine->gui->message(engine, TCOD_light_gray,
                                 "There's %s here\n",
                                 (*iter)->name);
    }

    actor->x = targetx;
    actor->y = targety;

    return true;
}

void player_update(struct engine *engine, struct actor *actor)
{
    if (actor->life && is_dead(actor)) {
        return;
    }

    if (should_level_up(engine, actor))
        actor->ai->level_up(engine, actor);

    int dx = 0, dy = 0;
    if (engine->key.pressed) {
        switch (engine->key.vk) {
            case TCODK_UP:
                dy = -1;
                break;
            case TCODK_DOWN:
                dy = 1;
                break;
            case TCODK_LEFT:
                dx = -1;
                break;
            case TCODK_RIGHT:
                dx = 1;
                break;
            case TCODK_CHAR:
                handle_action_key(engine, actor);
                break;
            case TCODK_ENTER:
                if (engine->key.lalt)
                    TCOD_console_set_fullscreen(!TCOD_console_is_fullscreen());
                break;
            default:
                break;
        }
    }

    if (dx != 0 || dy != 0) {
        engine->game_status = NEW_TURN;
        if (actor->ai->move_or_attack(engine, actor, actor->x + dx,
                                      actor->y + dy)) {
            compute_fov(engine);
        }
    }
}

