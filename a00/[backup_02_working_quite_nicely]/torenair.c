
#include <avr/io.h>
// #include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "uart/uart.h"
#include "lcdpcf8574/lcdpcf8574.h"
#include "torenair.h"
#include "ultrasonic.h"

extern volatile uint16_t systime_ms; // system state

// System Time Setup (millisecond precision)
void init_time_keypad(void) {
	// set up timer 0 for 1 mSec ticks
	TIMSK2 = (1<<OCIE2A); // turn on timer 0 cmp match ISR
	OCR2A = 249; // set the compare reg to 250 time ticks
	TCCR2A = (1<<WGM21); // turn on clear-on-match
	TCCR2B = (1<<CS22) | (0<<CS21) | (0<<CS20);	// clock prescalar to 64

	// reset system time to 0
	systime_ms = 0;
}

void initialize(void) {
	DDRB = 0x00;
	DDRC = 0x00;
	// DDRD = 0x00;
	PORTB = 0xFF;
	PORTC = 0xFF;
	// PORTD = 0xFF;
	
	// ------------------- keypad init code --------------------
	init_time_keypad();
	
	// ------------------- timer init code --------------------
	TCCR1B = (1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10);//(1<<WGM12)|(1<<CS12);//(1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10); //CS12 untuk prescaler 1024, WGM12 untuk mode operasi CTC
	OCR1A = 15624;//15999;//15624;//62499; //nilai OCRA dengan F_CPU 16MHz dan untuk waktu countdown 1 detik
	TIMSK1 = (1<<OCIE1A); //mengaktifkan Timer/Counter2 Compare Match A
	
	//init uart
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
    //init lcd
    lcd_init(LCD_DISP_ON);
    lcd_led(0); //set led
	
    //lcd go home
    lcd_home();
	
	//sei(); //set global interrupt enable
}