
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart/uart.h"

#define UART_BAUD_RATE 9600

#include "keypad.h"

uint8_t keymap(uint8_t input) {
	switch(input) {
		case 0:
			return(0x01); //1
		break;
		case 1:
			return(0x02); //2
		break;
		case 2:
			return(0x03); //3
		break;
		case 3:
			return(0x41); //A
		break;
		case 4:
			return(0x04); //4
		break;
		case 5:
			return(0x05); //5
		break;
		case 6:
			return(0x06); //6
		break;
		case 7:
			return(0x42); //B
		break;
		case 8:
			return(0x07); //7
		break;
		case 9:
			return(0x08); //8
		break;
		case 10:
			return(0x09); //9
		break;
		case 11:
			return(0x43); //C
		break;
		case 12:
			return(0x2A); //*
		break;
		case 13:
			return(0x00); //0
		break;
		case 14:
			return(0x23); //#
		break;
		case 15:
			return(0x44); //D
		break;
        default:
        {
			return(0xFF); //0xFF
        }
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
	
	DDR_R1 &= ~(1 << PD_R1);
	DDR_R2 &= ~(1 << PD_R2);
	DDR_R3 &= ~(1 << PD_R3);
	DDR_R4 &= ~(1 << PD_R4);
	PORT_R1 |= (1 << PD_R1);
	PORT_R2 |= (1 << PD_R2);
	PORT_R3 |= (1 << PD_R3);
	PORT_R4 |= (1 << PD_R4);
	
	for(c = 0; c < 4; c++)
	{
		DDRCol(0b00001000 >> c);
		PORTCol(0b11110111 >> c);
		for(r = 0; r < 4; r++)
		{
			if (bit_is_clear(PIN_R1, PD_R1)) {
				_delay_us(1000);
				if (bit_is_clear(PIN_R1, PD_R1)) {
					return (c);
				}
				return (0xFF);
			}
			if (bit_is_clear(PIN_R2, PD_R2)) {
				_delay_us(1000);
				if (bit_is_clear(PIN_R2, PD_R2)) {
					return (c + 4);
				}
				return (0xFF);
			}
			if (bit_is_clear(PIN_R3, PD_R3)) {
				_delay_us(1000);
				if (bit_is_clear(PIN_R3, PD_R3)) {
					return (c + 8);
				}
				return (0xFF);
			}
			if (bit_is_clear(PIN_R4, PD_R4)) {
				_delay_us(1000);
				if (bit_is_clear(PIN_R4, PD_R4)) {
					return (c + 12);
				}
				return (0xFF);
			}
		}
	}
	return (0xFF); //Indicate No keypressed
}


// int main()
// {
	// char buf[10];
	// uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	// sei();
	
	// uint8_t key = 0xFF;

	// while(1)
	// {
		// key = GetKeyPressed();	//Get the keycode of pressed key
		// sprintf(buf, "asdf: %d", key);
		// uart_puts(buf);
		// uart_puts("\r\n");
		// //_delay_ms(200);
	// }
	// return(0);
// }

