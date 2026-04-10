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

bool cdll_init_list(cdll_list* list){
	if(list == NULL){
		return false;
	}

	list-> head = NULL;

	return true;
}

bool cdll_insert_node_to_list(cdll_list* list, cdll_node* node){
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
	list->size++;

	return true;
}

bool cddl_remove_node_from_list(cdll_list* list, cdll_node* node){
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
			list->size--;
			list->head = NULL;
			return true;
		}
	}

	tmp -> next -> prev = tmp -> prev;
	tmp -> prev -> next = tmp ->next;

	tmp->next = NULL;

	list->size--;


	return true;
}


bool cdll_sort_list(cdll_list* list){
	return true;
}
