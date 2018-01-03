//MAIN CODE

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

#define UART_BAUD_RATE 9600

uint32_t count = 0;


//program utama
int main (void) {
	
	char buf[3];
	// ------------------- UART init code --------------------
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	sei();
	while(1) {
		count = (count)%10+1;
		sprintf(buf, "%lu\n\r", (long unsigned)count);
		// uart_puts(buf);
		// sprintf(buf, "%d", count);
		// buf[0] = (unsigned char)count;
		uart_puts(buf);
		// uart_putc("\n");
		_delay_ms(1000);
	}
	
	return 0;
}

