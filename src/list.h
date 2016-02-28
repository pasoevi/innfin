/*
  Copyright (C) 2013 Sergi Pasoev.

  This pragram is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

  Written by Sergi Pasoev <s.pasoev@gmail.com>
*/

/* Define a structure for the list of dataoids */
#ifndef LIST_H
#define LIST_H
typedef struct ListElmt_ {
        void *data;
        struct ListElmt_ *next;
} ListElmt;

typedef struct List_ {
        int size;
        int (*match) (const void *key1, const void *key2);
        void (*destroy)(void *data);

        ListElmt *head;
        ListElmt *tail;
} List;

void list_init(List *list, void (*destroy)(void *data));
void list_destroy(List *list);
int list_ins_next(List *list, ListElmt *element, const void *data);
int list_rem_next(List *list, ListElmt *element, void **data);
int list_rem(List *list, ListElmt *element);
#define list_size(list) ((list)->size)
#define list_head(list) ((list)->head)
#define list_tail(list) ((list)->tail)
#define list_ishead(list, element) ((element) == (list)->head ? 1 : 0)

int list_istail(ListElmt *element);
void * list_data(ListElmt *element);
void * list_next(ListElmt *element);

#endif
