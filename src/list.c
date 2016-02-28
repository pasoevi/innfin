/*
  Copyright (C) 2013, 2015 Sergi Pasoev.

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

#include <stdlib.h>
#include <string.h>
#include "list.h"

void list_init(List *list, void (*destroy)(void *data)) {
        list->size = 0;
        list->destroy = destroy;
        list->head = NULL;
        list->tail = NULL;

}

void * list_data(ListElmt *element){
        if(element == NULL){
                return NULL;
        }

        return element->data;
}

void * list_next(ListElmt *element){
        if(element == NULL){
                return NULL;
        }

        return element->next;
}

int list_istail(ListElmt *element){
        if(element == NULL){
                return -1;
        }

        return element->next == NULL ? 1 : 0;
}

void list_destroy(List *list) {
        void *data;

        while (list->size > 0)
                if (list_rem_next(list, NULL, (void **) &data) == 0 &&
                    list->destroy != NULL)
                        list->destroy(data);
        memset(list, 0, sizeof (List));
}

int list_ins_next(List *list, ListElmt *element, const void *data) {
        ListElmt *new_element;
        if ((new_element = (ListElmt *) malloc(sizeof *new_element)) == NULL) {
                return -1;
        }

        new_element->data = (void *) data;
        new_element->next = NULL;

        /* insertion at the head */
        if (element == NULL) {
                if (list_size(list) == 0) {
                        list->tail = new_element;
                }

                new_element->next = list->head;
                list->head = new_element;
        } else {
                if (element->next == NULL) {
                        list->tail = new_element;
                }

                new_element->next = element->next;
                element->next = new_element;
        }

        list->size++;

        return 0;
}

int list_rem_next(List *list, ListElmt *element, void **data) {
        ListElmt *old_element;

        if (list_size(list) == 0) {
                return -1;
        }

        /* Deleting at the head */
        if (element == NULL) {
                *data = list->head->data;
                old_element = list->head;
                list->head = list->head->next;

                if (list_size(list) == 1) {
                        list->tail = NULL;
                }
        } else {
                if (element->next == NULL) {
                        return -1;
                }

                *data = element->next->data;
                old_element = element->next;
                element->next = element->next->next;

                if (element->next == NULL) {
                        list->tail = element;
                }
        }

        if(old_element != NULL){
                free(old_element);
        }

        list->size--;
        return 0;
}

int list_rem(List *list, ListElmt *element) {
	ListElmt *elmt;
	void *data;
	
        if(list_ishead(list, element)){
                /* Pass NULL to signify deleting the head */
                list_rem_next(list, NULL, &data);
		if(data != NULL){
			free(data);
		}
        }else{
        
		for(elmt = list_head(list); elmt != NULL; elmt = list_next(elmt)){
			if(list_next(elmt) == element){
				list_rem_next(list, elmt, &data);
				if(data != NULL){
					free(data);
				}
			}
		}
	}

        return 0;
}
