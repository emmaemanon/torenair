//TIME STAMPER

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
#include "keypad.h"

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

//program utama
int main (void)
{
	//DDRA = 0x00;
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	//PORTA = 0xFF;
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;
	
	char buf[10];
	uint8_t key = 0xFF;
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
		
		display_main(&month, &day, &hour, &minute, &second, &rate, &height, &relay);
		
		key = GetKeyPressed();	//Get the keycode of pressed key
		sprintf(buf, "asdf: %d", key);
		uart_puts(buf);
		uart_puts("\r\n");
		_delay_ms(20);
	}
}


