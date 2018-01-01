
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart/uart.h"

#define UART_BAUD_RATE 9600

int main()
{
	uint8_t key = 0;
	char buf[10];
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	//sei();

	while(1)
	{
		//key = 0;	//Get the keycode of pressed key
		sprintf(buf, "asdf: %d", key);
		uart_puts(buf);
		uart_puts("\r\n");
		//_delay_ms(500);
		//LCDWriteIntXY(0,0,key,3);	//Print it at location 0,0 on LCD.
		key = key + 1;
	}
	return(0);
}

