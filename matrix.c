#include <ncurses.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

char rand_char()
{
    /* do not use two-byte Unicode here as this breaks the presentation,
     * e.g., "°^-/%%&@®†µ~∫‚∂ƒ@©π«≈∆@≈œµç•¿}{{|][¢¶]}}";
     */
    char matrix_chars[] = "M.:,–*m§$°^";
    return matrix_chars[rand() % strlen(matrix_chars)];
}

void run_matrix(int rows, int cols)
{
    int r, c;
    char matrix[cols][rows];

    /* empty screen */
    for (r=0; r<rows; r++)
        for (c=0; c<cols; c++) {
            matrix[c][r] = ' ';
        }
    
    while (1) {
        /* update the matrix */
        for (c=0; c<cols; c++) {
            /* randomly ignore this whole col */
            if ((rand() % 100) > 20) {
                ; /* FALLTHROUGH */
            } else {
                /* check if all cols are empty for this row */
                int empty = 1;
                for (r=0; r<rows; r++) {
                    if (matrix[c][r] != ' ')
                        empty = 0;
                }

                if (empty) {
                    if ((rand() % cols) > (cols*.9))
                        matrix[c][0] = rand_char();
                } else {
                    /* 0. Row is NOT empty, but still set a char with small prob as row
                    * could be almost empty and only fileld at bottom part */
                    if ((rand() % 100) > 95)
                        matrix[c][0] = rand_char();
                    
                    /* 1. shift down the rows in this column by random-choice */
                    int this_col = c;
                    int cur_row;

                    if ((rand() % 10) > 5) {
                        for (cur_row = rows-1; cur_row>0; cur_row--)
                            matrix[this_col][cur_row] = matrix[this_col][cur_row-1];
                        matrix[this_col][0] = ' '; /* empty the first row */
                    }

                    /* 2. add new elements to top+bottom of row */
                    int i, top = 0, bottom = 0;
                    for (i = 0; i < rows; i++) {
                        if (matrix[this_col][i] != ' ' && top == 0) {
                            top = i; bottom = i; /* once an element
                            was found top=bottom point to that element */
                        } else if (matrix[this_col][i] != ' ' && top)
                            bottom = i; /* alter bottom, if an element appears further down */
                    }

                    if (top>0)
                        if ((rand() % 10) > 7) {
                            matrix[this_col][top-1] = rand_char();
                        }
                    
                    if (bottom && bottom < (rows-2)) {
                        if ((rand() % 10) > 8) {
                            matrix[this_col][bottom+1] = rand_char();
                        }
                    }
                }
            }
        }

        /* print the new screen */
        for (c=0; c<cols; c++)
            for (r=0; r<rows; r++) {
                if (rand() % 2) {
                    mvaddch(r, c, matrix[c][r] | A_BOLD | COLOR_PAIR(1));
                } else {
                    mvaddch(r, c, matrix[c][r] | COLOR_PAIR(2) | A_BOLD);
                }
            }
    refresh();
    usleep(20000);
    /*getch();*/
    }
}

int main(void) {
    int rows, cols; /* terminal resolution */
    initscr();
    if (!has_colors()) { /* check for colors */
        fprintf(stderr, "term has no colors\n");
        endwin();
        return 1;
    }
    start_color(); /* call once after initscr() */
    getmaxyx(stdscr, rows, cols); /* a macro, so no & needed */

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    bkgd(COLOR_PAIR(1)); /* whole screen black */
    clear(); /* repaint all cells */
    refresh();
    srand(time(NULL));
    run_matrix(rows, cols);
    attroff(COLOR_PAIR(3));

    endwin();
    return 0;
}
