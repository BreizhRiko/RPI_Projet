#include "game.h"

GameBoard *create_game_board(SnakePart *snake, int col, int row)
{
    GameBoard *game_board = malloc(sizeof(GameBoard));
    game_board->snake = snake;
    game_board->col = col;
    game_board->row = row;
    return game_board;
}

SnakePart *create_snake(int x, int y)
{
    /*SnakePart *head = create_snake_part(5, 5);
    SnakePart *tail = create_snake_part(5, 5);
    head->next_part = tail;
    return head;*/
    SnakePart *snake = create_snake_part(x, x);
    return snake;
}

SnakePart *create_snake_part(int x, int y)
{
    SnakePart *snake_part = malloc(sizeof(SnakePart));
    snake_part->x = x;
    snake_part->y = y;
    snake_part->next_part = NULL;
    return snake_part;
}

void place_apple(GameBoard *game_board, int apple_id)
{
    int apple_x, apple_y;
    do {
        apple_x = rand()%game_board->col;
        apple_y = rand()%game_board->row;
    } while(is_occupied(game_board, apple_x, apple_y));
    game_board->apples[apple_id].x = apple_x;
    game_board->apples[apple_id].y = apple_y;
}

void display_snake(SnakePart *snake)
{
    while(snake) {
        mvaddch(snake->y, snake->x, (char) SNAKE_CHAR);
        snake = snake->next_part;
    }
}

void display_apples(Apple *apples)
{
    for(int i=0; i<APPLE_NUMBER; i++) {
        mvaddch(apples[i].y, apples[i].x, (char) APPLE_CHAR);
    }
}

bool is_occupied(GameBoard *game_board, int x, int y)
{
    if(is_occupied_by_snake(game_board, x, y)) return true;
    if(is_occupied_by_apple(game_board, x, y)) return true;
    return false;
}

bool is_occupied_by_snake(GameBoard *game_board, int x, int y)
{
    SnakePart *snake = game_board->snake;
    while(snake) {
        if(snake->x == x && snake->y == y) return true;
        snake = snake->next_part;
    }
    return false;
}

bool is_occupied_by_apple(GameBoard *game_board, int x, int y)
{
    for(int i; i<APPLE_NUMBER; i++) {
        if(game_board->apples[i].x == x && game_board->apples[i].y == y) return true;
    }
    return false;
}

//voir si il y a collision (serpent ou mur)
//voir si il y a une pomme
//              -> si pomme on place une part sur la pomme
//              -> sinon on place une part et on enleve la queue
bool move_snake(GameBoard *game_board, int direction, int *score)
{
    //check collision mur + creation de la nouvelle tete du serpent
    SnakePart *new_snake_head = check_move(game_board, direction);
    if(new_snake_head == NULL) return false;

    //check collision serpent
    if(is_occupied_by_snake(game_board, new_snake_head->x, new_snake_head->y)) return false;

    new_snake_head->next_part = game_board->snake;
    game_board->snake = new_snake_head;
    //check pomme
    if(!is_occupied_by_apple(game_board, new_snake_head->x, new_snake_head->y)) {
        remove_snake_tail(game_board);
    } else {
        *score += 5;
        find_and_replace_apple(game_board, new_snake_head->x, new_snake_head->y);
    }

    return true;
}

SnakePart *check_move(GameBoard *game_board, int direction)
{
    SnakePart *snake = game_board->snake;
    int new_x = snake->x, new_y = snake->y;
    switch(direction) {
        case UP:
        new_y = snake->y - 1;
        break;
        
        case DOWN:
        new_y = snake->y + 1;
        break;
        
        case LEFT:
        new_x = snake->x - 1;
        break;

        case RIGHT:
        new_x = snake->x + 1;
        break;
    }
    if(new_x < 0 || new_y < 0 || new_x > game_board->col || new_y > game_board->row) return NULL;
    else return create_snake_part(new_x, new_y);
}

void remove_snake_tail(GameBoard *game_board)
{
    SnakePart *current_part, *previous_part = NULL;
    current_part = game_board->snake;
    while(true) {
        if(current_part->next_part != NULL) {
            previous_part = current_part;
            current_part = current_part->next_part;
        } else {
            break;
        }
    }
    previous_part->next_part = NULL;
    free(current_part);
}

void find_and_replace_apple(GameBoard *game_board, int x, int y)
{
    int i;
    for(i = 0; i < APPLE_NUMBER; i++) {
        if(game_board->apples[i].x == x && game_board->apples[i].y == y) break;
    }
    place_apple(game_board, i);
}

void translate_led_map(GameBoard *led_game_board, unsigned char led_grid[8])
{
    unsigned char _led_grid[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int i;
    for(i = 0; i < 8; i++) led_grid[i] = _led_grid[i];

    //placer le serpent
    SnakePart *led_snake = led_game_board->snake;
    int x, y;
    while(led_snake != NULL) {
        x = led_snake->x;
        y = led_snake->y;

        if(x > 7) x = 7;
        if(x < 0) x = 0;
        if(y > 7) y = 7;
        if(y < 0) y = 0;
        led_grid[y] |= translate_register_from_x(x);

        led_snake = led_snake->next_part;
    }

    //placer les pommes
    for(i = 0; i < APPLE_NUMBER; i++) {
        x = led_game_board->apples[i].x;
        y = led_game_board->apples[i].y;

        if(x > 7) x = 7;
        if(x < 0) x = 0;
        if(y > 7) y = 7;
        if(y < 0) y = 0;
        led_grid[y] |= translate_register_from_x(x);
    }
}

GameBoard *scale_board(GameBoard *game_board)
{
	GameBoard *led_game_board = create_game_board(create_snake(game_board->snake->x, game_board->snake->y), 8, 8);

    double height_ratio = (double) (led_game_board->row) / game_board->row, width_ratio = (double) (led_game_board->col) / game_board->col;

    //mise à l'échelle du serpent
    SnakePart *snake = game_board->snake, *led_snake = led_game_board->snake;

    while(snake != NULL) {
        led_snake->x = (int) (snake->x * width_ratio);
        led_snake->y = (int) (snake->y * height_ratio);
        if(snake->next_part != NULL) led_snake->next_part = create_snake_part(0, 0);
        led_snake = led_snake->next_part;
        snake = snake->next_part;
    }

    //mise à l'échelle des pommes
    for(int i = 0; i < APPLE_NUMBER; i++) {
        led_game_board->apples[i].x = (int) (game_board->apples[i].x * width_ratio);
        led_game_board->apples[i].y = (int) (game_board->apples[i].y * height_ratio);
    }

    return led_game_board;
}

unsigned char translate_register_from_x(int x)
{
    unsigned char matrix_register;

    switch(x) {
        case 0:
        matrix_register = 0b10000000;
        break;

        case 1:
        matrix_register = 0b1000000;
        break;

        case 2:
        matrix_register = 0b100000;
        break;

        case 3:
        matrix_register = 0b10000;
        break;

        case 4:
        matrix_register = 0b1000;
        break;

        case 5:
        matrix_register = 0b100;
        break;

        case 6:
        matrix_register = 0b10;
        break;

        case 7:
        matrix_register = 0b1;
        break;
    }

    return matrix_register;
}