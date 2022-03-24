#include "joypi.h"
#include "game.h"

int main(void)
{
    int led_matrix_fd, seven_segment_fd;
    init_joypi(&led_matrix_fd, &seven_segment_fd);
    setup_event_listeners();
    init_ncurses();

    int col, row;
    getmaxyx(stdscr, row, col);

    GameBoard *game_board = create_game_board(create_snake(5, 5), col, row), *led_game_board;

    for(int i=0; i<APPLE_NUMBER; i++) place_apple(game_board, i);

    int score = 0;
    unsigned char led_map[8];

    g_direction = J_RIGHT;

    while(true) {
        clear();
        //affichage du serpent
        display_snake(game_board->snake);
        //affichage des pommes
        display_apples(game_board->apples);
        refresh();
        g_direction = get_direction(g_direction);
        led_game_board = scale_board(game_board);
        translate_led_map(led_game_board, led_map);
        display_led_map(led_matrix_fd, led_map);
        display_score(seven_segment_fd, score);
        if(!move_snake(game_board, g_direction, &score)) break;
    }
    
    endwin();
    clear_LED(led_matrix_fd);
    stop_MAX7219(led_matrix_fd);
    return 0;
}