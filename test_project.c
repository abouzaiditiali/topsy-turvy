#include <criterion/criterion.h>
#include <limits.h>
#include "logic.h"

/* Tests for pos.c */

/** make_pos **/
Test(make_pos, normal_values) {
    pos p = make_pos(3, 4);
    cr_assert_eq(p.r, 3);
    cr_assert_eq(p.c, 4);
}

Test(make_pos, origin) {
    pos p = make_pos(0, 0);
    cr_assert_eq(p.r, 0);
    cr_assert_eq(p.c, 0);
}

Test(make_pos, large_values) {
    pos p = make_pos(1000, 2000);
    cr_assert_eq(p.r, 1000);
    cr_assert_eq(p.c, 2000);
}

Test(make_pos, max_values) {
    pos p = make_pos(UINT_MAX, UINT_MAX);
    cr_assert_eq(p.r, UINT_MAX);
    cr_assert_eq(p.c, UINT_MAX);
}

Test(make_pos, mixed_values) {
    pos p = make_pos(0, 500);
    cr_assert_eq(p.r, 0);
    cr_assert_eq(p.c, 500);

    p = make_pos(500, 0);
    cr_assert_eq(p.r, 500);
    cr_assert_eq(p.c, 0);
}

/** posqueue_new **/
Test(posqueue_new, valid_creation) {
    posqueue *q = posqueue_new();
    cr_assert_not_null(q);
    cr_assert_null(q->head);
    cr_assert_null(q->tail);
    cr_assert_eq(q->len, 0);
    posqueue_free(q);
}

Test(posqueue_new, multiple_allocations) {
    posqueue *q1 = posqueue_new();
    posqueue *q2 = posqueue_new();
    cr_assert_not_null(q1);
    cr_assert_not_null(q2);
    cr_assert_neq(q1, q2);
    posqueue_free(q1);
    posqueue_free(q2);
}

Test(posqueue_new, memory_cleanup) {
    posqueue *q = posqueue_new();
    cr_assert_not_null(q);
    posqueue_free(q);
    cr_assert(true);
}

/** pos_enqueue **/
Test(pos_enqueue, enqueue_first_element) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(1, 1);
    pos_enqueue(q, p1);
    cr_assert_not_null(q->head);
    cr_assert_not_null(q->tail);
    cr_assert_eq(q->head, q->tail);
    cr_assert_eq(q->head->p.r, 1);
    cr_assert_eq(q->head->p.c, 1);
    cr_assert_eq(q->len, 1);
    posqueue_free(q);
}

Test(pos_enqueue, enqueue_multiple_elements) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(1, 1);
    pos p2 = make_pos(2, 2);
    pos p3 = make_pos(3, 3);
    pos_enqueue(q, p1);
    pos_enqueue(q, p2);
    pos_enqueue(q, p3);
    cr_assert_eq(q->len, 3);
    cr_assert_eq(q->head->p.r, 1);
    cr_assert_eq(q->head->p.c, 1);
    cr_assert_eq(q->tail->p.r, 3);
    cr_assert_eq(q->tail->p.c, 3);
    cr_assert_eq(q->head->next->p.r, 2);
    cr_assert_eq(q->tail->prev->p.r, 2);
    posqueue_free(q);
}

Test(pos_enqueue, enqueue_large_number_of_elements) {
    posqueue *q = posqueue_new();
    for (unsigned int i = 0; i < 1000; i++) {
        pos p = make_pos(i, i);
        pos_enqueue(q, p);
    }
    cr_assert_eq(q->len, 1000);
    cr_assert_eq(q->head->p.r, 0);
    cr_assert_eq(q->head->p.c, 0);
    cr_assert_eq(q->tail->p.r, 999);
    cr_assert_eq(q->tail->p.c, 999);
    posqueue_free(q);
}

Test(pos_enqueue, enqueue_with_same_values) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(5, 5);
    pos_enqueue(q, p1);
    pos_enqueue(q, p1);
    cr_assert_eq(q->len, 2);
    cr_assert_eq(q->head->p.r, 5);
    cr_assert_eq(q->head->p.c, 5);
    cr_assert_eq(q->tail->p.r, 5);
    cr_assert_eq(q->tail->p.c, 5);
    posqueue_free(q);
}

Test(pos_enqueue, enqueue_into_non_empty_queue) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(1, 1);
    pos p2 = make_pos(2, 2);
    pos p3 = make_pos(3, 3);
    pos_enqueue(q, p1);
    pos_enqueue(q, p2);
    cr_assert_eq(q->len, 2);
    cr_assert_eq(q->head->p.r, 1);
    cr_assert_eq(q->head->p.c, 1);
    cr_assert_eq(q->tail->p.r, 2);
    cr_assert_eq(q->tail->p.c, 2);
    pos_enqueue(q, p3);
    cr_assert_eq(q->len, 3);
    cr_assert_eq(q->tail->p.r, 3);
    cr_assert_eq(q->tail->p.c, 3);
    posqueue_free(q);
}

Test(pos_enqueue, enqueue_and_validate_links) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(1, 1);
    pos p2 = make_pos(2, 2);
    pos p3 = make_pos(3, 3);
    pos_enqueue(q, p1);
    pos_enqueue(q, p2);
    pos_enqueue(q, p3);
    cr_assert_eq(q->head->next->p.r, 2);
    cr_assert_eq(q->head->next->next->p.r, 3);
    cr_assert_eq(q->tail->prev->p.r, 2);
    cr_assert_eq(q->tail->prev->prev->p.r, 1);
    posqueue_free(q);
}

/** pos_dequeue **/
Test(pos_dequeue, dequeue_single_element) {
    posqueue *q = posqueue_new();
    pos p = make_pos(1, 1);
    pos_enqueue(q, p);

    pos dequeued = pos_dequeue(q);
    cr_assert_eq(dequeued.r, 1);
    cr_assert_eq(dequeued.c, 1);
    cr_assert_null(q->head);
    cr_assert_null(q->tail);
    cr_assert_eq(q->len, 0);

    posqueue_free(q);
}

Test(pos_dequeue, dequeue_multiple_elements) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(1, 1);
    pos p2 = make_pos(2, 2);
    pos p3 = make_pos(3, 3);

    pos_enqueue(q, p1);
    pos_enqueue(q, p2);
    pos_enqueue(q, p3);

    pos dequeued = pos_dequeue(q);
    cr_assert_eq(dequeued.r, 1);
    cr_assert_eq(dequeued.c, 1);
    cr_assert_eq(q->head->p.r, 2);
    cr_assert_eq(q->len, 2);

    dequeued = pos_dequeue(q);
    cr_assert_eq(dequeued.r, 2);
    cr_assert_eq(dequeued.c, 2);
    cr_assert_eq(q->head->p.r, 3);
    cr_assert_eq(q->len, 1);

    dequeued = pos_dequeue(q);
    cr_assert_eq(dequeued.r, 3);
    cr_assert_eq(dequeued.c, 3);
    cr_assert_null(q->head);
    cr_assert_null(q->tail);
    cr_assert_eq(q->len, 0);

    posqueue_free(q);
}

Test(pos_dequeue, dequeue_and_verify_structure) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(1, 1);
    pos p2 = make_pos(2, 2);

    pos_enqueue(q, p1);
    pos_enqueue(q, p2);

    pos dequeued = pos_dequeue(q);
    cr_assert_eq(dequeued.r, 1);
    cr_assert_eq(dequeued.c, 1);
    cr_assert_eq(q->head->p.r, 2);
    cr_assert_eq(q->len, 1);
    cr_assert_null(q->head->prev);

    dequeued = pos_dequeue(q);
    cr_assert_eq(dequeued.r, 2);
    cr_assert_eq(dequeued.c, 2);
    cr_assert_null(q->head);
    cr_assert_null(q->tail);
    cr_assert_eq(q->len, 0);

    posqueue_free(q);
}

Test(pos_dequeue, enqueue_and_dequeue_interleaved) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(1, 1);
    pos p2 = make_pos(2, 2);
    pos p3 = make_pos(3, 3);

    pos_enqueue(q, p1);
    pos_enqueue(q, p2);

    pos dequeued = pos_dequeue(q);
    cr_assert_eq(dequeued.r, 1);
    cr_assert_eq(dequeued.c, 1);

    pos_enqueue(q, p3);

    dequeued = pos_dequeue(q);
    cr_assert_eq(dequeued.r, 2);
    cr_assert_eq(dequeued.c, 2);

    dequeued = pos_dequeue(q);
    cr_assert_eq(dequeued.r, 3);
    cr_assert_eq(dequeued.c, 3);

    cr_assert_null(q->head);
    cr_assert_null(q->tail);
    cr_assert_eq(q->len, 0);

    posqueue_free(q);
}

/** posqueue_remback **/
Test(posqueue_remback, remove_single_element) {
    posqueue *q = posqueue_new();
    pos p = make_pos(1, 1);
    pos_enqueue(q, p);

    pos removed = posqueue_remback(q);
    cr_assert_eq(removed.r, 1);
    cr_assert_eq(removed.c, 1);
    cr_assert_null(q->head);
    cr_assert_null(q->tail);
    cr_assert_eq(q->len, 0);

    posqueue_free(q);
}

Test(posqueue_remback, remove_multiple_elements) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(1, 1);
    pos p2 = make_pos(2, 2);
    pos p3 = make_pos(3, 3);

    pos_enqueue(q, p1);
    pos_enqueue(q, p2);
    pos_enqueue(q, p3);

    pos removed = posqueue_remback(q);
    cr_assert_eq(removed.r, 3);
    cr_assert_eq(removed.c, 3);
    cr_assert_eq(q->tail->p.r, 2);
    cr_assert_eq(q->len, 2);

    removed = posqueue_remback(q);
    cr_assert_eq(removed.r, 2);
    cr_assert_eq(removed.c, 2);
    cr_assert_eq(q->tail->p.r, 1);
    cr_assert_eq(q->len, 1);

    removed = posqueue_remback(q);
    cr_assert_eq(removed.r, 1);
    cr_assert_eq(removed.c, 1);
    cr_assert_null(q->head);
    cr_assert_null(q->tail);
    cr_assert_eq(q->len, 0);

    posqueue_free(q);
}

Test(posqueue_remback, remove_and_verify_structure) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(1, 1);
    pos p2 = make_pos(2, 2);

    pos_enqueue(q, p1);
    pos_enqueue(q, p2);

    pos removed = posqueue_remback(q);
    cr_assert_eq(removed.r, 2);
    cr_assert_eq(removed.c, 2);
    cr_assert_eq(q->tail->p.r, 1);
    cr_assert_eq(q->len, 1);
    cr_assert_null(q->tail->next);

    removed = posqueue_remback(q);
    cr_assert_eq(removed.r, 1);
    cr_assert_eq(removed.c, 1);
    cr_assert_null(q->head);
    cr_assert_null(q->tail);
    cr_assert_eq(q->len, 0);

    posqueue_free(q);
}

Test(posqueue_remback, enqueue_and_remove_interleaved) {
    posqueue *q = posqueue_new();
    pos p1 = make_pos(1, 1);
    pos p2 = make_pos(2, 2);
    pos p3 = make_pos(3, 3);

    pos_enqueue(q, p1);
    pos_enqueue(q, p2);

    pos removed = posqueue_remback(q);
    cr_assert_eq(removed.r, 2);
    cr_assert_eq(removed.c, 2);

    pos_enqueue(q, p3);

    removed = posqueue_remback(q);
    cr_assert_eq(removed.r, 3);
    cr_assert_eq(removed.c, 3);

    removed = posqueue_remback(q);
    cr_assert_eq(removed.r, 1);
    cr_assert_eq(removed.c, 1);

    cr_assert_null(q->head);
    cr_assert_null(q->tail);
    cr_assert_eq(q->len, 0);

    posqueue_free(q);
}

/* Tests for board.c */

/** board_new **/
Test(board_new, valid_board_creation) {
    board *b = board_new(5, 5, BITS);
    cr_assert_not_null(b);
    cr_assert_eq(b->width, 5);
    cr_assert_eq(b->height, 5);
    cr_assert_eq(b->type, BITS);
    for (unsigned int r = 0; r < 5; r++) {
        for (unsigned int c = 0; c < 5; c++) {
            cr_assert_eq(board_get(b, make_pos(r, c)), EMPTY);
        }
    }
    board_free(b);
}

/** board_free **/
Test(board_free, free_valid_board) {
    board *b = board_new(5, 5, BITS);
    board_free(b);
    cr_assert(true); // Ensure no crashes occur
}

/** board_show **/
Test(board_show, display_empty_board) {
    board *b = board_new(20, 20, BITS);
    board_show(b);
    cr_assert(true); // Ensure no crashes occur
    board_free(b);
}

/** board_get **/
Test(board_get, get_valid_cell) {
    board *b = board_new(4, 4, BITS);
    cr_assert_eq(board_get(b, make_pos(2, 2)), EMPTY);
    board_free(b);
}

/** board_set **/
Test(board_set, set_valid_cell) {
    board *b = board_new(4, 4, BITS);
    pos p = make_pos(2, 2);
    board_set(b, p, BLACK);
    cr_assert_eq(board_get(b, p), BLACK);
    board_set(b, p, WHITE);
    cr_assert_eq(board_get(b, p), WHITE);
    board_free(b);
}

/* Tests for logic.c */

/** new_game **/
Test(new_game, full_initialization_check) {
    unsigned int run = 3;
    unsigned int width = 5;
    unsigned int height = 5;

    game *g = new_game(run, width, height, BITS);

    cr_assert_not_null(g);
    cr_assert_eq(g->run, run);

    cr_assert_eq(g->player, BLACKS_TURN);

    cr_assert_not_null(g->b);
    cr_assert_eq(g->b->width, width);
    cr_assert_eq(g->b->height, height);

    cr_assert_eq(g->b->type, BITS);

    for (unsigned int r = 0; r < height; r++) {
        for (unsigned int c = 0; c < width; c++) {
            cr_assert_eq(board_get(g->b, make_pos(r, c)), EMPTY);
        }
    }

    cr_assert_not_null(g->black_queue);
    cr_assert_eq(g->black_queue->len, 0);
    cr_assert_null(g->black_queue->head);
    cr_assert_null(g->black_queue->tail);

    cr_assert_not_null(g->white_queue);
    cr_assert_eq(g->white_queue->len, 0);
    cr_assert_null(g->white_queue->head);
    cr_assert_null(g->white_queue->tail);

    game_free(g);
}

/** game_free **/
Test(game_free, valid_game_free) {
    game *g = new_game(3, 5, 5, BITS);
    game_free(g);
    cr_assert(true);
}

/** drop_piece **/
Test(drop_piece, valid_moves) {
    game *g = new_game(3, 5, 5, BITS);

    cr_assert(drop_piece(g, 0));
    cr_assert_eq(board_get(g->b, make_pos(4, 0)), BLACK);
    cr_assert_eq(g->player, WHITES_TURN);

    cr_assert(drop_piece(g, 1));
    cr_assert_eq(board_get(g->b, make_pos(4, 1)), WHITE);
    cr_assert_eq(g->player, BLACKS_TURN);

    game_free(g);
}

Test(drop_piece, queue_updates) {
    game *g = new_game(3, 5, 5, BITS);

    cr_assert(drop_piece(g, 0));
    cr_assert_eq(g->black_queue->len, 1);
    cr_assert_eq(g->white_queue->len, 0);
    cr_assert_eq(g->black_queue->head->p.r, 4);
    cr_assert_eq(g->black_queue->head->p.c, 0);

    cr_assert(drop_piece(g, 1));
    cr_assert_eq(g->black_queue->len, 1);
    cr_assert_eq(g->white_queue->len, 1);
    cr_assert_eq(g->white_queue->head->p.r, 4);
    cr_assert_eq(g->white_queue->head->p.c, 1);

    game_free(g);
}

Test(drop_piece, full_column) {
    game *g = new_game(2, 2, 2, BITS);

    cr_assert(drop_piece(g, 0));
    cr_assert(drop_piece(g, 0));
    cr_assert_not(drop_piece(g, 0));

    cr_assert_eq(g->player, BLACKS_TURN);
    cr_assert_eq(board_get(g->b, make_pos(0, 0)), WHITE);
    cr_assert_eq(board_get(g->b, make_pos(1, 0)), BLACK);

    game_free(g);
}

/** disarray **/

/* Helper to check if all board cells have been correctly updated after a move
   It takes in an array of cells to compare against */
void check_board_cells(game *g, cell* expected) {
    unsigned int i = 0, height = g->b->height, width = g->b->width;
    for (unsigned int r = 0; r < height; r++) {
        for (unsigned int c = 0; c < width; c++) {
            pos p = make_pos(r, c);
            cell actual_cell = board_get(g->b, p);
            cr_assert_eq(actual_cell, expected[i++], 
                         "Mismatch at (%u,%u): Expected %d, got %d", 
                         r, c, expected[i], actual_cell);
        }
    }
}

/* Helper to check if all positions in a queue have been correctly updated 
    after a move.
 * It takes in an array of positions to compare against as well as the 
    expected length of the queue */
void check_queue_positions(posqueue *queue, pos expected_positions[], 
                            unsigned int expected_len) {
    cr_assert_eq(queue->len, expected_len);
    pq_entry *front_entry = queue->head;
    if (front_entry) {
        cr_assert_null(front_entry->prev);
    }
    for (unsigned int i = 0; i < expected_len; i++) {
        cr_assert_not_null(front_entry);
        cr_assert_eq(front_entry->p.r, expected_positions[i].r);
        cr_assert_eq(front_entry->p.c, expected_positions[i].c);
        if (front_entry->next) {
            cr_assert_eq(front_entry, front_entry->next->prev);
        }
        front_entry = front_entry->next;
    }
    cr_assert_null(front_entry);
}

/* Helper to check if the turn has been updated correctly after a move */
void check_player_turn(game *g, turn expected_turn) {
    cr_assert_eq(g->player, expected_turn);
}

Test(disarray, test_one_cell_no_change) {
    game *g = new_game(1, 1, 1, BITS);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        BLACK,
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(0, 0),
    };
    pos expected_white_positions[] = {};
    check_queue_positions(g->black_queue, expected_black_positions, 1);
    check_queue_positions(g->white_queue, expected_white_positions, 0);
    check_player_turn(g, BLACKS_TURN);
    game_free(g);
}

Test(disarray, test_empty) {
    game *g = new_game(1, 1, 1, BITS);
    disarray(g);
    cell expected_board[] = {
        EMPTY,  
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {};
    pos expected_white_positions[] = {};
    check_queue_positions(g->black_queue, expected_black_positions, 0);
    check_queue_positions(g->white_queue, expected_white_positions, 0);
    check_player_turn(g, WHITES_TURN);
    game_free(g);
}

Test(disarray, test_two_cells_no_change) {
    game *g = new_game(1, 1, 2, BITS);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        EMPTY,  
        BLACK   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(1, 0)  
    };
    pos expected_white_positions[] = {};
    check_queue_positions(g->black_queue, expected_black_positions, 1);
    check_queue_positions(g->white_queue, expected_white_positions, 0);
    check_player_turn(g, BLACKS_TURN);
    game_free(g);
}

Test(disarray, test_two_cells_flip) {
    game *g = new_game(1, 1, 2, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        BLACK,  
        WHITE   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(0, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(1, 0)  
    };
    check_queue_positions(g->black_queue, expected_black_positions, 1);
    check_queue_positions(g->white_queue, expected_white_positions, 1);
    check_player_turn(g, WHITES_TURN);
    game_free(g);
}

Test(disarray, test_two_cells_flip_and_one_top_cell) {
    game *g = new_game(1, 1, 3, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        EMPTY,
        BLACK,  
        WHITE   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(1, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(2, 0)  
    };
    check_queue_positions(g->black_queue, expected_black_positions, 1);
    check_queue_positions(g->white_queue, expected_white_positions, 1);
    check_player_turn(g, WHITES_TURN);
    game_free(g);
}

Test(disarray, test_three_cells_flip_middle_no_change) {
    game *g = new_game(1, 1, 4, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        EMPTY, 
        BLACK,
        WHITE,  
        BLACK   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(1, 0), make_pos(3, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(2, 0)  
    };
    check_queue_positions(g->black_queue, expected_black_positions, 2);
    check_queue_positions(g->white_queue, expected_white_positions, 1);
    check_player_turn(g, BLACKS_TURN);
    game_free(g);
}

Test(disarray, test_three_cells_flip_more_top_cells) {
    game *g = new_game(1, 1, 5, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        EMPTY,
        EMPTY, 
        BLACK,
        WHITE,  
        BLACK   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(2, 0), make_pos(4, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(3, 0)  
    };
    check_queue_positions(g->black_queue, expected_black_positions, 2);
    check_queue_positions(g->white_queue, expected_white_positions, 1);
    check_player_turn(g, BLACKS_TURN);
    game_free(g);
}

Test(disarray, test_three_cells_flip_half_height) {
    game *g = new_game(1, 1, 6, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        EMPTY, 
        EMPTY,
        EMPTY, 
        BLACK,
        WHITE,  
        BLACK   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(3, 0), make_pos(5, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(4, 0)  
    };
    check_queue_positions(g->black_queue, expected_black_positions, 2);
    check_queue_positions(g->white_queue, expected_white_positions, 1);
    check_player_turn(g, BLACKS_TURN);
    game_free(g);
}

Test(disarray, test_four_cells_flip) {
    game *g = new_game(1, 1, 4, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        BLACK,  
        WHITE, 
        BLACK,
        WHITE   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(0, 0), make_pos(2, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(1, 0), make_pos(3, 0)  
    };
    check_queue_positions(g->black_queue, expected_black_positions, 2);
    check_queue_positions(g->white_queue, expected_white_positions, 2);
    check_player_turn(g, WHITES_TURN);
    game_free(g);
}

Test(disarray, test_four_cells_flip_one_top) {
    game *g = new_game(1, 1, 5, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        EMPTY, 
        BLACK,  
        WHITE, 
        BLACK,
        WHITE   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(1, 0), make_pos(3, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(2, 0), make_pos(4, 0)  
    };
    check_queue_positions(g->black_queue, expected_black_positions, 2);
    check_queue_positions(g->white_queue, expected_white_positions, 2);
    check_player_turn(g, WHITES_TURN);
    game_free(g);
}

Test(disarray, test_four_cells_flip_more_top_cells) {
    game *g = new_game(1, 1, 10, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        EMPTY,
        EMPTY,
        EMPTY,
        EMPTY,
        EMPTY,
        EMPTY, 
        BLACK,  
        WHITE, 
        BLACK,
        WHITE   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(6, 0), make_pos(8, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(7, 0), make_pos(9, 0)  
    };
    check_queue_positions(g->black_queue, expected_black_positions, 2);
    check_queue_positions(g->white_queue, expected_white_positions, 2);
    check_player_turn(g, WHITES_TURN);
    game_free(g);
}

Test(disarray, test_two_columns) {
    game *g = new_game(1, 2, 2, BITS);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        BLACK, EMPTY,
        BLACK, WHITE
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(0, 0), make_pos(1, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(1, 1)
    };
    check_queue_positions(g->black_queue, expected_black_positions, 2);
    check_queue_positions(g->white_queue, expected_white_positions, 1);
    check_player_turn(g, BLACKS_TURN);
    game_free(g);
}

Test(disarray, test_two_columns_2) {
    game *g = new_game(1, 2, 3, BITS);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    disarray(g);
    cell expected_board[] = {
        BLACK, EMPTY,
        BLACK, WHITE,
        BLACK, WHITE
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(0, 0), make_pos(1, 0), make_pos(2, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(1, 1), make_pos(2, 1)
    };
    check_queue_positions(g->black_queue, expected_black_positions, 3);
    check_queue_positions(g->white_queue, expected_white_positions, 2);
    check_player_turn(g, BLACKS_TURN);
    game_free(g);
}

/** offset **/
Test(offset, test_empty_queue) {
    game *g = new_game(1, 1, 1, BITS);
    drop_piece(g, 0);
    cr_assert(!offset(g));
    game_free(g);
}

Test(offset, test_first_two_drops) {
    game *g = new_game(1, 2, 1, BITS);
    drop_piece(g, 0);
    drop_piece(g, 1);
    offset(g);
    cell expected_board[] = {
        EMPTY, EMPTY
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {};
    pos expected_white_positions[] = {};
    check_queue_positions(g->black_queue, expected_black_positions, 0);
    check_queue_positions(g->white_queue, expected_white_positions, 0);
    check_player_turn(g, WHITES_TURN);
    game_free(g);
}

Test(offset, test_one_piece_left_after_offset) {
    game *g = new_game(1, 3, 1, BITS);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 2);
    offset(g);
    cell expected_board[] = {
        BLACK, EMPTY, EMPTY
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(0, 0)  
    };
    pos expected_white_positions[] = {};
    check_queue_positions(g->black_queue, expected_black_positions, 1);
    check_queue_positions(g->white_queue, expected_white_positions, 0);
    check_player_turn(g, BLACKS_TURN);
    game_free(g);
}

Test(offset, test_no_pieces_left) {
    game *g = new_game(1, 1, 2, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    offset(g);
    cell expected_board[] = {
        EMPTY, 
        EMPTY,  
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {};
    pos expected_white_positions[] = {};
    check_queue_positions(g->black_queue, expected_black_positions, 0);
    check_queue_positions(g->white_queue, expected_white_positions, 0);
    check_player_turn(g, WHITES_TURN);
    game_free(g);
}

Test(offset, test_piece_drops_after_offset) {
    game *g = new_game(1, 1, 3, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    offset(g);
    cell expected_board[] = {
        EMPTY,
        EMPTY, 
        BLACK   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(2, 0)  
    };
    pos expected_white_positions[] = {};
    check_queue_positions(g->black_queue, expected_black_positions, 1);
    check_queue_positions(g->white_queue, expected_white_positions, 0);
    check_player_turn(g, BLACKS_TURN);
    game_free(g);
}

Test(offset, test_top_and_bottom_removed) {
    game *g = new_game(1, 1, 4, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    offset(g);
    cell expected_board[] = {
        EMPTY, 
        EMPTY,
        BLACK, 
        WHITE   
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(2, 0)  
    };
    pos expected_white_positions[] = {
        make_pos(3, 0)  
    };
    check_queue_positions(g->black_queue, expected_black_positions, 1);
    check_queue_positions(g->white_queue, expected_white_positions, 1);
    check_player_turn(g, WHITES_TURN);
    game_free(g);
}

Test(offset, test_two_columns) {
    game *g = new_game(1, 2, 4, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 0);
    offset(g);
    cell expected_board[] = {
        EMPTY, EMPTY, 
        EMPTY, EMPTY, 
        WHITE, EMPTY, 
        BLACK, BLACK 
    };
    check_board_cells(g, expected_board);
    pos expected_black_positions[] = {
        make_pos(3, 0), make_pos(3 ,1)
    };
    pos expected_white_positions[] = {
        make_pos(2, 0)  
    };
    check_queue_positions(g->black_queue, expected_black_positions, 2);
    check_queue_positions(g->white_queue, expected_white_positions, 1);
    check_player_turn(g, BLACKS_TURN);
    game_free(g);
}

/** game_outcome **/
Test(game_outcome, test_black_win) {
    game *g = new_game(1, 1, 1, BITS);
    drop_piece(g, 0);
    cr_assert(game_outcome(g) == BLACK_WIN);
    game_free(g);
}

Test(game_outcome, test_white_win_couple_moves) {
    game *g = new_game(2, 3, 3, BITS);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 0);
    cr_assert(game_outcome(g) == WHITE_WIN);
    game_free(g);
}

Test(game_outcome, test_in_progress) {
    game *g = new_game(2, 3, 3, BITS);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 1);
    cr_assert(game_outcome(g) == IN_PROGRESS);
    game_free(g);
}

Test(game_outcome, test_no_more_moves) {
    game *g = new_game(3, 3, 2, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 1);
    drop_piece(g, 2);
    cr_assert(game_outcome(g) == DRAW); //no more available moves
    game_free(g);
}

Test(game_outcome, test_draw_after_offset) {
    game *g = new_game(3, 3, 3, BITS);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 2);
    offset(g);
    cr_assert(game_outcome(g) == DRAW); //both win
    game_free(g);
}

