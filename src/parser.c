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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "monsters.h"

#define MAX_LINE_LEN 80

/*
 * Pick a random actor from a file containing monsters, items, spells, etc. 
 * @param filename - an info file to parse, in Record-Jar format, described
 * in The Art of Unix Programming.
 * @param realm_id - the dungeon level, portal id, or id of a place. This is
 * used to calculate the chance of a monster/item/spell/etc appearing in this
 * realm.
 * @param used to return the lucky actor parsed from the file. 
 */
int parse_jar(char *filename, int realm_id, struct actor **actor)
{
    FILE *file = fopen(filename, "r");
    int len = 0;
    char line[MAX_LINE_LEN];

    /* Picking any monster, doesn't matter which, as it will be overwritten */
    struct actor *tmp_actor = mktroll(0, 0);

    while (fgets(line, MAX_LINE_LEN, file)) {
        char key[MAX_LINE_LEN];
        char val[MAX_LINE_LEN];
        char colon;

        /* % separates separate items, monsters, spells, etc. */
        if (starts_with_c(line, '%')) {
            *actor = tmp_actor;
        }

        if (!starts_with_c(line, '#')) {
			if (sscanf(line, "%[^:] %c %s", key, &colon, val) != 3) {
				return -1;
			}
            if (!strcmp(key, "name")) {
                char *tmp = malloc(80);
				if (!tmp) {
					return -1;
				}
                if (strlen(tmp_actor->name) < strlen(val))
                    tmp_actor->name = realloc(tmp_actor->name, sizeof(val) + 1);
                strcpy(tmp, val);
            } else if (!strcmp(key, "ch")) {
                tmp_actor->ch = val[0];
            } else if (!strcmp(key, "strength")) {
                tmp_actor->ai->skills[SKILL_STRENGTH].val = atof(val);
            } else if (!strcmp(key, "intelligence")) {
                tmp_actor->ai->skills[SKILL_INTELL].val = atof(val);
            } else if (!strcmp(key, "dexterity")) {
                tmp_actor->ai->skills[SKILL_AGILITY].val = atof(val);
            } else if (!strcmp(key, "power")) {
                tmp_actor->attacker->power = atof(val);
            }
        }
    }

    return 0;
}


/*
 * Find a blank space in s starting from start up to and including n
 * characters. The n parameter can be negative to indicate search
 * backwards.
 */
int find_blnk(char s[], int start, int n)
{
    int pos = -1;

    /* if searching backwards, move cursor to the last character before
       '\0' */
    if (n < 0) {
        if (s[start] == '\0')
            --start;
        if (s[start] == '\n')
            --start;
    } else {
        /* 
         * correct improper start, but leave it as it is if searching
         * backwards
         **/
        if (start < 0)
            start = 0;
    }

    while (s[start] != '\0' && s[start] != '\n' && s[start] != EOF) {
        if (start < 0)
            printf("searching in vain: %d\n", start);

        if (s[start] == ' ')
            pos = start;

        if (n > 0)
            ++start;
        else
            --start;
    }

    return pos;
}

/*
 * find the last non-blank character in s. return -1 if no
 * non-whitespace characters found
 */
int find_last_nonblnk(char s[])
{
    int pos = -1; /* position of the last non-blank character */
    // int len = strlen(s); /* length of s */
    int len = 10;

    int i = len;
    while (s[i] != ' ') {
        pos = i;
        i--;
    }

    if (pos > 0) {
        while (s[pos] == ' ')
            pos--;
    }

    return pos;
}

int starts_with_c(char s[], char ch)
{
    if (strlen(s) < 0)
        return 0;

    return s[0] == ch;
}

int read_key_val(char *str, char *key, char *val)
{
	return 0;
}
