
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart/uart.h"

#define UART_BAUD_RATE 9600

#include "Keypad.h"

//#define KEYPAD_PIN_ROW	((bit_is_clear(PIN_R1, PD_R1) << 3) \
//						((bit_is_clear(PIN_R2, PD_R2) << 2) \
//						((bit_is_clear(PIN_R3, PD_R3) << 1) \
//						((bit_is_clear(PIN_R4, PD_R4) << 0)
//| (PIN_R2 << 2) | (PIN_R3 << 1) | (PIN_R4))
//#define KEYPAD_PIN_COL ((PIN_C1 << 3) | (PIN_C2 << 2) | (PIN_C3 << 1) | (PIN_C4))

void DDRRow(uint8_t byteValue)
{
	if((byteValue >> 3) & 1){
		DDR_R1 |= (1 << PD_R1);
	} else {
		DDR_R1 &= ~(1 << PD_R1);
	}
	if((byteValue >> 2) & 1){
		DDR_R2 |= (1 << PD_R2);
	} else {
		DDR_R2 &= ~(1 << PD_R2);
	}
	if((byteValue >> 1) & 1){
		DDR_R3 |= (1 << PD_R3);
	} else {
		DDR_R3 &= ~(1 << PD_R3);
	}
	if((byteValue >> 0) & 1){
		DDR_R4 |= (1 << PD_R4);
	} else {
		DDR_R4 &= ~(1 << PD_R4);
	}
}

void DDRCol(uint8_t byteValue)
{
	if((byteValue >> 3) & 1){
		DDR_C1 |= (1 << PD_C1);
	} else {
		DDR_C1 &= ~(1 << PD_C1);
	}
	if((byteValue >> 2) & 1){
		DDR_C2 |= (1 << PD_C2);
	} else {
		DDR_C2 &= ~(1 << PD_C2);
	}
	if((byteValue >> 1) & 1){
		DDR_C3 |= (1 << PD_C3);
	} else {
		DDR_C3 &= ~(1 << PD_C3);
	}
	if((byteValue >> 0) & 1){
		DDR_C4 |= (1 << PD_C4);
	} else {
		DDR_C4 &= ~(1 << PD_C4);
	}
}

void PORTCol(uint8_t byteValue)
{
	if((byteValue >> 3) & 1){
		PORT_C1 |= (1 << PD_C1);
	} else {
		PORT_C1 &= ~(1 << PD_C1);
	}
	if((byteValue >> 2) & 1){
		PORT_C2 |= (1 << PD_C2);
	} else {
		PORT_C2 &= ~(1 << PD_C2);
	}
	if((byteValue >> 1) & 1){
		PORT_C3 |= (1 << PD_C3);
	} else {
		PORT_C3 &= ~(1 << PD_C3);
	}
	if((byteValue >> 0) & 1){
		PORT_C4 |= (1 << PD_C4);
	} else {
		PORT_C4 &= ~(1 << PD_C4);
	}
}

uint8_t GetKeyPressed()
{
	uint8_t r, c;
	
	//PortRow(0x0F);
	//PortCol(0x00);
	
	DDR_R1 &= ~(1 << PD_R1);
	DDR_R2 &= ~(1 << PD_R2);
	DDR_R3 &= ~(1 << PD_R3);
	DDR_R4 &= ~(1 << PD_R4);
	PORT_R1 |= (1 << PD_R1);
	PORT_R2 |= (1 << PD_R2);
	PORT_R3 |= (1 << PD_R3);
	PORT_R4 |= (1 << PD_R4);
	//PORT_C1 |= ((1 << PIN_C1);
	//PORT_C2 |= ((1 << PIN_C2);
	//PORT_C3 |= ((1 << PIN_C3);
	//PORT_C4 |= ((1 << PIN_C4);
	
	for(c=0; c<4; c++)
	{
		DDRCol(0b00001000 >> c);
		PORTCol(0b11110111 >> c);
		for(r=0; r<4; r++)
		{
			//return(KEYPAD_PIN_ROW);
			if (bit_is_clear(PIN_R1, PD_R1)) {
				return (c);
			}
			if (bit_is_clear(PIN_R2, PD_R2)) {
				return (c + 4);//r*4 + c);
			};
			if (bit_is_clear(PIN_R3, PD_R3)) {
				return (c + 8);
			}
			if (bit_is_clear(PIN_R4, PD_R4)) {
				return (c + 12);//r*4 + c);
			}
			//if(!(KEYPAD_PIN_ROW & (0X08 >> r)))
			//{
			//	return (KEYPAD_PIN_ROW);//r*4 + c);
			//}
			//_delay_ms(1000);
		}
		//DDRCol(0x08 >> c);
		//PORTCol(~(0x08 >> c));
		//DDRCol(0x08 >> c);
		//_delay_ms(10);
	}
	return 0XFF;//Indicate No keypressed
}

int main()
{
	char buf[10];
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	sei();
	
	uint8_t key = 0xFF;

	while(1)
	{
		key = GetKeyPressed();	//Get the keycode of pressed key
		sprintf(buf, "asdf: %d", key);
		uart_puts(buf);
		uart_puts("\r\n");
		_delay_ms(200);
	}
	return(0);
}

