#include "joypi.h"

int g_direction;

void init_joypi(int *led_matrix_fd, int *seven_segment_fd)
{
    wiringPiSetup();
    init_gpio_ports();
	*led_matrix_fd = wiringPiSPISetup(0, 32000000);
    init_MAX7219(*led_matrix_fd);
	*seven_segment_fd = wiringPiI2CSetup(0x70);
	init_7segment(*seven_segment_fd);
}

void init_gpio_ports(void)
{
    // initialisation des ports des boutons de la croix directionnelle
    pinMode(UP_BUTTON_PORT, OUTPUT);
    pinMode(DOWN_BUTTON_PORT, OUTPUT);
    pinMode(LEFT_BUTTON_PORT, OUTPUT);
    pinMode(RIGHT_BUTTON_PORT, OUTPUT);

    // initialisation des ports de la matrice LED
    pinMode(MOSI_PORT, OUTPUT);
	pinMode(CLK_PORT, OUTPUT);
	pinMode(LOAD_PORT, OUTPUT);
	digitalWrite(LOAD_PORT, 1);
}

void init_MAX7219(int led_matrix_fd)
{
	write_MAX7219(led_matrix_fd, DECODE_MODE, 0x00);	//désactive le decode-mode
	write_MAX7219(led_matrix_fd, INTENSITY, 0x01);		//met l'intensité des LED à 1
	write_MAX7219(led_matrix_fd, SCAN_LIMIT, 0x07);	//non utilisé
	write_MAX7219(led_matrix_fd, SHUTDOWN,0x01);		//active la matrice LED
	write_MAX7219(led_matrix_fd, DISPLAY_TEST, 0x00);	//désactive la fonction d'affichage "test"
    clear_LED(led_matrix_fd);
}

void init_7segment(int seven_segment_fd)
{
	wiringPiI2CWrite(seven_segment_fd, 0x21);
	wiringPiI2CWrite(seven_segment_fd, HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | ((unsigned char) HT16K33_BLINK_OFF << 1));
	wiringPiI2CWrite(seven_segment_fd, HT16K33_CMD_BRIGHTNESS | (unsigned char) 10);
}

void stop_MAX7219(int led_matrix_fd)
{
    write_MAX7219(led_matrix_fd, SHUTDOWN, 0x00);
}

void setup_event_listeners(void)
{
    wiringPiISR(UP_BUTTON_PORT, INT_EDGE_FALLING, up_button);	    //active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton haut
    wiringPiISR(DOWN_BUTTON_PORT, INT_EDGE_FALLING, down_button);	//active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton bas
	wiringPiISR(LEFT_BUTTON_PORT, INT_EDGE_FALLING, left_button);	//active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton gauche
	wiringPiISR(RIGHT_BUTTON_PORT, INT_EDGE_FALLING, right_button);	//active un écouteur pour une interruption lors d'un front descendant sur le gpio du bouton droit
}

void write_byte_MAX7219(unsigned char data)
{
	for(int i = 0; i < 8; i++) {
		digitalWrite(CLK_PORT, 0);
		digitalWrite(MOSI_PORT, data & 0x80);
		delayMicroseconds(1);
		data <<= 1;
		digitalWrite(CLK_PORT, 1);
	}
}

void write_MAX7219(int led_matrix_fd, unsigned char register_adress, unsigned char data_to_write)
{
	unsigned char adress_and_data[2] = {register_adress, data_to_write};

	digitalWrite(LOAD_PORT, 0);
	write_byte_MAX7219(register_adress);
	write_byte_MAX7219(data_to_write);
	digitalWrite(LOAD_PORT, 1);
}

void clear_LED(int led_matrix_fd)
{
	write_MAX7219(led_matrix_fd, 0x01, 0x00);
	write_MAX7219(led_matrix_fd, 0x02, 0x00);
	write_MAX7219(led_matrix_fd, 0x03, 0x00);
	write_MAX7219(led_matrix_fd, 0x04, 0x00);
	write_MAX7219(led_matrix_fd, 0x05, 0x00);
	write_MAX7219(led_matrix_fd, 0x06, 0x00);
	write_MAX7219(led_matrix_fd, 0x07, 0x00);
	write_MAX7219(led_matrix_fd, 0x08, 0x00);
}

void up_button(void)
{
	g_direction = J_UP;
}

void down_button(void)
{
    g_direction = J_DOWN;
}

void left_button(void)
{
    g_direction = J_LEFT;
}

void right_button(void)
{
    g_direction = J_RIGHT;
}

void init_ncurses(void)
{
    initscr();
    cbreak();		// pas de buffer de ligne
    noecho();		// pas d'echo des input getchar
    curs_set(0);	// désactive le curseur
}

void display_led_map(int led_matrix_fd, unsigned char led_map[8])
{
	for(int i = 0; i < 8; i++) {
		write_MAX7219(led_matrix_fd, (unsigned char) (i + 1), led_map[i]);
	}
}

void display_score(int seven_segment_fd, int score)
{
	int ones, tens, hundreds, thousands;
	int number[4];
	unsigned char adress = 0x00;

	thousands = score * 0.001;
	number[0] = thousands;
    score -= thousands * 1000;
    hundreds = score * 0.01;
	number[1] = hundreds;
    score -= hundreds * 100;
    tens = score * 0.1;
	number[2] = tens;
    score -= tens * 10;
    ones = score;
	number[3] = ones;

	unsigned char digit_number_data[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

	for(int i = 0; i < 4; i++) { 
		if(i == 2) adress += 2;
		wiringPiI2CWriteReg8(seven_segment_fd, adress++, digit_number_data[number[i]] & 0xFF); 
		wiringPiI2CWriteReg8(seven_segment_fd, adress++, digit_number_data[number[i]] >> 8);  
	}
}

int get_direction(int previous_direction)
{
	delay(100);
    switch(g_direction) {
        case J_UP:
        if(previous_direction != J_DOWN) return J_UP;
        return previous_direction;
        break;

        case J_DOWN:
        if(previous_direction != J_UP) return J_DOWN;
        return previous_direction;
        break;

        case J_LEFT:
        if(previous_direction != J_RIGHT) return J_LEFT;
        return previous_direction;
        break;

        case J_RIGHT:
        if(previous_direction != J_LEFT) return J_RIGHT;
        return previous_direction;
        break;

        default:
        return previous_direction;
        break;
    }
}