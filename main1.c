
#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define BOARD_WIDTH    10   
#define BOARD_HEIGHT   20    
#define CELL_SIZE      30    
#define EMPTY           0
#define FILLED          1
#define NUM_PIECES      7
#define PIECES_SIZE     4

#define WINDOW_W       500   
#define WINDOW_H       660   


#define BOARD_LEFT      20   
#define BOARD_TOP       20   

Color piece_colors[7] = {
    SKYBLUE,    // I piece
    YELLOW,     // O piece
    PURPLE,     // T piece
    GREEN,      // S piece
    RED,        // Z piece
    BLUE,       // J piece
    ORANGE      // L piece
};


int pieces[7][4][4] = {
    /* I */ {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
    /* O */ {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
    /* T */ {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
    /* S */ {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
    /* Z */ {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
    /* J */ {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
    /* L */ {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}}
};


int board[BOARD_HEIGHT][BOARD_WIDTH];
int board_colors[BOARD_HEIGHT][BOARD_WIDTH]; 
int current_piece[4][4];
int current_x, current_y, current_type;
int score, lines, level, game_over;


int fall_timer = 0;


void  init_game();
void  new_piece();
void  copy_piece(int src[4][4], int dst[4][4]);
int   check_collision(int piece[4][4], int x, int y);
void  freeze_piece();
int   clear_lines();
void  try_rotate();
void  move_left();
void  move_right();
void  move_down();
int   get_fall_speed();
void  draw_game();
void  draw_cell(int col, int row, Color color);
void  draw_grid();
void  draw_panel();

// ---------------- MAIN FUNCTION ---------------- //

int main(void)
{
    srand((unsigned int)time(NULL));
    InitWindow(WINDOW_W, WINDOW_H, "TETRIS - Raylib Edition");

    SetTargetFPS(60);

    init_game();

    while (!WindowShouldClose() && !game_over) 
    {
        if (IsKeyPressed(KEY_LEFT))  
        move_left();
        if (IsKeyPressed(KEY_RIGHT)) 
        move_right();
        if (IsKeyPressed(KEY_UP))    
        try_rotate();
        if (IsKeyPressed(KEY_Q))     
        game_over = 1;
        if (IsKeyDown(KEY_DOWN)) 
        {
              fall_timer += 5;
        }

        
        fall_timer++;
        if( fall_timer >= get_fall_speed()) 
        {
            fall_timer = 0;
            move_down();
        }
        BeginDrawing();
            draw_game();
        EndDrawing();
    }

 
    if (game_over)
    {
        while (!WindowShouldClose()) 
        {
            BeginDrawing();
                ClearBackground((Color){15, 15, 25, 255});
                DrawText("GAME OVER!!", 120, 220, 50, PINK);
                DrawText(TextFormat("Score: %d", score), 170, 290, 30, WHITE);
                DrawText(TextFormat("Lines: %d", lines), 170, 330, 30, LIGHTGRAY);
                DrawText(TextFormat("Level: %d", level), 170, 370, 30, LIGHTGRAY);
                DrawText("Press ENTER to exit", 140, 440, 22, DARKGRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) 
            break;
        }
    }
    CloseWindow();
    return 0;
}

void draw_game() 
{
    int i, j;
    ClearBackground((Color){15, 15, 25, 255});
    draw_grid();
    for (i = 0; i < BOARD_HEIGHT; i++) 
    {
        for (j = 0; j < BOARD_WIDTH; j++) 
        {
            if (board[i][j] == FILLED) 
            {
                Color c = piece_colors[board_colors[i][j]];
                draw_cell(j, i, c);
            }
        }
    }

    for (i = 0; i < 4; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            if (current_piece[i][j] == 0) 
            continue;
            int board_row = current_y + i;
            int board_col = current_x + j;
            if (board_row >= 0 && board_row < BOARD_HEIGHT &&
                board_col >= 0 && board_col < BOARD_WIDTH) 
                {
                draw_cell(board_col, board_row, piece_colors[current_type]);
            }
        }
    }

    DrawRectangleLines(
        BOARD_LEFT - 2,              /* x position   */
        BOARD_TOP  - 2,              /* y position   */
        BOARD_WIDTH * CELL_SIZE + 4, /* width        */
        BOARD_HEIGHT * CELL_SIZE + 4,/* height       */
        (Color){80, 80, 100, 255}    /* border color */
    );

    draw_panel();
}


void draw_cell(int col, int row, Color color) 
{
    int px = BOARD_LEFT + col * CELL_SIZE;  
    int py = BOARD_TOP  + row * CELL_SIZE; 
    DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, color);
    DrawRectangle(px + 2, py + 2, CELL_SIZE - 4, 3, (Color){255, 255, 255, 60});
}

void draw_grid() 
{
    int i, j;
    for (i = 0; i < BOARD_HEIGHT; i++) 
    {
        for (j = 0; j < BOARD_WIDTH; j++) 
        {
            if (board[i][j] == EMPTY) 
            {
                int cx = BOARD_LEFT + j * CELL_SIZE;
                int cy = BOARD_TOP  + i * CELL_SIZE;
                DrawRectangle(cx - 1, cy - 1, 5, 4,(Color){40, 40, 60, 255}); 
            }
        }
    }
}


void draw_panel() 
{
    int px = BOARD_LEFT + BOARD_WIDTH * CELL_SIZE + 20; 
    int py = BOARD_TOP;                                  
    int pw = 130;   
    DrawRectangle(px, py, pw, 400, (Color){25, 25, 40, 255});
    DrawRectangleLines(px, py, pw, 400, (Color){60, 60, 90, 255});

    DrawText("TETRIS", px + 18, py + 15, 24, RED);
    DrawLine(px + 10, py + 45, px + pw - 10, py + 45, (Color){60,60,90,255});

    DrawText("SCORE",  px + 15, py + 60,  16, LIGHTGRAY);
    DrawText(TextFormat("%d", score), px + 15, py + 80, 22, YELLOW);

    DrawText("LINES",  px + 15, py + 120, 16, LIGHTGRAY);
    DrawText(TextFormat("%d", lines), px + 15, py + 140, 22, GREEN);

    DrawText("LEVEL",  px + 15, py + 180, 16, LIGHTGRAY);
    DrawText(TextFormat("%d", level), px + 15, py + 200, 22, SKYBLUE);

    DrawLine(px + 10, py + 240, px + pw - 10, py + 240, (Color){60,60,90,255});
    
    DrawText("CONTROLS:", px + 12, py + 255, 14, LIGHTGRAY);
    DrawText("A D  Move",  px + 12, py + 278, 13, DARKGRAY);
    DrawText(" W  Rotate", px + 12, py + 296, 13, DARKGRAY);
    DrawText("S   Drop",   px + 12, py + 314, 13, DARKGRAY);
    DrawText("Q    Quit",   px + 12, py + 332, 13, DARKGRAY);

    int bar_y = py + 370;
    DrawText("SPEED", px + 15, bar_y, 14, LIGHTGRAY);
    DrawRectangle(px + 15, bar_y + 18, 100, 8, (Color){40, 40, 60, 255});
    int bar_fill = (lines % 10) * 10; 
    DrawRectangle(px + 15, bar_y + 18, bar_fill, 8, ORANGE);
}


void init_game() 
{
    int r, c;
    for (r = 0; r < BOARD_HEIGHT; r++)
    {
        for (c = 0; c < BOARD_WIDTH; c++) 
        {
            board[r][c]        = EMPTY;
            board_colors[r][c] = 0;
        }
    score = 0; lines = 0; level = 1; game_over = 0;fall_timer = 0;
    new_piece();
    }
}

void new_piece() 
{
    current_type = rand() % NUM_PIECES;
    copy_piece(pieces[current_type], current_piece);
    current_x = BOARD_WIDTH / 2 - 4 / 2;
    current_y = 0;
    if (check_collision(current_piece, current_x, current_y))
        game_over = 1;
}

void copy_piece(int src[4][4],
                int dst[4][4]) 
                {
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            dst[i][j] = src[i][j];
}

int check_collision(int piece[4][4], int x, int y) 
{
    int i, j, br, bc;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (piece[i][j] == 0) continue;
            br = y + i;
            bc = x + j;
            if (bc < 0 || bc >= BOARD_WIDTH)  return 1;
            if (br >= BOARD_HEIGHT)            return 1;
            if (br < 0)                        continue;
            if (board[br][bc] == FILLED)       return 1;
        }
    }
    return 0;
}

void try_rotate()
{
    int rotated[4][4];
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            rotated[j][4 - 1 - i] = current_piece[i][j];
    if (!check_collision(rotated, current_x, current_y))
        copy_piece(rotated, current_piece);
}

int clear_lines() 
{
    int i, j, k, count = 0, full;
    for (i = BOARD_HEIGHT - 1; i >= 0; i--) 
    {
        full = 1;
        for (j = 0; j < BOARD_WIDTH; j++)
            if (board[i][j] == EMPTY) { full = 0; break; }
        if (full) {
            count++;
            for (k = i; k > 0; k--)
                for (j = 0; j < BOARD_WIDTH; j++) 
                {
                    board[k][j]        = board[k-1][j];
                    board_colors[k][j] = board_colors[k-1][j];
                }
            for (j = 0; j < BOARD_WIDTH; j++) 
            {
                board[0][j]        = EMPTY;
                board_colors[0][j] = 0;
            }
            i++;
        }
    }
    return count;
}

void freeze_piece() 
{
    int i, j, br, bc, cleared;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (current_piece[i][j] == 0) continue;
            br = current_y + i;
            bc = current_x + j;
            if (br >= 0 && br < BOARD_HEIGHT) 
            {
                board[br][bc]        = FILLED;
                board_colors[br][bc] = current_type; /* save color! */
            }
        }
    }
    cleared = clear_lines();
    if (cleared > 0) 
    {
        lines += cleared;
        if (cleared == 1) score += 100 * level;
        if (cleared == 2) score += 300 * level;
        if (cleared == 3) score += 500 * level;
        if (cleared == 4) score += 800 * level;
        level = (lines / 10) + 1;
    }
    new_piece();
}

void move_left()  
{
    if (!check_collision(current_piece, current_x - 1, current_y))
        current_x--;
}
void move_right() 
{
    if (!check_collision(current_piece, current_x + 1, current_y))
        current_x++;
}
void move_down() 
{
    if (!check_collision(current_piece, current_x, current_y + 1))
        current_y++;
    else
        freeze_piece();
}

int get_fall_speed() 
{
    int speed = 45 - (level - 1) * 4;
    return speed < 8 ? 8 : speed;
}
