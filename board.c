#include "board.h"

board* board_new(unsigned int width, unsigned int height, enum type type) {
    if (width == 0 || height == 0) {
        fprintf(stderr, "Game is unplayable\n");
        exit(1);
    }
    board* b = (board*)malloc(sizeof(board));
    check_malloc(b);
    if (type == MATRIX) {
        cell** matrix = (cell**)malloc(sizeof(cell*) * height); 
        check_malloc(matrix);
        for (unsigned int r = 0; r < height; r++) {
            cell* row = (cell*)malloc(sizeof(cell) * width);
            check_malloc(row);
            matrix[r] = row;
        }
        for (unsigned int r = 0; r < height; r++) {
            for (unsigned int c = 0; c < width; c++) {
                matrix[r][c] = EMPTY;
            }
        }
        b->type = MATRIX;
        b->u.matrix = matrix;
    } else {
        unsigned int len_array, bits_needed = width * height * 2; 
        if ((bits_needed % 32) != 0) {
            len_array = (bits_needed / 32) + 1;
        } else {
            len_array = bits_needed / 32;
        }
        unsigned int* a = (unsigned int*)malloc(sizeof(unsigned int) * 
                            len_array);
        check_malloc(a);
        for (unsigned int i = 0; i < len_array; i++) {
            a[i] = 0;
        }
        b->type = BITS;
        b->u.bits = a;
    }
    b->width = width;
    b->height = height;
    return b;
}

void board_free(board* b) {
    if (b->type == MATRIX) {
        unsigned int height = b->height;
        cell** matrix = b->u.matrix;
        for (unsigned int r = 0; r < height; r++) {
            free(matrix[r]);
        }
        free(matrix);
    } else {
        free(b->u.bits);
    }
    free(b);
}

/* This helper function takes in an index i and prints out the corresponding 
   character or digit.
   It prints out:   digits if i < 10, 
                    capital letters if 10 <= i < 36, 
                    lower-case letters if 36 <= i < 62,
                    ? for any greater size to indicate that we have run out 
                    of labels */
void print_from_ind(unsigned int i) {
    if (i < 10) {
        printf("%d", i);
    } else if (i >= 10 && i < 36) {
        printf("%c", 'A' + (i - 10));
    } else if (i >= 36 && i < 62) {
        printf("%c", 'a' + (i - 36));
    } else {
        printf("?");
    }
}

void board_show(board* b) {
    check_null_pointer(b);
    printf("\n");
    printf("  ");
    unsigned int width = b->width;
    for (unsigned int c = 0; c < width; c++) {
        print_from_ind(c);
        printf(" ");
    }
    printf("\n");
    unsigned int height = b->height;
    cell ce; 
    for (unsigned int r = 0; r < height; r++) {
        print_from_ind(r);
        printf(" ");
        for (unsigned int c = 0; c < width; c++) {
            ce = board_get(b, make_pos(r, c));
            if (ce == EMPTY) {
                printf(".");
            } else if (ce == BLACK) {
                printf("*");
            } else if (ce == WHITE) {
                printf("o");
            }
            printf(" ");
        }
        printf("\n"); 
    }
    printf("\n");
}

/* This helper function takes in a board and a position and raises an error 
   if the position is outside the board */
void check_out_of_bounds_indexing(board* b, pos p) {
    unsigned int r = p.r, c = p.c, width = b->width, height = b->height;
    if (r < 0 || r >= height || c < 0 || c >= width) {
        fprintf(stderr, "Out-of-bounds indexing\n");
        exit(1);
    }
}

cell board_get(board* b, pos p) {
    check_null_pointer(b);
    check_out_of_bounds_indexing(b, p);
    if (b->type == MATRIX) {
        return b->u.matrix[p.r][p.c];
    } else {
        unsigned int glob_rank_pair = (p.c + 1) + (p.r * b->width) - 1;
        unsigned int i = glob_rank_pair / 16;
        unsigned char loc_rank_pair = glob_rank_pair % 16;
        unsigned int cell_val = (b->u.bits[i] >> (loc_rank_pair * 2)) & 0x3;
        switch (cell_val) {
            case 0:
                return EMPTY;
            case 1:
                return BLACK;
            default:
                return WHITE;
        }
    }
}

void board_set(board* b, pos p, cell c) {
    check_null_pointer(b);
    if (b->type == MATRIX) {
        check_out_of_bounds_indexing(b, p);
        b->u.matrix[p.r][p.c] = c;
    } else {
        unsigned int glob_rank_pair = (p.c + 1) + (p.r * b->width) - 1;
        unsigned int i = glob_rank_pair / 16;
        unsigned char loc_rank_pair = glob_rank_pair % 16;
        unsigned int* a = b->u.bits;
        a[i] &= 0xFFFFFFFF ^ (0x3 << (loc_rank_pair * 2));
        if (c == BLACK) {
            a[i] |= 0x1 << (loc_rank_pair * 2);
        } else if (c == WHITE) {
            a[i] |= 0x2 << (loc_rank_pair * 2);
        }
    }
}

