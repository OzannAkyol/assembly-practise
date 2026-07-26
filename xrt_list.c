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

__STATIC_FORCEINLINE void cdll_increment_list_size(cdll_list* list);
__STATIC_FORCEINLINE void cdll_decrement_list_size(cdll_list* list);

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

bool cdll_init_list(cdll_list* list){
	if(list == NULL){
		return false;
	}

	list-> head = NULL;

	return true;
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

void cdll_traverse_list(cdll_list* list);
/*
 *@sort list as descending order
 */

bool cdll_sort_list(cdll_list* list) {
    if (list == NULL || list->size < 2) {
        return false;
    }

    int passes = (int)list->size - 1;  // avoids uint8_t underflow bug too

    while (passes > 0) {

        cdll_node* current_node = list->head;      // reset each pass
        cdll_node* next_node    = current_node->next; // reset each pass

        while (next_node != list->head) {
            TCB_t* current_node_tcb = (TCB_t*)current_node->data;
            TCB_t* next_node_tcb    = (TCB_t*)next_node->data;

            if (current_node_tcb->currentPriority <= next_node_tcb->currentPriority) {
                cdll_node* dummy_next = next_node->next;

                current_node->next       = next_node->next;
                next_node->next->prev    = current_node;
                next_node->prev          = current_node->prev;
                next_node->next          = current_node;
                current_node->prev->next = next_node;
                current_node->prev       = next_node;

                if (current_node == list->head) {
                    list->head = next_node;
                }

                next_node = dummy_next;
            } else {
                current_node = next_node;
                next_node    = next_node->next;
            }
        }
        passes--;
    }

    cdll_traverse_list(list);
    return true;
}

void cdll_traverse_list(cdll_list* list){
	cdll_node* tmp = list -> head;
	uint8_t size = list -> size;
	volatile TCB_t* node = (TCB_t*)tmp ->data;
	(void)node;
	while(size-- > 0){
		node = (TCB_t*)tmp ->data;
		tmp = tmp -> next;
	}

}

/*
 *  @note: this function placed node according to it's priority order.
 *
 */
bool cdll_push_data_with_priority_order(cdll_list* list, cdll_node* node){
	if(list == NULL || node == NULL){
		return false;
	}

	if(list -> head == NULL){
		list -> head = node;
		list -> head -> prev = node;
		list -> head -> next = node;
		cdll_increment_list_size(list);
		return true;
	}

	TCB_t* given_node = (TCB_t*)node->data;

	cdll_node* tmp = list -> head;
	TCB_t* tmp_thread = (TCB_t*) tmp -> data;


	uint32_t size = list -> size;

	while(size-- > 0){
		if(tmp_thread -> currentPriority > given_node -> currentPriority){
			tmp = tmp -> next;

			if(tmp == list->head){
				cdll_insert_node_to_tail(list, node);
				break;
			}

			tmp_thread = (TCB_t*)tmp -> data;

		}
		else if(tmp_thread -> currentPriority == given_node -> currentPriority){
			tmp = tmp -> next;

			if(tmp == list->head){
				cdll_insert_node_to_tail(list, node);
				break;
			}

			tmp_thread = (TCB_t*)tmp -> data;


			if(tmp_thread -> currentPriority < given_node -> currentPriority){
				node -> next = tmp ;
				node -> prev = tmp -> prev;

				tmp -> prev -> next = node;
				tmp -> prev = node;

				cdll_increment_list_size(list);

				break;
			}
		}
		else {
			cdll_insert_node_to_head(list, node);
			break;
		}
	}

	return true;
}

__STATIC_FORCEINLINE void cdll_increment_list_size(cdll_list* list){
	if(list != NULL){
		list->size++;
	}
}

__STATIC_FORCEINLINE void cdll_decrement_list_size(cdll_list* list){
	if(list != NULL){
		list->size--;
	}
}
