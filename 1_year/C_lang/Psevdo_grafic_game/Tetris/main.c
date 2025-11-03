#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define BOARD_WIDTH  20
#define BOARD_HEIGHT 20
#define OFFSET_X     5
#define OFFSET_Y     2

int board[BOARD_HEIGHT][BOARD_WIDTH] = {0};

// 7 figure: {dy, dx}
int PIECES[7][4][2] = {
    {{0,0}, {0,-1}, {0,1}, {0,2}}, // I
    {{0,0}, {0,1}, {1,0}, {1,1}}, // O
    {{0,0}, {0,-1}, {0,1}, {1,0}}, // T
    {{0,0}, {1,0}, {0,-1}, {0,1}}, // L
    {{0,0}, {1,0}, {0,1}, {0,-1}}, // J
    {{0,0}, {0,1}, {1,0}, {1,-1}}, // S
    {{0,0}, {0,-1}, {1,0}, {1,1}}  // Z
};

int current_piece_index;
int next_piece_index;
int piece_x, piece_y;
int rotation = 0;

int score = 0;
int level = 1;
int lines_cleared = 0;
int speed_ms = 500;
int fast_drop = 0; //flag for speed

// --- code CP866 ---
#define CORNER_TL 201  // ╔
#define CORNER_TR 187  // ╗
#define CORNER_BL 200  // ╚
#define CORNER_BR 188  // ╝
#define H_LINE    205  // ═
#define V_LINE    186  // ║
#define BLOCK     219  // █
#define BLOOD_BLOCK 178 // ▓ (for blood)

void gotoxy(int x, int y) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    SetConsoleCursorPosition(h, pos);
}

void set_color(int bg, int fg) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, (bg << 4) | fg);
}

void reset_color() {
    set_color(0, 7);
}

void draw_block(int x, int y, int filled) {
    gotoxy(OFFSET_X + x * 2, OFFSET_Y + y);
    if (filled) {
        printf("%c%c", BLOCK, BLOCK);
    } else {
        printf("  ");
    }
}

void draw_board() {
    gotoxy(OFFSET_X - 1, OFFSET_Y - 1);
    printf("%c", CORNER_TL);
    for (int i = 0; i < BOARD_WIDTH; i++) printf("%c%c", H_LINE, H_LINE);
    printf("%c", CORNER_TR);

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        gotoxy(OFFSET_X - 1, OFFSET_Y + y);
        printf("%c", V_LINE);
        for (int x = 0; x < BOARD_WIDTH; x++) {
            draw_block(x, y, board[y][x]);
        }
        gotoxy(OFFSET_X + BOARD_WIDTH * 2, OFFSET_Y + y);
        printf("%c", V_LINE);
    }

    gotoxy(OFFSET_X - 1, OFFSET_Y + BOARD_HEIGHT);
    printf("%c", CORNER_BL);
    for (int i = 0; i < BOARD_WIDTH; i++) printf("%c%c", H_LINE, H_LINE);
    printf("%c", CORNER_BR);
}

void draw_piece(int px, int py, int piece_idx, int erase) {
    for (int i = 0; i < 4; i++) {
        int x = px + PIECES[piece_idx][i][1];
        int y = py + PIECES[piece_idx][i][0];
        if (y >= 0 && y < BOARD_HEIGHT && x >= 0 && x < BOARD_WIDTH) {
            draw_block(x, y, !erase);
        }
    }
}

int can_place(int px, int py, int piece_idx) {
    for (int i = 0; i < 4; i++) {
        int x = px + PIECES[piece_idx][i][1];
        int y = py + PIECES[piece_idx][i][0];
        if (x < 0 || x >= BOARD_WIDTH || y >= BOARD_HEIGHT) return 0;
        if (y >= 0 && board[y][x]) return 0;
    }
    return 1;
}

void lock_piece() {
    for (int i = 0; i < 4; i++) {
        int x = piece_x + PIECES[current_piece_index][i][1];
        int y = piece_y + PIECES[current_piece_index][i][0];
        if (y >= 0) board[y][x] = 1;
    }
}

void clear_lines() {
    int lines = 0;
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        int full = 1;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x] == 0) { full = 0; break; }
        }
        if (full) {
            for (int yy = y; yy > 0; yy--) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    board[yy][x] = board[yy-1][x];
                }
            }
            for (int x = 0; x < BOARD_WIDTH; x++) board[0][x] = 0;
            lines++;
            y++;
        }
    }
    if (lines > 0) {
        lines_cleared += lines;
        score += lines * 100;
        level = 1 + lines_cleared / 10;
        speed_ms = 500 - (level - 1) * 50;
        if (speed_ms < 50) speed_ms = 50;
    }
}

void draw_ui() {
    // next
    gotoxy(OFFSET_X + BOARD_WIDTH * 2 + 3, OFFSET_Y);
    printf("NEXT:");

    gotoxy(OFFSET_X + BOARD_WIDTH * 2 + 3, OFFSET_Y + 2);
    printf("%c", CORNER_TL);
    for (int i = 0; i < 10; i++) printf("%c", H_LINE);
    printf("%c", CORNER_TR);

    for (int i = 0; i < 6; i++) {
        gotoxy(OFFSET_X + BOARD_WIDTH * 2 + 3, OFFSET_Y + 3 + i);
        printf("%c          %c", V_LINE, V_LINE);
    }

    gotoxy(OFFSET_X + BOARD_WIDTH * 2 + 3, OFFSET_Y + 9);
    printf("%c", CORNER_BL);
    for (int i = 0; i < 10; i++) printf("%c", H_LINE);
    printf("%c", CORNER_BR);

    for (int y = OFFSET_Y + 3; y <= OFFSET_Y + 8; y++) {
        for (int x = OFFSET_X + BOARD_WIDTH * 2 + 4; x <= OFFSET_X + BOARD_WIDTH * 2 + 12; x++) {
            gotoxy(x, y);
            printf(" ");
        }
    }

    int px = OFFSET_X + BOARD_WIDTH * 2 + 6;
    int py = OFFSET_Y + 5;
    for (int i = 0; i < 4; i++) {
        int x = px + PIECES[next_piece_index][i][1] * 2;
        int y = py + PIECES[next_piece_index][i][0];
        gotoxy(x, y);
        printf("%c%c", BLOCK, BLOCK);
    }

    gotoxy(OFFSET_X + BOARD_WIDTH * 2 + 3, OFFSET_Y + 12);
    printf("Point: %d", score);
    gotoxy(OFFSET_X + BOARD_WIDTH * 2 + 3, OFFSET_Y + 13);
    printf("LVL: %d", level);
    gotoxy(OFFSET_X + BOARD_WIDTH * 2 + 3, OFFSET_Y + 14);
    printf("Speed: %d ms", speed_ms);
}

void rotate_piece() {
    int temp_piece[4][2];
    for (int i = 0; i < 4; i++) {
        temp_piece[i][0] = PIECES[current_piece_index][i][0];
        temp_piece[i][1] = PIECES[current_piece_index][i][1];
    }

    draw_piece(piece_x, piece_y, current_piece_index, 1);

    for (int i = 0; i < 4; i++) {
        PIECES[current_piece_index][i][0] = -temp_piece[i][1];
        PIECES[current_piece_index][i][1] = temp_piece[i][0];
    }

    if (!can_place(piece_x, piece_y, current_piece_index)) {
        for (int i = 0; i < 4; i++) {
            PIECES[current_piece_index][i][0] = temp_piece[i][0];
            PIECES[current_piece_index][i][1] = temp_piece[i][1];
        }
    }

    draw_piece(piece_x, piece_y, current_piece_index, 0);
    rotation = (rotation + 1) % 4;
}

void draw_you_dead() {
    int start_x = OFFSET_X + 8;
    int start_y = OFFSET_Y + 8;

    char dead_art[5][60];

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 59; j++) {
            dead_art[i][j] = ' ';
        }
        dead_art[i][59] = '\0';
    }

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if((i == 0 && (j == 0 || j == 4)) ||
               (i == 1 && (j == 1 || j == 3)) ||
               (i >= 2 && j == 2)) {
                dead_art[i][j] = BLOCK;
            }
        }
    }

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if((i == 0 || i == 4) && (j > 0 && j < 4)) {
                dead_art[i][j+6] = BLOCK;
            }
            if((j == 0 || j == 4) && (i > 0 && i < 4)) {
                dead_art[i][j+6] = BLOCK;
            }
        }
    }

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if((j == 0 || j == 4) && i < 4) {
                dead_art[i][j+12] = BLOCK;
            }
            if(i == 4 && (j > 0 && j < 4)) {
                dead_art[i][j+12] = BLOCK;
            }
        }
    }

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(j == 0) {
                dead_art[i][j+24] = BLOCK;
            }
            if((i == 0 || i == 4) && (j > 0 && j < 4)) {
                dead_art[i][j+24] = BLOCK;
            }
            if((j == 4) && (i > 0 && i < 4)) {
                dead_art[i][j+24] = BLOCK;
            }
        }
    }

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(j == 0) {
                dead_art[i][j+30] = BLOCK;
            }
            if((i == 0 || i == 2 || i == 4) && (j > 0 && j < 4)) {
                dead_art[i][j+30] = BLOCK;
            }
        }
    }

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if((i == 0 || i == 2) && (j > 0 && j < 4)) {
                dead_art[i][j+36] = BLOCK;
            }
            if((j == 0 || j == 4) && i > 0) {
                dead_art[i][j+36] = BLOCK;
            }
        }
    }

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 5; j++) {
            if(j == 0) {
                dead_art[i][j+42] = BLOCK;
            }
            if((i == 0 || i == 4) && (j > 0 && j < 4)) {
                dead_art[i][j+42] = BLOCK;
            }
            if((j == 4) && (i > 0 && i < 4)) {
                dead_art[i][j+42] = BLOCK;
            }
        }
    }

    int art_height = 5;

    set_color(0, 4);

    for (int i = 0; i < art_height; i++) {
        gotoxy(start_x, start_y + i);
        for (int j = 0; j < 59; j++) {
            printf("%c", dead_art[i][j]);
        }
    }

    reset_color();
}

void blood_animation() {
    for (int stage = 0; stage < 3; stage++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH+10; x++) {
                if ((y + x + stage) % 3 == 0) {
                    set_color(4, 4);
                    gotoxy(OFFSET_X + x * 2, OFFSET_Y + y);
                    printf("%c%c", BLOOD_BLOCK, BLOOD_BLOCK);
                    reset_color();
                }
            }
        }
        Sleep(400);
    }

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            set_color(4, 4);
            gotoxy(OFFSET_X + x * 2, OFFSET_Y + y);
            printf("%c%c", BLOOD_BLOCK, BLOOD_BLOCK);
            reset_color();
        }
        Sleep(50);
    }
}

void game_over_animation() {
    system("cls");

    draw_you_dead();
    Sleep(3000);

    blood_animation();
    Sleep(2000);

    set_color(4, 15);
    gotoxy(OFFSET_X + 10, OFFSET_Y + BOARD_HEIGHT + 5);
    printf("Press ANY KEY for new game or ESC to exit");
    reset_color();
}

int start_new_game() {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            board[y][x] = 0;
        }
    }

    current_piece_index = rand() % 7;
    next_piece_index = rand() % 7;
    piece_x = BOARD_WIDTH / 2;
    piece_y = 0;
    score = 0;
    level = 1;
    lines_cleared = 0;
    speed_ms = 500;
    fast_drop = 0;
    rotation = 0;

    system("cls");
    draw_board();
    draw_ui();
    draw_piece(piece_x, piece_y, current_piece_index, 0);

    return 1;
}

int main() {
    SetConsoleOutputCP(866);
    SetConsoleCP(866);
    srand((unsigned)time(NULL));

    current_piece_index = rand() % 7;
    next_piece_index = rand() % 7;
    piece_x = BOARD_WIDTH / 2;
    piece_y = 0;

    draw_board();
    draw_ui();
    draw_piece(piece_x, piece_y, current_piece_index, 0);

    int game_over = 0;
    int paused = 0;
    DWORD last_move = GetTickCount();

    while (1) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 27) break; // Esc
            if (key == ' ') paused = !paused;

            if (!paused) {
                int old_x = piece_x;
                int old_y = piece_y;

                if (key == 75) piece_x--; // ←
                if (key == 77) piece_x++; // →
                if (key == 80) { // ↓
                    fast_drop = 1;
                }
                if (key == 72) rotate_piece();
                if (key == 82 || key == 'r') {
                    for (int i = 0; i < 3; i++) rotate_piece();
                }

                if ((key == 75 || key == 77) && !can_place(piece_x, piece_y, current_piece_index)) {
                    piece_x = old_x;
                } else if (key == 75 || key == 77) {
                    draw_piece(old_x, piece_y, current_piece_index, 1);
                    draw_piece(piece_x, piece_y, current_piece_index, 0);
                }
            }
        }

        if (!paused && !game_over) {
            DWORD current_time = GetTickCount();
            int current_speed = fast_drop ? 50 : speed_ms;

            if ((int)(current_time - last_move) >= current_speed) {
                last_move = current_time;
                int old_y = piece_y;
                piece_y++;

                if (!can_place(piece_x, piece_y, current_piece_index)) {
                    piece_y = old_y;
                    lock_piece();
                    clear_lines();
                    draw_board();
                    draw_ui();

                    current_piece_index = next_piece_index;
                    next_piece_index = rand() % 7;
                    piece_x = BOARD_WIDTH / 2;
                    piece_y = 0;
                    fast_drop = 0;

                    if (!can_place(piece_x, piece_y, current_piece_index)) {
                        game_over = 1;
                    } else {
                        draw_ui();
                        draw_piece(piece_x, piece_y, current_piece_index, 0);
                    }
                } else {
                    draw_piece(piece_x, old_y, current_piece_index, 1);
                    draw_piece(piece_x, piece_y, current_piece_index, 0);
                }
            }
        }

        if (game_over) {
            game_over_animation();

            char key = _getch();
            if (key == 27) {
                break;
            } else {
                start_new_game();
                game_over = 0;
                last_move = GetTickCount();
            }
        }

        if (paused && !game_over) {
            gotoxy(OFFSET_X + BOARD_WIDTH / 2 - 3, OFFSET_Y + BOARD_HEIGHT + 2);
            printf("PAUSE");
        } else if (!game_over) {
            gotoxy(OFFSET_X + BOARD_WIDTH / 2 - 3, OFFSET_Y + BOARD_HEIGHT + 2);
            printf("      ");
        }

        Sleep(10);
    }

    reset_color();
    return 0;
}