#ifndef POS_H
#define POS_H

#include <stdio.h>
#include <stdlib.h>

struct pos {
    unsigned int r, c;
};

typedef struct pos pos;


typedef struct pq_entry pq_entry;

struct pq_entry {
    pos p;
    pq_entry *next, *prev;
};


struct posqueue {
    pq_entry *head, *tail;
    unsigned int len;
};

typedef struct posqueue posqueue;


/**
 * make_pos
 * 
 * Creates and returns a `pos` structure initialized with the given row and 
 *  column values.
 * 
 * Parameters:
 *   - r: The row value (unsigned integer).
 *   - c: The column value (unsigned integer).
 * 
 * Returns:
 *   - A `pos` structure with the specified row and column.
 */
pos make_pos(unsigned int r, unsigned int c);


/**
 * posqueue_new
 * 
 * Allocates and initializes a new, empty position queue.
 * 
 * Returns:
 *   - A pointer to the newly created `posqueue` structure.
 *     The queue's `head` and `tail` pointers are set to NULL, and its length 
 *      is initialized to 0.
 * 
 * Note:
 *   - The caller is responsible for freeing the queue using `posqueue_free`.
 */
posqueue* posqueue_new();


/**
 * pos_enqueue
 * 
 * Adds a new position to the back of the queue.
 * 
 * Parameters:
 *   - q: A pointer to the `posqueue` structure.
 *   - p: The `pos` structure to add to the queue.
 * 
 * Modifies:
 *   - Updates the queue's `tail` to point to the new entry.
 *   - Increases the `len` of the queue by 1.
 * 
 * Note:
 *   - Raises an error if the queue pointer is NULL.
 */
void pos_enqueue(posqueue* q, pos p);


/**
 * pos_dequeue
 * 
 * Removes and returns the position at the front of the queue.
 * 
 * Parameters:
 *   - q: A pointer to the `posqueue` structure.
 * 
 * Returns:
 *   - The `pos` structure at the front of the queue.
 * 
 * Modifies:
 *   - Updates the `head` of the queue to point to the next entry.
 *   - Decreases the `len` of the queue by 1.
 * 
 * Note:
 *   - Raises an error if the queue is empty or NULL.
 */
pos pos_dequeue(posqueue* q);


/**
 * posqueue_remback
 * 
 * Removes and returns the position at the back of the queue.
 * 
 * Parameters:
 *   - q: A pointer to the `posqueue` structure.
 * 
 * Returns:
 *   - The `pos` structure at the back of the queue.
 * 
 * Modifies:
 *   - Updates the `tail` of the queue to point to the previous entry.
 *   - Decreases the `len` of the queue by 1.
 * 
 * Note:
 *   - Raises an error if the queue is empty or NULL.
 */
pos posqueue_remback(posqueue* q);


/**
 * posqueue_free
 * 
 * Frees all memory associated with the position queue.
 * 
 * Parameters:
 *   - q: A pointer to the `posqueue` structure.
 * 
 * Modifies:
 *   - Deallocates all entries in the queue.
 *   - Frees the queue itself.
 */
void posqueue_free(posqueue* q);


/* General-purpose function to be used across files
   It raises an error if memory allocation fails */
void check_malloc(void* p);


/* General-purpose function to be used across files
   It raises an error if the pointer is null 
   This function is called at the start of functions which cannot handle null 
   pointers */
void check_null_pointer(void* p);


#endif /* POS_H */
