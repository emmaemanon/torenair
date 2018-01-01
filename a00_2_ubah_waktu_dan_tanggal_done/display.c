
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

extern volatile uint16_t hr_t1;
extern volatile uint16_t hr_t0;
extern volatile uint8_t mn_t1;
extern volatile uint8_t mn_t0;
extern volatile uint8_t sc_t1;
extern volatile uint8_t sc_t0;
extern volatile uint8_t count;

void display_main(volatile uint8_t *month,volatile uint8_t *day,volatile uint8_t *hour,volatile uint8_t *minute,volatile uint8_t *second,volatile uint8_t *rate,volatile uint8_t *height,volatile uint8_t *relay,volatile uint8_t *relay_mode)
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
	switch(*relay_mode){
		case 0: buf_r_m = 0x41; break;
		case 1: buf_r_m = 0x4D; break;
		default:
		{
			buf_r_m = 0x45;
		}
	}
	//buf_r_m = 0xFF;
	
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

void display_menu_wkt_tgl(void)
{
	char buf1[20];
	char buf2[20];
	
	sprintf(buf1, "1- Ubah Waktu   ");
	sprintf(buf2, "2- Ubah Tanggal ");
	
	lcd_gotoxy(0, 0);
	lcd_puts(buf1);
	
	lcd_gotoxy(0, 1);
	lcd_puts(buf2);
}

void display_ubah_waktu(void)
{
	char buf1[20];
	char buf2[20];
	
	sprintf(buf1, "  hh : mm : ss %d", count);
	sprintf(buf2, "  %d%d : %d%d : %d%d  ", hr_t1, hr_t0, mn_t1, mn_t0, sc_t1, sc_t0);
	
	lcd_gotoxy(0, 0);
	lcd_puts(buf1);
	
	lcd_gotoxy(0, 1);
	lcd_puts(buf2);
}

void display_ubah_tanggal(void)
{
	char buf1[20];
	char buf2[20];
	
	sprintf(buf1, "ubah tanggal 1");
	sprintf(buf2, "ubah tanggal 2");
	
	lcd_gotoxy(0, 0);
	lcd_puts(buf1);
	
	lcd_gotoxy(0, 1);
	lcd_puts(buf2);
}

void display_prompt_waktu(void)
{
	char buf1[20];
	char buf2[20];
	
	sprintf(buf1, "prompt waktu 1");
	sprintf(buf2, "prompt waktu 2");
	
	lcd_gotoxy(0, 0);
	lcd_puts(buf1);
	
	lcd_gotoxy(0, 1);
	lcd_puts(buf2);
}

void display_prompt_tgl(void)
{
	char buf1[20];
	char buf2[20];
	
	sprintf(buf1, "prompt tanggal 1");
	sprintf(buf2, "prompt tanggal 2");
	
	lcd_gotoxy(0, 0);
	lcd_puts(buf1);
	
	lcd_gotoxy(0, 1);
	lcd_puts(buf2);
}