//TIME STAMPER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart/uart.h"

#include "lcdpcf8574/lcdpcf8574.h"

//#define F_CPU	16000000UL

#define UART_BAUD_RATE 9600

volatile uint16_t year = 2017;//1; //MAX 65536
volatile uint8_t month = 12;//1; //MAX 12
volatile uint8_t day = 31;//1; //MAX 31
volatile uint8_t hour = 6;//; //MAX 23
volatile uint8_t minute = 16;//; //MAX 60
volatile uint8_t second = 4; //MAX 60
volatile uint8_t rate = 0;
volatile uint8_t height = 0;
volatile uint8_t relay = 0;

//interrupt penghitung waktu
ISR(TIMER1_COMPA_vect)
{
	//tambah detik setiap masuk interrupt
	second++;
	
	if(second == 60) //kembalikan detik ke 0 setiap 60 hitungan
	{
		second = 0;
		minute++;
	}
	if(minute == 60) //kembalikan menit ke 0 setiap 60 hitungan
	{
		minute = 0;
		hour++;
	}
	if(hour == 24) //kembalikan jam ke 0 setiap 24 hitungan
	{
		hour = 0;
		day++;
	}
	if ((month == 2) && (((year%4 == 0) && (year%100 != 0)) || (year%400 == 0)) && (day == 30)) //reset hari
	{
		day = 1;
		month++;
	}
	if ((month == 2) && ((year%4 != 0) || ((year%100 == 0) && (year%400 != 0))) && (day == 29))
	{
		day = 1;
		month++;
	}
	if (((month == 1) || (month == 3) || (month == 5) || (month == 7) || (month == 8) || (month == 10) || (month == 12)) && (day == 32))
	{
		day = 1;
		month++;
	}
	if (((month == 4) || (month == 6) || (month == 9) || (month == 11)) && (day == 31))
	{
		day = 1;
		month++;
	}
	if(month == 13) //kembalikan jam ke 0 setiap 24 hitungan
	{
		month = 1;
		year++;
	}
}

void display_main(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t rate, uint8_t height, uint8_t relay)
{
	char buf1[20];
	char buf2[20];
	char buf_m[4];
	char buf_r[4];
	char buf_r_m;
	
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
	buf_r_m = 0xFF;
	
	//taruh parameter
	sprintf(buf1, "%c%c%c-%2d|%2d:%2d:%2d%c", buf_m[0], buf_m[1], buf_m[2], day, hour, minute, second,buf_r_m);
	sprintf(buf2, "%3ds|%4dcm|%c%c%c%c", rate, height, buf_r[0], buf_r[1], buf_r[2],buf_r_m);
	
	lcd_gotoxy(0, 0);
	lcd_puts(buf1);
	uart_puts(buf1);
	uart_puts("\r\n");
	
	lcd_gotoxy(0, 1);
	lcd_puts(buf2);
	uart_puts(buf2);
	uart_puts("\r\n");
}

//program utama
int main (void)
{
	
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	
    //init lcd
    lcd_init(LCD_DISP_ON);
    lcd_led(0); //set led
	
    //lcd go home
    lcd_home();
	
	//EICRA = 0x0F; //rising edge INT0 dan INT1 memberikan interrupt
	//EIMSK = 0x03; //mengaktifkan INT0 dan INT1
	
	TCCR1B = (1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10);//(1<<WGM12)|(1<<CS12);//(1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10); //CS12 untuk prescaler 1024, WGM12 untuk mode operasi CTC
	OCR1A = 15624;//62499; //nilai OCRA dengan F_CPU 16MHz dan untuk waktu countdown 1 detik
	TIMSK1 = (1<<OCIE1A); //mengaktifkan Timer/Counter2 Compare Match A
	sei(); //set global interrupt enable
	
	while(1)
	{
		//DDRA = 0x00;
		DDRB = 0x00;
		DDRC = 0x00;
		DDRD = 0x00;
		//PORTA = 0xFF;
		PORTB = 0xFF;
		PORTC = 0xFF;
		PORTD = 0xFF;
		
		display_main(month, day, hour, minute, second, rate, height, relay);
		
		_delay_ms(20);
		
		//sprintf(buf, "%2d-%2d-%2d|%2d:%2d:%2d", year, month, day, hour, minute, second);
		//uart_puts(buf);
		//uart_puts("\r\n");
	}
}


