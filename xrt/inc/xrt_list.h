/**
 * @file    xrt_list.h
 * @brief   Circular doubly linked list.
 *
 * The list allocates nothing: every node lives inside the object that owns it
 * and the caller keeps it alive.
 *
 */

#ifndef INC_XRT_LIST_H_
#define INC_XRT_LIST_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct cdll_node_t cdll_node;
typedef struct cdll_list_t cdll_list;

/** @brief A list element*/
struct cdll_node_t{
    void*      data;  /**< Owner of this node */
    cdll_node* prev;
    cdll_node* next;
};

/** @brief List anchor. */
struct cdll_list_t{
    cdll_node* head;  /**< First element, NULL when the list is empty. */
    uint8_t    size;  /**< Number of elements currently linked. */
};

/**
 * @brief Resets a list to the empty state.
 *
 * @param list List to initialise.
 * @return false when @p list is NULL, true otherwise.
 *
 */
bool cdll_init_list(cdll_list* list);

/**
 * @brief Links a node as the last element of a list.
 *
 * @param list List to insert into.
 * @param node Node to insert.
 * @return false when any argument is NULL, true otherwise.
 */
bool cdll_insert_node_to_tail(cdll_list* list, cdll_node* node);

/**
 * @brief Links a node before an existing one,
 *
 * @param list     List both nodes belong to.
 * @param existing Node to insert before. Assumed to be in @p list; this
 *                 is not verified, as checking would cost a full traversal.
 * @param new_node Node to insert.
 * @return false when any argument is NULL, true otherwise.
 *
 */
bool cdll_insert_node_before(cdll_list* list, cdll_node* existing, cdll_node* new_node);

/**
 * @brief Unlinks a node the caller already knows to be in the list.
 *
 * @param list List to remove from.
 * @param node Node to remove. Membership is assumed, not verified: passing a
 *             node from another list corrupts both the links and @c size.
 *
 * @note The node is left self-linked, so it is safe to insert elsewhere.
 */
void cdll_remove_known_node_from_list(cdll_list* list, cdll_node* node);

/**
 * @brief Returns the first element of a list.
 *
 * @param list List to read.
 * @return The head node, or NULL when @p list is NULL or empty.
 */
cdll_node* cdll_get_list_head(cdll_list* list);

#endif /* INC_XRT_LIST_H_ */