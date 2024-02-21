// ---------------------------------------------------------------------------
// application: minesweeper
// file: Huy_Nguyen.c
// Author: Huy Nguyen
// Date: 10/12/2023
// ---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Constants
#define MAXTOKENCOUNT 10
#define MAXTOKENLENGTH 80
#define MAXLINELENGTH 80

// Structure for a cell
struct cell {
    //int position; // value used to show location of cell within its 2-D board
    int adjcount; // value from 0-8 indicating how many adjacent cells are mines
    int mined; // 0/1 to indicate if this cell contains a mine (1) or not (0)
    int covered; // 0/1 to indicate if this cell is currently covered (1) or not (0)
    int flagged; // 0/1 to indicate if this cell is currently flagged (1) or not (0)
};
typedef struct cell cell;

// Function prototypes

// Global variables
cell** board; // 2-D array declared as type "pointer to pointer to cell"
int rows;
int cols;
int r;
int c;
int mines;
int result;

// set neighbor cells around the mined
int rowneighbors[] = { -1,-1,+0,+1,+1,+1,+0,-1 };
int colneighbors[] = { +0,+1,+1,+1,+0,-1,-1,-1 };
// ---------------------------------------------------------------------------
// generate random number from 0 to (range-1)
int get_random(int range) {
    return ((int)floor((float)range * rand() / RAND_MAX)) % range;
}

// create new table
void command_new() {
    board = (cell**)malloc(sizeof(cell*) * rows);

    for (int i = 0; i < rows; i++) {
        board[i] = (cell*)malloc(sizeof(cell) * cols);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            //board[i][j].position = i * cols + j;

            // set the whole board to coverd and not flagged
            board[i][j].covered = 1;
            board[i][j].flagged = 0;

            board[i][j].mined = 0;
        }
    }

    // set mines random in the board
    for (int m = 0; m < mines; m++) {
        r = get_random(rows);
        c = get_random(cols);

        // if the location r,c is mined already then generate a new location
        while (board[r][c].mined == 1) {
            r = get_random(rows);
            c = get_random(cols);
        }

        // set mine for location r,c
        board[r][c].mined = 1;
    }

    // set minecount for the rest base on location of mines
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int minecount = 0;
            for (int d = 0; d < 8; d++) {
                int rn = r + rowneighbors[d];
                int cn = c + colneighbors[d];

                if (0 <= rn && rn < rows && 0 <= cn && cn < cols) {
                    if (board[rn][cn].mined == 1)
                        minecount++;
                }
            }
            board[r][c].adjcount = minecount;
        }
    }
}

// ----------------------------------------------------------------------SHOW

// display the data from a cell
void display_cell(cell* c) {
    // if c is flagged then print P
    if (c->flagged == 1)
        printf("%2s", "F");
    // if c is covered then print /
    else if (c->covered == 1)
        printf("%2s", "/");
    // if there is a mine then print *
    else if (c->mined == 1)
        printf("%2s", "*");
    // if there is not a number then print .
    else if (c->adjcount == 0)
        printf("%2s", ".");
    // print the number
    else
        printf("%2d", c->adjcount);
}

// display the board on the screen
void command_show() {
    // print # for columns
    printf("---");
    for (int j = 0; j < cols; j++) {
        printf("%2d ", j + 1);
    }
    printf("\n");
    for (int j = 0; j < cols*3+3; j++) {
        printf("-");
    }
    printf("\n");

    for (int i = 0; i < rows; i++) {
        // print # for rows
        printf("%2d|", i + 1);
        for (int j = 0; j < cols; j++) {

            display_cell(&board[i][j]);
            printf(" ");
        }
        printf("\n");
    }

    printf("\n\n");
}

// ---------------------------------------------------------------------------WIN

// check if player win the game (1)
int check_win() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (board[i][j].mined == 1 && board[i][j].flagged == 0)
                return 0;
            else if (board[i][j].mined == 0 && board[i][j].covered == 1)
                return 0;
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------LOSE

// check if player lose the game (1)
int check_lose() {

    if (board[r][c].mined == 1)
        return 1;
    else
        return 0;
}

// ---------------------------------------------------------------------------

// cover the whole board
void coverall() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            board[i][j].covered = 1;
        }
    }
}

// uncover the whole board
void uncoverall() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            board[i][j].covered = 0;
        }
    }
}

// ---------------------------------------------------------------------------COVER

// recursive function to uncover the cell and the cells around
void uncover_recursive(int r, int c) {
    board[r][c].covered = 0;
    // remove the flag from that cell
    if (board[r][c].flagged == 1)
        board[r][c].flagged = 0;

    if (board[r][c].adjcount == 0) {
        // for each neighbor n of d
        for (int d = 0; d < 8; d++) {
            int rn = r + rowneighbors[d];
            int cn = c + colneighbors[d];

            if (0 <= rn && rn < rows && 0 <= cn && cn < cols) {
                if (board[rn][cn].covered == 1)
                    uncover_recursive(rn, cn);
            }
        }
    }
}

// uncover the cell at specific location
void command_uncover() {
    uncover_recursive(r, c);
    command_show();

    check_lose();
    check_win();
}

// ---------------------------------------------------------------------------FLAG

// flag the cell if player thinks there is a mine
void command_flag() {
    if (board[r][c].flagged == 0)
        board[r][c].flagged = 1;
    else
        printf("cell already flagged\n");

    check_win();
}

// unflag the cell
void command_unflag() {
    if (board[r][c].flagged == 1)
        board[r][c].flagged = 0;
    else
        printf("cell already unflagged\n");
}

// ---------------------------------------------------------------------------

// show menu
void menu() {
    printf("=================================\n");
    printf("          COMMAND\n");
    printf("new [#rows] [#columns] [#mines]\n");
    printf("show: display current board\n");
    printf("quit: quit the app\n");
    printf("uncover [row] [column]\n");
    printf("flag [row] [column]\n");
    printf("unflag [row] [column]\n");
    printf("=================================\n");
}
// ---------------------------------------------------------------------------
int rungame() {
    char line[MAXLINELENGTH], linecopy[MAXLINELENGTH];
    char tokens[MAXTOKENCOUNT][MAXTOKENLENGTH];
    int tokencount;
    char* p;

    menu();
    while (1) {
        printf(">> ");

        //getline(line, MAXLINELENGTH);
        fgets(line, MAXLINELENGTH, stdin);
        int linelen = strlen(line);
        if (line[linelen - 1] == '\n')
            line[linelen - 1] = '\0';

        //gettokens(line, tokens, &tokencount);
        strcpy(linecopy, line);
        p = strtok(linecopy, " ");
        tokencount = 0;

        while (p != NULL) {
            strcpy(tokens[tokencount], p);
            p = strtok(NULL, " ");
            tokencount++;
        }

        // if then commands
        // QUIT
        if (strcmp(tokens[0], "quit") == 0) {
            printf("Bye!!!\n");
            break;
        }

        // NEW
        else if (strcmp(tokens[0], "new") == 0) {
            rows = atoi(tokens[1]);
            cols = atoi(tokens[2]);
            mines = atoi(tokens[3]);

            command_new();
            command_show();
        }

        // SHOW
        else if (strcmp(tokens[0], "show") == 0) {
            command_show();
        }

        // COVERALL
        else if (strcmp(tokens[0], "coverall") == 0) {
            coverall();
            command_show();
        }

        // UNCOVERALL
        else if (strcmp(tokens[0], "uncoverall") == 0) {
            uncoverall();
            command_show();
        }

        // UNCOVER
        else if (strcmp(tokens[0], "uncover") == 0) {
            r = atoi(tokens[1]) - 1;
            c = atoi(tokens[2]) - 1;

            command_uncover();

            // print Lost
            if (check_lose() == 1) {
                printf("You uncovered a mine\n\n");
                printf("=====================\n");
                printf("----  GAME OVER  ----\n");
                printf("=====================\n");
                break;
            }
        }

        // FLAG
        else if (strcmp(tokens[0], "flag") == 0) {
            r = atoi(tokens[1]) - 1;
            c = atoi(tokens[2]) - 1;

            command_flag();
        }

        // UNFLAG
        else if (strcmp(tokens[0], "unflag") == 0) {
            r = atoi(tokens[1]) - 1;
            c = atoi(tokens[2]) - 1;

            command_unflag();
        }

        if (check_win() == 1) {
            command_show();
            printf("=====================\n");
            printf("-----  YOU WON  -----\n");
            printf("=====================\n");
            break;
        }
    }
    return 0;
}
// ---------------------------------------------------------------------------
int main(void) {
    rungame();
    return 0;
}