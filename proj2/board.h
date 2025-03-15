#ifndef BOARD_H
#define BOARD_H

#include "pos.h"

enum cell {
    EMPTY,
    BLACK,
    WHITE
};

typedef enum cell cell;


union board_rep {
    enum cell** matrix;
    unsigned int* bits;
};

typedef union board_rep board_rep;

enum type {
    MATRIX, BITS
};


struct board {
    unsigned int width, height;
    enum type type;
    board_rep u;
};

typedef struct board board;


/**
 * board_new
 * 
 * Creates and returns a new game board with the specified dimensions and type.
 * 
 * Parameters:
 *   - width: The number of columns in the board (unsigned integer).
 *   - height: The number of rows in the board (unsigned integer).
 *   - type: The representation type of the board (enum type).
 * 
 * Returns:
 *   - A pointer to the newly created `board` structure.
 *     The board is fully initialized and all cells are set to EMPTY.
 */
board* board_new(unsigned int width, unsigned int height, enum type type);

/**
 * board_free
 * 
 * Frees all memory associated with the specified board.
 * 
 * Parameters:
 *   - b: A pointer to the `board` structure to be freed.
 * 
 * Modifies:
 *   - Deallocates all cells and internal data of the board.
 *   - Frees the board structure itself.
 */
void board_free(board* b);

/**
 * board_show
 * 
 * Displays the current state of the game board.
 * 
 * Parameters:
 *   - b: A pointer to the `board` structure to display.
 * 
 * Output:
 *   - Prints the board with row and column headers, as well as cell contents:
 *     - `.` for EMPTY cells
 *     - `*` for BLACK pieces
 *     - `o` for WHITE pieces
 * 
 * Note:
 *   - Raises an error if the board pointer is NULL 
 */
void board_show(board* b);

/**
 * board_get
 * 
 * Retrieves the value of a cell at a specified position on the board.
 * 
 * Parameters:
 *   - b: A pointer to the `board` structure.
 *   - p: A `pos` structure indicating the row and column of the cell.
 * 
 * Returns:
 *   - The `cell` value at the specified position (EMPTY, BLACK, or WHITE).
 * 
 * Note:
 *   - Performs bounds-checking and raises an error if the position is out of 
 *      range.
 *   - Raises an error if the board pointer is NULL
 */
cell board_get(board* b, pos p);

/**
 * board_set
 * 
 * Sets the value of a cell at a specified position on the board.
 * 
 * Parameters:
 *   - b: A pointer to the `board` structure.
 *   - p: A `pos` structure indicating the row and column of the cell.
 *   - c: The `cell` value to set (EMPTY, BLACK, or WHITE).
 * 
 * Modifies:
 *   - Updates the specified cell on the board with the provided value.
 * 
 * Note:
 *   - Performs bounds-checking and raises an error if the position is out of 
 *      range.
 *   - Raises an error if the board pointer is NULL
 */
void board_set(board* b, pos p, cell c);

#endif /* BOARD_H */
