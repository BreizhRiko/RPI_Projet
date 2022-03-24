#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>

//BUTTON PORTS
#define UP_BUTTON_PORT 25
#define DOWN_BUTTON_PORT 23
#define LEFT_BUTTON_PORT 6
#define RIGHT_BUTTON_PORT 24

//SPI PORTS MAX7219
#define MOSI_PORT 12
#define CLK_PORT 14
#define LOAD_PORT 11

//MAX7219 MATRIX REGISTERS
#define DECODE_MODE 0x09
#define INTENSITY 0x0a
#define SCAN_LIMIT 0x0b 	//inutile pour notre utilisation
#define SHUTDOWN 0x0c
#define DISPLAY_TEST 0x0f

//VARIABLES DE DIRECTION DU JEU
#define J_UP 0
#define J_DOWN 1
#define J_LEFT 2
#define J_RIGHT 3

//CONFIG DATA 7SEGMENT
#define HT16K33_BLINK_CMD 0x80 
#define HT16K33_BLINK_DISPLAYON 0x01 
#define HT16K33_BLINK_OFF 0 
#define HT16K33_BLINK_2HZ  1 
#define HT16K33_BLINK_1HZ  2 
#define HT16K33_BLINK_HALFHZ  3 
#define HT16K33_CMD_BRIGHTNESS 0xE0

extern int g_direction;

void init_joypi(int *led_matrix_fd, int *seven_segment_fd);
void init_gpio_ports(void);
void init_MAX7219(int led_matrix_fd);
void init_7segment(int seven_segment_fd);
void stop_MAX7219(int led_matrix_fd);
void write_byte_MAX7219(unsigned char data);
void write_MAX7219(int led_matrix_fd, unsigned char register_adress, unsigned char data_to_write);
void setup_event_listeners(void);

void up_button(void);
void down_button(void);
void left_button(void);
void right_button(void);

void init_ncurses(void);

void display_led_map(int led_matrix_fd, unsigned char led_map[8]);
void display_score(int seven_segment_fd, int score);
int get_direction(int previous_direction);
void clear_LED(int led_matrix_fd);