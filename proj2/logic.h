#ifndef LOGIC_H
#define LOGIC_H

#include <stdbool.h>
#include "board.h"

enum turn {
    BLACKS_TURN,
    WHITES_TURN
};

typedef enum turn turn;


enum outcome {
    IN_PROGRESS,
    BLACK_WIN,
    WHITE_WIN,
    DRAW
};

typedef enum outcome outcome;


struct game {
    unsigned int run;
    board* b;
    posqueue *black_queue, *white_queue;
    turn player;
};

typedef struct game game;


struct disarray_thread_args {
    game* g;
    unsigned int column;
    unsigned int* drop_per_col;
};

typedef struct disarray_thread_args t_args;

/**
 * new_game
 * 
 * Creates and initializes a new game with the specified configuration.
 * 
 * Parameters:
 *   - run: The number of consecutive pieces needed to win (unsigned integer).
 *   - width: The number of columns in the game board (unsigned integer).
 *   - height: The number of rows in the game board (unsigned integer).
 *   - type: The representation type of the board (enum type).
 * 
 * Returns:
 *   - A pointer to the newly created `game` structure.
 *     The game is fully initialized with an empty board and empty piece 
 *      queues.
 * 
 * Note:
 *   - The caller is responsible for freeing the game using `game_free`.
 *   - Raises an error if memory allocation fails or if the `run` value is 
 *      impractical for the board size.
 */
game* new_game(unsigned int run, unsigned int width,
               unsigned int height, enum type type);

/**
 * game_free
 * 
 * Frees all memory associated with the specified game.
 * 
 * Parameters:
 *   - g: A pointer to the `game` structure to be freed.
 * 
 * Modifies:
 *   - Deallocates the game board, piece queues, and the game structure itself.
 */
void game_free(game* g);

/**
 * drop_piece
 * 
 * Drops a piece for the current player into the specified column.
 * 
 * Parameters:
 *   - g: A pointer to the `game` structure.
 *   - column: The column index where the piece should be dropped (zero-based).
 * 
 * Returns:
 *   - `true` if the piece was successfully dropped.
 *   - `false` if the column is full
 * 
 * Modifies:
 *   - Updates the board with the dropped piece.
 *   - Advances the turn to the next player if the move is valid.
 *   - Updates the piece queue for the current player.
 * 
 * Note:
 *   - Raises an error if the game pointer is NULL or the column index is out 
 *      of bounds.
 */
bool drop_piece(game* g, unsigned int column);

/**
 * disarray
 * 
 * Performs a disarray move, reflecting the board across the horizontal 
 * centerline, then applying gravity to each column. Specifically, for boards 
 * represented with a matrix (but not for boards represented with packed bits), 
 * it creates one thread per column in the board, each thread independently 
 * manipulating the positions of pieces in that column according to the rules 
 * of disarray.
 * 
 * Parameters:
 *   - g: A pointer to the `game` structure.
 * 
 * Modifies:
 *   - Reflects the board and repositions pieces to the bottom of their 
 *      respective columns.
 *   - Updates the coordinates in the piece queues to reflect the new 
 *      positions.
 *   - Advances the turn to the next player.
 * 
 * Note:
 *   - Raises an error if the game pointer is NULL.
 */
void disarray(game* g);

/**
 * offset
 * 
 * Executes an offset move by removing the oldest piece of the current player 
 *  and the newest piece of the opponent.
 * 
 * Parameters:
 *   - g: A pointer to the `game` structure.
 * 
 * Returns:
 *   - `true` if the offset move was performed successfully.
 *   - `false` if one or both players have no pieces on the board, making the 
 *      move invalid.
 * 
 * Modifies:
 *   - Removes the specified pieces from the board and their respective queues.
 *   - Updates the coordinates of pieces above the removed ones due to gravity.
 *   - Advances the turn to the next player.
 * 
 * Note:
 *   - Raises an error if the game pointer is NULL.
 */
bool offset(game* g);

/**
 * game_outcome
 * 
 * Determines the current outcome of the game.
 * 
 * Parameters:
 *   - g: A pointer to the `game` structure.
 * 
 * Returns:
 *   - An `outcome` enumeration value representing the game state:
 *       - `IN_PROGRESS`: The game is still ongoing; no player has achieved 
 *          the required run length.
 *       - `BLACK_WIN`: The black player has won by achieving the required run 
 *          length.
 *       - `WHITE_WIN`: The white player has won by achieving the required run 
 *          length.
 *       - `DRAW`: The game has ended in a draw due to:
 *           - The board being completely filled without a winner.
 *           - Both players achieving the required run length simultaneously 
 *              due to a `disarray` or `offset` move. 
 *
 * Note:
 *   - Raises an error if the game pointer is NULL.
 */
outcome game_outcome(game* g);


#endif /* LOGIC_H */
