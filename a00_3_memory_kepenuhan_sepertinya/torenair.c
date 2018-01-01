
#include <avr/io.h>

void init_arduino(void) {
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;
}