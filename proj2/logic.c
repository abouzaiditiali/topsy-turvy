#include <pthread.h>
#include "logic.h"

game* new_game(unsigned int run, unsigned int width,
               unsigned int height, enum type type) {
    if (run > height && run > width) {
        fprintf(stderr, "Game is impractical\n");
        exit(1);
    }
    game* g = (game*)malloc(sizeof(game));
    check_malloc(g);
    g->black_queue = posqueue_new();
    g->white_queue = posqueue_new();
    g->run = run;
    g->b = board_new(width, height, type);
    g->player = BLACKS_TURN;
    return g;
}

void game_free(game* g) {
    posqueue_free(g->black_queue); 
    posqueue_free(g->white_queue);
    board_free(g->b);
    free(g);
}

bool drop_piece(game* g, unsigned int column) {
    check_null_pointer(g);
    cell top_cell = board_get(g->b, make_pos(0, column));  
    if (top_cell != EMPTY) {
        return false;
    }
    for (unsigned int r = g->b->height - 1; r >= 0; r--) {
        pos curr_p = make_pos(r, column);
        cell curr_cell = board_get(g->b, curr_p);
        if (curr_cell == EMPTY) {
            cell cell_to_drop;
            if (g->player == BLACKS_TURN) {
                cell_to_drop = BLACK;
                pos_enqueue(g->black_queue, curr_p);
                g->player = WHITES_TURN;
            } else {
                cell_to_drop = WHITE;
                pos_enqueue(g->white_queue, curr_p);
                g->player = BLACKS_TURN;
            }
            board_set(g->b, curr_p, cell_to_drop);
            break;
        }
    }
    return true;
}

/* This helper function takes in a board and two positions on the board 
    and swaps them by updating the board */
void swap_pos(board* b, pos p1, pos p2) {
    cell cell_1 = board_get(b, p1), cell_2 = board_get(b, p2);
    board_set(b, p1, cell_2);
    board_set(b, p2, cell_1);
}

/* This helper function updates the positions in a queue after a disarray move.
 * It takes in the head of a queue, an array which indicates the drop for each 
    column, and board height.
 * It updates each position in the queue with the new position calculated using 
    the height and the drop */
void update_queue_after_disarray(pq_entry* head, unsigned int* drop_per_col, 
                                                        unsigned int height) {
    while (head) {
        unsigned int curr_r = head->p.r, curr_c = head->p.c;
        head->p.r = height - 1 - curr_r + drop_per_col[curr_c];
        head = head->next;
    }
}

/* This helper function updates the turn in a game by checking the current turn 
    and setting it to the other */
void update_turn(game* g) {
    if (g->player == BLACKS_TURN) {
        g->player = WHITES_TURN;
    } else {
        g->player = BLACKS_TURN;
    }
}

/* This is a helper function to disarray. It is used for both matrix and bits 
   representations. It takes in a pointer to a board, a column, and a pointer 
   to an element in the drop_count. It only iterates over a single column 
   (specified in the parameter) and updates the drop_count out_parameter 
   correspondingly */
void process_column(board* b, unsigned int column, unsigned int* drop_count) {
    unsigned int height = b->height;
    unsigned int bottom_r = height - 1;
    *drop_count = 0; 
    for (unsigned int r = 0; r < height; r++) {
        if (bottom_r <= r) {
            break;
        }
        pos p_top = make_pos(r, column);
        if (board_get(b, p_top) == EMPTY) {
            (*drop_count)++;
            continue;
        }
        pos p_bottom = make_pos(bottom_r, column);
        swap_pos(b, p_top, p_bottom);
        bottom_r--;
    }
}
 
/* This is a thread routine for processing a single column.
 * It casts the argument to a pointer of type t_args, where t_args is
   a structure that holds the game state, column index, and the drop_per_col
   array used for gravity calculations.
 * It returns NULL to indicate that the thread has completed its work */
void* process_column_routine(void* arg) {
    t_args* args = (t_args*)arg;
    process_column(args->g->b, 
                   args->column, 
                   &args->drop_per_col[args->column]);
    return NULL;
}

void disarray(game* g) {
    check_null_pointer(g);
    unsigned int height = g->b->height, width = g->b->width, 
                 drop_per_col[width];
    if (g->b->type == MATRIX) {
        pthread_t threads[width];
        t_args args[width];
        for (unsigned int c = 0; c < width; c++) {
            args[c].g = g;
            args[c].column = c;
            args[c].drop_per_col = drop_per_col;
            pthread_create(&threads[c], 
                           NULL, 
                           process_column_routine, 
                           &args[c]);
        }
        for (unsigned int c = 0; c < width; c++) {
            pthread_join(threads[c], NULL);
        }
    } else {
        for (unsigned int c = 0; c < width; c++) {
            process_column(g->b, c, &drop_per_col[c]);
        }
    }
    update_queue_after_disarray(g->black_queue->head, drop_per_col, height);
    update_queue_after_disarray(g->white_queue->head, drop_per_col, height);
    update_turn(g);
}

/* This helper function takes in a board and updates it after an offset move
 * It only updates the pieces that were on top of the piece removed by offset
 * For this reason, the function takes in the position of the removed piece 
    through removed_piece_r and removed_piece_c.
 * This function specifically handles the case where the current player's 
    oldest piece and the opponent's latest piece are in different columns, so 
    that all pieces on top of the removed piece have to be dropped by 1 */
void drop_by_one(board* b, unsigned int removed_piece_r,
                           unsigned int removed_piece_c) {
    for (int r = removed_piece_r - 1; r >= 0; r--) {
        pos curr_p = make_pos(r, removed_piece_c);
        cell curr_cell = board_get(b, curr_p);
        if (curr_cell == EMPTY) {
            break;
        }
        pos p_below = make_pos(curr_p.r + 1, curr_p.c);
        board_set(b, p_below, curr_cell);
        board_set(b, curr_p, EMPTY);
    }
}

/* This helper function updates all positions in a queue after an offset move.
 * It takes in the head to a queue, as well as the oldest and most recent 
    pieces' positions, respectively oldest_pos and latest_pos.
 * It handles both the case when the pieces removed are in the same column and 
    when they are not. To handle the former, it additionally takes in two 
    other parameters which correspond to the row indices of the two removed 
    pieces' positions with the higher value r being bottom_r (called bottom 
    due to how it is perceived when looking at a board) and the lower value r 
    being top_r.
 * All positions are simply dropped by 1 when both removed positions are not 
    in the same column.
 * When both removed positions are in the same column, positions are dropped 
    by 1 or 2 depending on where the current r situates relative to bottom_r 
    and top_r */
void update_queue_after_offset(pq_entry* head, pos latest_pos, pos oldest_pos, 
                               unsigned int bottom_r, unsigned int top_r) { 
    while (head) {
        unsigned int curr_r = head->p.r, curr_c = head->p.c;
        unsigned int lat_r = latest_pos.r, lat_c = latest_pos.c;
        unsigned int old_r = oldest_pos.r, old_c = oldest_pos.c;
        if (curr_c == lat_c && curr_c == old_c) {
            if (curr_r < bottom_r && curr_r > top_r) {
                head->p.r += 1;
            } else if (curr_r < top_r) {
                head->p.r += 2;
            }
        } else if (curr_c == lat_c) {
            if (curr_r < lat_r) {
                head->p.r += 1;
            }
        } else if (curr_c == old_c) {
            if (curr_r < old_r) {
                head->p.r += 1;
            }
        }
        head = head->next;
    }
}

/* This helper function updates the board after an offset move.
 * It uses the same concept of bottom_r and top_r found in the previous helper 
    function (please refer to previous helper definition for 
    update_queue_after_offset).
 * It specifically handles the case when both removed pieces are in the same 
    column, which is provided through the parameter col */
void drop_by_one_or_two(board* b, unsigned int col, unsigned int bottom_r, 
                                                    unsigned int top_r) {
    unsigned int jump_to_last_empty_r = 1;
    for (int r = bottom_r - 1; r >= 0; r--) {
        pos curr_p = make_pos(r, col);
        pos p_below = make_pos(r + 1, col);
        cell curr_cell = board_get(b, curr_p);
        cell cell_below = board_get(b, p_below);
        if (curr_cell == EMPTY && cell_below == EMPTY) {
            if (jump_to_last_empty_r == 2) {
                break;
            }
            jump_to_last_empty_r = 2;
            continue;
        }
        pos p_last_empty = make_pos(r + jump_to_last_empty_r, col);
        board_set(b, p_last_empty, curr_cell);
        board_set(b, curr_p, EMPTY);
    }
}

bool offset(game* g) {
    check_null_pointer(g);
    if (g->black_queue->head == NULL || g->white_queue->head == NULL) {
        return false;
    }
    pos latest_pos, oldest_pos;
    if (g->player == BLACKS_TURN) {
        latest_pos = posqueue_remback(g->white_queue);
        oldest_pos = pos_dequeue(g->black_queue);
    } else {
        latest_pos = posqueue_remback(g->black_queue);
        oldest_pos = pos_dequeue(g->white_queue);
    }
    board_set(g->b, latest_pos, EMPTY);
    board_set(g->b, oldest_pos, EMPTY);
    unsigned int bottom_r = 0, top_r = 0;
    unsigned int lat_r = latest_pos.r, lat_c = latest_pos.c; 
    unsigned int old_r = oldest_pos.r, old_c = oldest_pos.c;
    if (lat_c != old_c) {
        drop_by_one(g->b, lat_r, lat_c);
        drop_by_one(g->b, old_r, old_c);
    } else {
        if (lat_r < old_r) {
            bottom_r = old_r;
            top_r = lat_r;
        } else {
            bottom_r = lat_r;
            top_r = old_r;
        }
        drop_by_one_or_two(g->b, lat_c, bottom_r, top_r);
    }
    update_queue_after_offset(g->black_queue->head, latest_pos, oldest_pos, 
                                                    bottom_r, top_r);
    update_queue_after_offset(g->white_queue->head, latest_pos, oldest_pos, 
                                                    bottom_r, top_r);
    update_turn(g);
    return true;
}

/* This helper function checks if a specific trajectory contains a run.
 * It takes in an array of two integers from -1 to 1 (inclusive) which 
    determine the transformation to make. The first element corresponds to how 
    the row index should be transformed and the second is for the column.
 * It also takes in the position of the first piece to start checking from */
bool is_run(game* g, char* transformation, pos start_p) { 
    char trans_r = transformation[0], trans_c = transformation[1];
    unsigned int run = g->run, curr_r = start_p.r + trans_r, 
                               curr_c = start_p.c + trans_c;
    cell expected = board_get(g->b, make_pos(start_p.r, start_p.c));
    for (unsigned int i = 0; i < run - 1; i++) {
        cell curr_cell = board_get(g->b, make_pos(curr_r, curr_c));
        if (curr_cell != expected) {
            return false;
        }
        curr_r += trans_r;
        curr_c += trans_c;
    }
    return true;
}

/* This helper function loops over a queue and checks if a run is found.
 * If it is found, the out_parameter out_run is updated to true.
 * It takes in a pointer to the queue that needs to be checked.
 * To optimize the search, a transformation of only four directions is used:
    (-1, -1) for north west, (-1, 0) for north, (-1, 1) for north east, 
    (0, 1) for east. These four transformations cover every possible vertical 
    line, horizontal line, and diagonal line on a board.
 * To further optimize the search, if the current row index of the piece that 
    is being checked is at a distance from any of the edges of the board that 
    is strictly less than the run, then the transformation for that specific 
    direction is skipped */
void check_run(game* g, pq_entry* head, bool* out_run) {
    unsigned int run = g->run, width = g->b->width;
    while (head) {
        pos curr_p = make_pos(head->p.r, head->p.c);
        char transformations[4][2];
        unsigned char len = 0;
        if (run <= (curr_p.r + 1)) {
            transformations[len][0] = -1; //north
            transformations[len++][1] = 0; 
            if (run <= (curr_p.c + 1)) {
                transformations[len][0] = -1; //north west
                transformations[len++][1] = -1; 
            }
            if (run <= (width - curr_p.c)) {
                transformations[len][0] = -1; //north east
                transformations[len++][1] = 1;
            }
        }
        if (run <= (width - curr_p.c)) {
            transformations[len][0] = 0; //east
            transformations[len++][1] = 1;
        }
        for (unsigned char i = 0; i < len; i++) {
            if (is_run(g, transformations[i], curr_p)) {
                *out_run = true;
                break;
            }
        }
        if (*out_run) {
            break;
        }
        head = head->next;
    }
}

/* This helper function loops over all the top cells (cells of row index equal 
    to 0) and returns true if one of them is empty to indicate that there is 
    still an available move. If all top cells are not empty, the function 
    returns false, indicating that the board is full and that no moves are 
    available */
bool available_move(game* g) {
    unsigned int width = g->b->width;
    for (unsigned int c = 0; c < width; c++) {
        cell curr_cell = board_get(g->b, make_pos(0, c));
        if (curr_cell == EMPTY) {
            return true;
        }
    }
    return false;
}

outcome game_outcome(game* g) {
    check_null_pointer(g);
    pq_entry *head_bl = g->black_queue->head, *head_wh = g->white_queue->head;
    bool black_run = false, white_run = false;
    check_run(g, head_bl, &black_run);
    check_run(g, head_wh, &white_run);
    if (black_run && white_run) {
        return DRAW;
    } else if (black_run) {
        return BLACK_WIN;
    } else if (white_run) {
        return WHITE_WIN;
    } else if (!available_move(g)) {
        return DRAW;
    } else {
        return IN_PROGRESS;
    }
}

