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
static void mkmemorial(struct actor *actor)
{

}

static void make_player_ghost(struct engine *engine, struct actor *player)
{
    /*
     * TODO: Unimplemented.
     */
}

/* Transform the actor into a decaying corpse */
void player_die(struct engine *engine, struct actor *actor,
                struct actor *killer)
{
    engine->gui->message(engine, TCOD_red, "You die.\n");
    /* Call the common death function */
    die(engine, actor, NULL);
    mkmemorial(actor);
    engine->game_status = DEFEAT;

}

struct actor *mkplayer(int x, int y)
{
    struct actor *player = mkactor(x, y, '@', "you", TCOD_white);

    /* Artificial intelligence */
    player->ai = init_ai(player_update, player_move_or_attack);
    player->ai->skills.strength = 10;
    player->ai->skills.intelligence = 9;
    player->ai->skills.fighting = 12;

    player->attacker = init_attacker(10, attack);

    player->life = mklife(100, 100, 6, "your dead body", take_damage,
                             player_die);
    player->life->regen = regen_hp;

    player->life->max_stomach = 500;
    player->life->stomach = player->life->max_stomach;

    player->inventory = init_container(26);

    return player;
}

bool descend(struct engine *engine, struct actor *actor, struct actor *stairs)
{
    if (actor == NULL || is_dead(actor) || stairs == NULL)
        return false;

    if (actor->x == stairs->x && actor->y == stairs->y) {
        load_level(engine, engine->level + 1);
        return true;
    } else {
        engine->gui->message(engine, TCOD_gray,
                             "You can't climb down here. Try on stairs.");
        return false;
    }
}

bool player_move_or_attack(struct engine *engine, struct actor *player,
                           int targetx, int targety)
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

    if (is_wall(engine->map, targetx, targety))
        return false;

    /* Look for actors to attack */
    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors);
         iter++) {
        struct actor *actor = *iter;
        if (actor->life && !is_dead(actor) &&
            actor->x == targetx && actor->y == targety) {
            /* There is an actor there, cat't walk */
            player->attacker->attack(engine, player, actor);
            return false;
        }
    }

    /* Look for corpses or pickable items */
    for (iter = (struct actor **) TCOD_list_begin(engine->actors);
         iter != (struct actor **) TCOD_list_end(engine->actors);
         iter++) {
        struct actor *actor = *iter;
        bool corpse_or_item =
                (actor->life && is_dead(actor)) || actor->pickable;
        if (corpse_or_item && actor->x == targetx
            && actor->y == targety)
            engine->gui->message(engine, TCOD_light_gray,
                                 "There's %s here\n",
                                 (*iter)->name);
    }

    player->x = targetx;
    player->y = targety;

    return true;
}

void display_stats(struct engine *engine, struct actor *actor)
{
    /* Display the items frame */
    TCOD_console_t *con = engine->gui->inventory_con;
    TCOD_color_t color = (TCOD_color_t) {200, 180, 50};
    TCOD_console_set_default_foreground(con, color);
    TCOD_console_print_frame(con, 0, 0, INVENTORY_WIDTH,
                             INVENTORY_HEIGHT, true,
                             TCOD_BKGND_DEFAULT, "Stats");

    /*
     * Count the items that specify the predicate and display the
     * items with their respective shortcuts.
     */
    TCOD_console_set_default_foreground(con, TCOD_white);
    int y = 0;
    TCOD_console_print(con, 2, ++y, "(%c) strength %.f", 's', engine->player->ai->skills.strength);
    TCOD_console_print(con, 2, ++y, "(%c) intelligence %.f", 'i', engine->player->ai->skills.intelligence);
    TCOD_console_print(con, 2, ++y, "(%c) agility %.f", 'a', engine->player->ai->skills.strength);


        /* Blit the items console to the root console. */
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
               TCOD_list_size(actor->inventory->items)) {
            struct actor *tmp =
                    TCOD_list_get(actor->inventory->items, actor_index);
        }
    }


}

struct actor *choose_from_inventory(struct engine *engine,
                                    struct actor *actor,
                                    const char *window_title,
                                    bool(*predicate)(struct actor *actor))
{
    /* Display the items frame */
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
    int nitems = 0;
    int shortcut = 'a';
    int y = 1;
    struct actor **iter;
    for (iter = (struct actor **) TCOD_list_begin(actor->inventory->items);
         iter != (struct actor **) TCOD_list_end(actor->inventory->items);
         iter++) {
        struct actor *item = *iter;
        if (predicate(item)) {
            TCOD_console_print(con, 2, y, "(%c) %s", shortcut, item->name);
            y++;
            nitems++;
        }
        shortcut++;

    }

    /* Blit the items console to the root console. */
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
               TCOD_list_size(actor->inventory->items)) {
            struct actor *tmp =
                    TCOD_list_get(actor->inventory->items, actor_index);
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

void handle_key(struct engine *engine, struct actor *actor)
{
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
        case 'i': /* display items */
            invoke_command(engine, NULL, is_usable, "items");
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
        case 's': /* Display stats */
            display_stats(engine, engine->player);
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
                handle_key(engine, actor);
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

    if (engine->game_status == NEW_TURN && actor->life)
        actor->life->regen(engine, actor);
}

double regen_hp(struct engine *engine, struct actor *actor)
{
    double hp_gained = 0.1f;
    if (is_dead(actor) || !actor->life ||
        actor->life->hp >= actor->life->max_hp)
        return 0.f;

    actor->life->hp += hp_gained;

    double hp_overflow = actor->life->hp - actor->life->max_hp;

    if (hp_overflow > 0) {
        hp_gained = hp_gained - hp_overflow;
        actor->life->hp = actor->life->max_hp;
    }

    return hp_gained;
}

