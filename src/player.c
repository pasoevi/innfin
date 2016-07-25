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

#include "player.h"

/* Writes a memorial file */
static void make_memorial(struct actor *actor)
{

}

/* Transform the actor into a decaying corpse */
void player_die(struct engine *engine, struct actor *actor,
                struct actor *killer)
{
    engine->gui->message(engine, TCOD_red, "You die.\n");
    /* Call the common death function */
    die(engine, actor, NULL);
    make_memorial(actor);
    engine->game_status = DEFEAT;

}

struct actor *make_player(int x, int y)
{
    struct actor *tmp =
            init_actor(x, y, '@', "you", TCOD_white);

    /* Artificial intelligence */
    tmp->ai = init_ai(player_update, player_move_or_attack);
    tmp->ai->skills.strength = 15;
    tmp->ai->skills.intelligence = 9;

    /* Init attacker */
    tmp->attacker = init_attacker(10, attack);

    /* Init life */
    tmp->life = init_life(1000, 1000, 6, "your dead body", take_damage,
                          player_die);
    tmp->life->max_stomach = 500;
    tmp->life->stomach = tmp->life->max_stomach;

    /* Init inventory */
    tmp->inventory = init_container(26);

    return tmp;
}

bool descend(struct engine *engine, struct actor *actor, struct actor *stairs)
{
    if (actor == NULL || is_dead(actor) || stairs == NULL)
        return false;

    if (actor->x == stairs->x && actor->y == stairs->y) {
        load_level(engine, engine->level + 1);
        return true;
    }
    else {
        engine->gui->message(engine, TCOD_gray,
                             "You can't climb down here. Try on stairs.");
        return false;
    }
}

bool player_move_or_attack(struct engine *engine, struct actor *player,
                           int target_x, int target_y)
{
    /* Consume energy from stomach and kill the player if beyond
     * starvation.
     */
    if (!make_hungry(player, 1)) {
        engine->gui->message(engine, TCOD_light_grey,
                             "You starve to death.\n");
        player->life->die(engine, player, NULL);
        return false;
    }

    if (is_wall(engine->map, target_x, target_y))
        return false;

    /* Look for actors to attack */
    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors);
         iter++) {
        if ((*iter)->life && !is_dead(*iter) &&
            (*iter)->x == target_x && (*iter)->y == target_y) {
            /* There is an actor there, cat't walk */
            player->attacker->attack(engine, player, *iter);
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
        if (corpse_or_item && actor->x == target_x
            && actor->y == target_y)
            engine->gui->message(engine, TCOD_light_gray,
                                 "There's %s here\n",
                                 (*iter)->name);
    }

    player->x = target_x;
    player->y = target_y;

    return true;
}

struct actor *choose_from_inventory(struct engine *engine,
                                    struct actor *actor,
                                    const char *window_title,
                                    bool(*predicate)(struct actor *actor))
{
    /* Display the inventory frame */
    TCOD_console_t *con = engine->gui->inventory_con;
    TCOD_color_t color = (TCOD_color_t) {200, 180, 50};
    TCOD_console_set_default_foreground(con, color);
    TCOD_console_print_frame(con, 0, 0, INVENTORY_WIDTH,
                             INVENTORY_HEIGHT, true,
                             TCOD_BKGND_DEFAULT, window_title);

    /*
     * Count the items that specify the predicate and display the
     * items with their respective shortcuts.
     */
    TCOD_console_set_default_foreground(con, TCOD_white);
    int num_items = 0;
    int shortcut = 'a';
    int y = 1;
    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(actor->inventory->inventory);
         iter != (struct actor **) TCOD_list_end(actor->inventory->inventory);
         iter++) {
        struct actor *item = *iter;
        if (predicate(item)) {
            TCOD_console_print(con, 2, y, "(%c) %s", shortcut, item->name);
            y++;
            num_items++;
        }
        shortcut++;

    }

    /* Blit the inventory console to the root console. */
    TCOD_console_blit(con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT,
                      NULL, engine->window_w / 2 - INVENTORY_WIDTH / 2,
                      engine->window_h / 2 - INVENTORY_HEIGHT / 2, 1.f,
                      1.f);
    TCOD_console_flush();

    /* wait for a key press */
    TCOD_key_t key;
    TCOD_sys_wait_for_event(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
    if (key.vk == TCODK_CHAR) {
        int actor_index = key.c - 'a';
        if (actor_index >= 0
            && actor_index <
               TCOD_list_size(actor->inventory->inventory)) {
            struct actor *tmp =
                    TCOD_list_get(actor->inventory->inventory, actor_index);
            if (predicate(tmp))
                return tmp;
            else
                engine->gui->message(engine, TCOD_light_grey,
                                     "You can't %s that.\n", window_title);
        }
    }

    return NULL;
}

/*
 * Invoke the command on the item which is chosen by the item_chooser
 * function. The command to be invoked is specified as the *command*
 * argument or if passed NULL, it is implied to be the
 * item->pickable->use function.
 */
void invoke_command(struct engine *engine,
                    bool command(struct engine *engine, struct actor *actor,
                                 struct actor *item),
                    bool (*item_chooser)(struct actor *actor),
                    const char *window_title)
{
    struct actor *item =
            choose_from_inventory(engine, engine->player, window_title,
                                  item_chooser);
    if (item) {
        if (command)
            command(engine, engine->player, item);
        else
            item->pickable->use(engine, engine->player, item);

        engine->game_status = NEW_TURN;
    }
}

void handle_action_key(struct engine *engine, struct actor *actor)
{
    /* */
    switch (engine->key.c) {
        case ',':
        case 'g':
            try_pick(engine);
            break;
        case '>':
            descend(engine, engine->player, engine->stairs);
            break;
        case '<':
            engine->gui->message(engine, TCOD_gray,
                                 "You can't climb up here. Try on stairs.");
            break;
        case 'a':
            engine->gui->message(engine, TCOD_gray,
                                 "You have no special abilities.");
            break;
        case 'd': /* Drop item */
            invoke_command(engine, drop, is_usable, "drop");
            break;
        case 'D': /* Drop the last item */
            drop_last(engine, actor);
            break;
        case 'e': /* Eat */
            invoke_command(engine, NULL, is_edible, "eat");
            break;
        case 'i': /* display inventory */
            invoke_command(engine, NULL, is_usable, "inventory");
            break;
        case 'q': /* Quaff */
            invoke_command(engine, NULL, is_drinkable, "quaff");
            break;

        case 'w': /* Wield */
            invoke_command(engine, NULL, is_wieldable, "wield");
            break;
        case 'W': /* Unield */
            unwield_current_weapon(engine, actor);
            break;
        default:
            engine->gui->message(engine, TCOD_grey, "Unknown command: %c.\n",
                                 engine->key.c);
            break;
    }
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
            case TCODK_KP1:
            case TCODK_1:
                dx = -1;
                dy = 1;
                break;
            case TCODK_KP4:
            case TCODK_LEFT:
            case TCODK_4:
                dx = -1;
                break;
            case TCODK_7:
            case TCODK_KP7:
                dx = -1;
                dy = -1;
                break;
            case TCODK_UP:
            case TCODK_8:
            case TCODK_KP8:
                dy = -1;
                break;
            case TCODK_9:
            case TCODK_KP9:
                dx = 1;
                dy = -1;
                break;
            case TCODK_RIGHT:
            case TCODK_6:
            case TCODK_KP6:
                dx = 1;
                break;
            case TCODK_KP3:
                dx = 1;
                dy = 1;
                break;
            case TCODK_DOWN:
            case TCODK_KP2:
                dy = 1;
                break;
            case TCODK_KP5:
            case TCODK_5:
                engine->game_status = NEW_TURN;
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

