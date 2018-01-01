/*
lcdpcf8574 lib sample

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "lcdpcf8574/lcdpcf8574.h"
#include "Keypad/Keypad.h"

#define UART_BAUD_RATE 9600
//#define F_CPU 16000000
#include "uart/uart.h"

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'C','D','E','F'}
};
byte rowPins[ROWS] = {3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void display_main(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t rate, uint8_t height, uint8_t relay);

int main(void)
{
	//DDRC = 0b00000000;        //make port a as input
	//PORTC = 0b00110000;       //enable all pull-ups  

	//init uart
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

    sei();

    //init lcd
    lcd_init(LCD_DISP_ON);
    lcd_led(0); //set led

    //lcd go home
    lcd_home();
	
	uint8_t count = 0;
	

    while(1) {
		
		uint8_t month = count % 12 + 1;
		uint8_t day = 9;
		uint8_t hour = 7;
		uint8_t minute = 7;
		uint8_t second = 8;
		
		uint8_t rate = count % 12 + 1;
		uint8_t height = count % 2;
		uint8_t relay = count % 2;
		
		display_main(month, day, hour, minute, second, rate, height, relay);
		
		
		char customKey = customKeypad.getKey();

		if (customKey){
		Serial.println(customKey);
		}
		
		_delay_ms(100);
		count++;
    }
}

void display_main(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t rate, uint8_t height, uint8_t relay)
{
	char buf1[16];
	char buf2[16];
	char buf_m[4];
	char buf_r[4];
	
	switch(month){
		case 1: strcpy(buf_m, "Jan\0"); break;
		case 2: strcpy(buf_m, "Feb\0"); break;
		case 3: strcpy(buf_m, "Mar\0"); break;
		case 4: strcpy(buf_m, "Apr\0"); break;
		case 5: strcpy(buf_m, "May\0"); break;
		case 6: strcpy(buf_m, "Jun\0"); break;
		case 7: strcpy(buf_m, "Jul\0"); break;
		case 8: strcpy(buf_m, "Aug\0"); break;
		case 9: strcpy(buf_m, "Sep\0"); break;
		case 10: strcpy(buf_m, "Oct\0"); break;
		case 11: strcpy(buf_m, "Nov\0"); break;
		case 12: strcpy(buf_m, "Dec\0"); break;
		default: strcpy(buf_m, "   \0");
	}
	switch(relay){
		case 0: strcpy(buf_r, "OFF\0"); break;
		case 1: strcpy(buf_r, " ON\0"); break;
		default: strcpy(buf_r, "   \0");
	}
	
	//taruh parameter
	sprintf(buf1, "%c%c%c-%2d|%2d:%2d:%2d", buf_m[0], buf_m[1], buf_m[2], day, hour, minute, second);
	sprintf(buf2, "%3ds|%4dcm|%c%c%c", rate, height, buf_r[0], buf_r[1], buf_r[2]);
	
	lcd_gotoxy(0, 0);
	lcd_puts(buf1);
	uart_puts(buf1);
	uart_puts("\r\n");
	
	lcd_gotoxy(0, 1);
	lcd_puts(buf2);
	uart_puts(buf2);
	uart_puts("\r\n");
}
