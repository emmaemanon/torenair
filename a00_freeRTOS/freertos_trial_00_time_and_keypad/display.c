
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart/uart.h"

#include "lcdpcf8574/lcdpcf8574.h"

#include "display.h"

void display_main(volatile uint8_t *month,volatile uint8_t *day,volatile uint8_t *hour,volatile uint8_t *minute,volatile uint8_t *second,volatile uint8_t *rate,volatile uint8_t *height,volatile uint8_t *relay)
{
	char buf1[20];
	char buf2[20];
	char buf_m[4];
	char buf_r[4];
	char buf_r_m;
	
	switch(*month){
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
	switch(*relay){
		case 0: strcpy(buf_r, "OFF\0"); break;
		case 1: strcpy(buf_r, " ON\0"); break;
		default: strcpy(buf_r, "   \0");
	}
	buf_r_m = 0xFF;
	
	//taruh parameter
	sprintf(buf1, "%c%c%c-%2d|%2d:%2d:%2d%c", buf_m[0], buf_m[1], buf_m[2], *day, *hour, *minute, *second, buf_r_m);
	sprintf(buf2, "%3ds|%4dcm|%c%c%c%c", *rate, *height, buf_r[0], buf_r[1], buf_r[2], buf_r_m);
	
	lcd_gotoxy(0, 0);
	lcd_puts(buf1);
	// uart_puts(buf1);
	// uart_puts("\r\n");
	
	lcd_gotoxy(0, 1);
	lcd_puts(buf2);
	// uart_puts(buf2);
	// uart_puts("\r\n");
}