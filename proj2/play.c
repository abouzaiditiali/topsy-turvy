#include "logic.h"

/* This helper to the function check_arguments is given a pointer to a string 
    and checks if the string is a valid option. A valid option contains two 
    elements: a '-', followed by either h, w, r, m, or b */
bool is_valid_option(char* s) {
    unsigned char i = 0;
    while (s[i]) {
        if (i == 2) {
            return false;
        }
        i++;
    }
    if (i != 2) {
        return false;
    }
    return s[0] == '-' && (s[1] == 'h' || s[1] == 'w' || s[1] == 'r' || 
                            s[1] == 'm' || s[1] == 'b'); 
}

/* This helper to the function is_valid_nonnegative_number is given a 
    character and checks whether it is a digit or not */
bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

/* This helper to the function check_arguments is given a pointer to a string 
    and checks if the string is a valid non-negative number.
 * It checks that it is non-negative by checking that every element of the 
    string is a digit.
 * It also checks that the number is not strictly greater than the maximum 
    value an unsigned int can hold, as that is the type of height, width, and 
    run */
bool is_valid_nonnegative_number(char* s) {
    unsigned char i = 0;
    while (s[i]) {
        if (!is_digit(s[i])) {
            return false;
        }
        i++;
    }
    return true;
}

/* This function checks if all command-line arguments are valid.
 * It takes in the argument count argc, the array of strings argv, and 
    out-parameters -type, height, width, run- to update based on the what the 
    command-line arguments are.
 * The user is able to specify any valid values on the command line, and in 
    any order, as long as each option from -h -w -r is directly followed by 
    its value, and that exactly one of -b and -m is specified.
 * Widths that would result in ? column label(s) are unplayable 
 * Prints out error messages if not given all required command-line arguments, 
    or unplayable ones */
void check_arguments(int argc, char** argv, enum type* type, 
                     unsigned int* height, 
                     unsigned int* width, 
                     unsigned int* run) {
    if (argc != 8) {
        fprintf(stderr, "Invalid number of command-line arguments. "
                        "The required number is 8.\n");
        exit(1);
    }
    bool h_found = false, w_found = false, r_found = false, m_found = false, 
         b_found = false;
    for (unsigned char i = 1; i < argc; i++) {
        if (!is_valid_option(argv[i]) && 
            !is_valid_nonnegative_number(argv[i])) {
            fprintf(stderr, "Argument %hhu is neither a valid option "
                            "nor a valid number.\n", i);
            exit(1);
        }
        if (is_valid_option(argv[i])) {
            if (argv[i][1] == 'm') {
                *type = MATRIX;
                m_found = true;
                continue;
            } else if (argv[i][1] == 'b') {
                *type = BITS;
                b_found = true;
                continue;
            }
            if (i == argc - 1) {
                fprintf(stderr, "Option -h, -w, or -r cannot be the last "
                                "argument.\n");
                exit(1);
            }
            if (!is_valid_nonnegative_number(argv[i+1])) {
                fprintf(stderr, "Valid option is not followed by a valid "
                                "number.\n");
                exit(1);
            }
            switch (argv[i][1]) {
                case 'h':
                    *height = atoi(argv[i + 1]);
                    h_found = true;
                    break;
                case 'w':
                    *width = atoi(argv[i + 1]);
                    if (*width > 62) {
                        fprintf(stderr, "Invalid width. Width has to be less "
                                        "than or equal to 62 for the game to "
                                        "be playable.\n");
                        exit(1);
                    }
                    w_found = true;
                    break;
                case 'r':
                    *run = atoi(argv[i + 1]);
                    r_found = true;
                    break;
            }
        }
    }
    if (!h_found || !w_found || !r_found || (!m_found && !b_found)) {
        fprintf(stderr, "One or more options are missing.\n");
        exit(1);
    }
}

/* Prints the player whose turn it is by taking in a pointer to the game 
    state */
void print_turn(game* g) {
    if (g->player == BLACKS_TURN) {
        printf("Black: \n");
    } else {
        printf("White: \n");
    }
}

/* Helper to the function convert_char_to_num. 
 * It takes in a char and checks whether it is an uppercase letter */
bool is_uppcase(char c) {
    return c >= 'A' && c <= 'Z';
}

/* Helper to the function convert_char_to_num. 
 * It takes in a char and checks whether it is a lowercase letter */
bool is_lowcase(char c) {
    return c >= 'a' && c <= 'z';
}

/* Checks if the character input during gameplay is valid. It is valid if it 
    is either '!' for offset, '^' for disarray, a digit, uppercase, or 
    lowercase letter for a drop */
bool is_valid_character(char c) {
    return c == '!' || c == '^' || is_digit(c) || is_uppcase(c) || 
                                                  is_lowcase(c);
}

/* Converts the character input of the player during gameplay into a number 
    which will correspond to the column in which to drop. It returns this 
    column */
unsigned int convert_char_to_num(char c) {
    if (is_digit(c)) {
        return c - '0';
    } else if (is_uppcase(c)) {
        return (c - 'A') + 10;
    } else {
        return (c - 'a') + 32;
    }
}

/* Takes in the outcome and prints it */
void print_outcome(outcome o) {
    if (o == BLACK_WIN) {
        printf("Black wins.\n");
    } else if (o == WHITE_WIN) {
        printf("White wins.\n");
    } else {
        printf("It is a draw.\n");
    }
}

/* The main function implements the overall gameplay interface.
 * It takes input from the player and prints out the corresponding move on the 
    board */
int main(int argc, char** argv) {
    unsigned int height, width, run;
    enum type type;
    check_arguments(argc, argv, &type, &height, &width, &run);
    game* g = new_game(run, width, height, type);
    board_show(g->b);
    bool is_move_successful = false;
    char ch;
    while (1) {
        if (is_move_successful) {
            board_show(g->b);
        }
        print_turn(g);
        scanf("%c%*c", &ch);
        if (!is_valid_character(ch)) {
            is_move_successful = false;
            continue;
        }
        if (ch == '!') {
            if (!offset(g)) {
                is_move_successful = false;
                continue;
            }
        } else if (ch == '^') {
            disarray(g);
        } else {
            unsigned int col = convert_char_to_num(ch);
            if (col >= width || !drop_piece(g, col)) {
                is_move_successful = false;
                continue;
            }
        }
        outcome o = game_outcome(g);
        if (o != IN_PROGRESS) {
           board_show(g->b);    
           print_outcome(o);
           game_free(g);
           exit(1);
        }
        is_move_successful = true;
    }
    return 0;   
}

