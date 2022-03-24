#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#define APPLE_NUMBER 5
#define SNAKE_CHAR 254
#define APPLE_CHAR 162

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

typedef struct {
    int x;
    int y;
    struct SnakePart *next_part;
} SnakePart;

typedef struct {
    int x;
    int y;
} Apple;

typedef struct {
    SnakePart *snake;
    Apple apples[APPLE_NUMBER];
    int col;
    int row;
} GameBoard;

GameBoard *create_game_board(SnakePart *snake, int col, int row);
SnakePart *create_snake(int x, int y);
SnakePart *create_snake_part(int x, int y);
void place_apple(GameBoard *game_board, int apple_id);
void display_snake(SnakePart *snake);
void display_apples(Apple *apples);
bool is_occupied(GameBoard *game_board, int x, int y);
bool is_occupied_by_snake(GameBoard *game_board, int x, int y);
bool is_occupied_by_apple(GameBoard *game_board, int x, int y);
bool move_snake(GameBoard *game_board, int direction, int *score);
SnakePart *check_move(GameBoard *game_board, int direction);
void remove_snake_tail(GameBoard *game_board);
void find_and_replace_apple(GameBoard *game_board, int x, int y);

void translate_led_map(GameBoard *led_game_board, unsigned char led_grid[8]);
GameBoard *scale_board(GameBoard *game_board);
unsigned char translate_register_from_x(int x);
