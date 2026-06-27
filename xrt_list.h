/*
 * xrt_list.h
 *
 *  Created on: Apr 4, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_LIST_H_
#define INC_XRT_LIST_H_

#include "stdbool.h"
#include "cmsis_gcc.h"

typedef struct cdll_node_t cdll_node;
typedef struct cdll_list_t cdll_list;

struct cdll_node_t{
	void* data;
	cdll_node* prev;
	cdll_node* next;
};

struct cdll_list_t{
	cdll_node* head;
	uint8_t size;
};

bool cdll_init_list(cdll_list* list);
bool cdll_insert_node_to_tail(cdll_list* list, cdll_node* node);
bool cdll_remove_node_from_list(cdll_list* list, cdll_node* node);
bool cdll_sort_list(cdll_list* list);
bool cdll_insert_node_to_head(cdll_list* list, cdll_node* node);
cdll_node* cdll_get_list_head(cdll_list* list);
/*
 * It is created for kernel functions, Since it affects the complexity
 */ //__STATIC_FORCEINLINE void cdll_remove_known_node_from_list(cdll_list* list, cdll_node* node)
void cdll_remove_known_node_from_list(cdll_list* list, cdll_node* node);
bool cdll_push_data_with_priority_order(cdll_list* list, cdll_node* node);

#endif /* INC_XRT_LIST_H_ */
