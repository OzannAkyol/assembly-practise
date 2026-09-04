/*
 * xrt_list.c
 *
 *  Created on: Apr 4, 2026
 *      Author: ozi
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#include "xrt_list.h"
#include "xrt_thread.h"

bool cdll_init_list(cdll_list* list){
	if(list == NULL){
		return false;
	}

	list-> head = NULL;
	list->size = 0;

	return true;
}

void cdll_remove_known_node_from_list(cdll_list* list, cdll_node* node){
	if(list -> size == 1){
		list -> size = 0;
		list -> head = NULL;
	}
	else{
		node -> next -> prev = node -> prev;
		node -> prev -> next = node -> next;

		if(node == list -> head){
			list -> head = node -> next;
		}

		node -> next = node;
		node -> prev = node;
		cdll_decrement_list_size(list);
	}
}

bool cdll_insert_node_to_tail(cdll_list* list, cdll_node* node){
	if(list-> head == NULL){
		list-> head = node;
		node-> prev = node;
		node-> next = node;
	}
	else{
		node->next = list->head;
		node->prev = list->head->prev;

		list->head->prev->next = node;
		list->head->prev = node;
	}
	cdll_increment_list_size(list);

	return true;
}

bool cdll_insert_node_to_head(cdll_list* list, cdll_node* node){
	if(list == NULL || node == NULL){
		return false;
	}

	if(list ->size == 0){
		node -> next = node;
		node -> prev = node;

	}
	else{
		node -> next = list -> head;
		node -> prev = list -> head -> prev;

		node -> prev -> next = node;
		list -> head -> prev = node;

	}

	list->head = node;
	cdll_increment_list_size(list);

	return true;
}

cdll_node* cdll_get_list_head(cdll_list* list){
	if(list != NULL && list->head != NULL){
		return list->head;
	}
	return NULL;
}

bool cdll_remove_node_from_list(cdll_list* list, cdll_node* node){
	if(list == NULL || node == NULL){
		return false;
	}
	cdll_node* tmp = list->head;

	while(tmp != node){
		tmp = tmp->next;
		if(tmp == list->head){
			return false;
		}
	}

	if(node == list -> head){
		list->head = tmp -> next;
		if(list->size == 1){

			cdll_decrement_list_size(list);

			list->head = NULL;
			return true;
		}
	}

	tmp -> next -> prev = tmp -> prev;
	tmp -> prev -> next = tmp ->next;

	tmp->next = tmp;
	tmp->prev = tmp;

	cdll_decrement_list_size(list);


	return true;
}

/*
 *  @note: this function placed node according to it's priority order.
 *
 */
bool cdll_push_data_with_priority_order(cdll_list* list, cdll_node* given_node){
	if(list == NULL || given_node == NULL){
		return false;
	}

	if(list -> head == NULL){
		list -> head = given_node;
		list -> head -> prev = given_node;
		list -> head -> next = given_node;
		cdll_increment_list_size(list);
		return true;
	}

	TCB_t* given_thread = (TCB_t*)given_node->data;
	cdll_node* tmp_node = list -> head;

	uint8_t size = list -> size;
	while(size-- > 0){
		TCB_t* tmp_thread = (TCB_t*) tmp_node -> data;

		if(tmp_thread -> current_priority < given_thread -> current_priority){
			given_node -> next = tmp_node;
			given_node -> prev = tmp_node -> prev;

			tmp_node -> prev -> next = given_node;
			tmp_node -> prev = given_node;

			if(tmp_node == list -> head){
				list -> head = given_node;
			}
			cdll_increment_list_size(list);
			return true;
		}
		tmp_node = tmp_node -> next;
	}

	cdll_insert_node_to_tail(list, given_node);

	return true;
}
