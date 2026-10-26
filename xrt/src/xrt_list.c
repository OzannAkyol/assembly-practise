/*
 * xrt_list.c
 *
 *  Created on: Apr 4, 2026
 *      Author: ozi
 */

#include <stddef.h>
#include <stdint.h>

#include "xrt_list.h"

static void cdll_increment_list_size(cdll_list* list);
static void cdll_decrement_list_size(cdll_list* list);

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
		node -> next = node;
		node -> prev = node;
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

bool cdll_insert_node_before(cdll_list* list, cdll_node* existing, cdll_node* new_node){
	if(list == NULL || existing == NULL || new_node == NULL){
		return false;
	}

	new_node -> next = existing;
	new_node -> prev = existing ->prev;

	existing -> prev -> next =  new_node;
	existing -> prev = new_node;

	if(existing == list -> head){
		list -> head = new_node;
	}

	cdll_increment_list_size(list);

	return true;
}

static void cdll_increment_list_size(cdll_list* list){
	if(list != NULL){
		list->size++;
	}
}

static void cdll_decrement_list_size(cdll_list* list){
	if(list != NULL){
		list->size--;
	}
}
