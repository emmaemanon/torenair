
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart/uart.h"

#include "torenair.h"

extern uint8_t butnum; // The decoded button number
extern uint8_t keynumber; // Actual numerical value of button number
extern uint8_t button_flag;
extern uint8_t last_butnum; // Last stored keypad number
extern uint8_t last_keynumber;
extern uint8_t button_state;

uint8_t keymap(uint8_t input) {
	switch(input) {
		case 0: return(0x01); break; //1
		case 1: return(0x02); break; //2
		case 2: return(0x03); break; //3
		case 3: return(0x41); break; //A
		case 4: return(0x04); break; //4
		case 5: return(0x05); break; //5
		case 6: return(0x06); break; //6
		case 7: return(0x42); break; //B
		case 8: return(0x07); break; //7
		case 9: return(0x08); break; //8
		case 10: return(0x09); break; //9
		case 11: return(0x43); break; //C
		case 12: return(0x2A); break; //*
		case 13: return(0x00); break; //0
		case 14: return(0x23); break; //#
		case 15: return(0x44); break; //D
        default: {return(0xFF);} //0xFF
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

uint8_t read_keypad(void)
{
	last_butnum = butnum;
	unsigned char r, c;
	
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
				return NO_BUTTON_PUSHED;
			}
			if (bit_is_clear(PIN_R2, PD_R2)) {
				_delay_us(1000);
				if (bit_is_clear(PIN_R2, PD_R2)) {
					return (c + 4);
				}
				return NO_BUTTON_PUSHED;
			}
			if (bit_is_clear(PIN_R3, PD_R3)) {
				_delay_us(1000);
				if (bit_is_clear(PIN_R3, PD_R3)) {
					return (c + 8);
				}
				return NO_BUTTON_PUSHED;
			}
			if (bit_is_clear(PIN_R4, PD_R4)) {
				_delay_us(1000);
				if (bit_is_clear(PIN_R4, PD_R4)) {
					return (c + 12);
				}
				return NO_BUTTON_PUSHED;
			}
		}
	}
	return NO_BUTTON_PUSHED;
}

// Check button pressed and appropriately set flags
void check_button(void) {
    if ((butnum == NO_BUTTON_PUSHED) || (last_butnum != butnum)) {
        button_state = RELEASED_STATE;
        button_flag = NOT_PUSHED_FLAG;
	}
}

void debounce_button(void) {
	switch(button_state) {
		case RELEASED_STATE:
			if (butnum != NO_BUTTON_PUSHED) {
				button_state = MAYBE_PUSH_STATE;
			}
			break;

		case MAYBE_PUSH_STATE:
			if (butnum != NO_BUTTON_PUSHED) {
				button_state = PUSHED_STATE;
				button_flag = PUSHED_FLAG;
			}
			else {
				button_state = RELEASED_STATE;
			}
			break;

		case PUSHED_STATE:
			if (butnum != NO_BUTTON_PUSHED) {
				button_state = MAYBE_REL_STATE;
			}
			break;

		case MAYBE_REL_STATE:
			if (butnum != NO_BUTTON_PUSHED) {
				button_state = PUSHED_STATE;
			}
			else {
				button_state = RELEASED_STATE;
				button_flag = NOT_PUSHED_FLAG;
			}
			break;

		default:
			break;
	}
}

// Determine button action based on button pressed
void button_action(void) {
	// Change state based on setting button press
	change_state();
	display_msg();
	last_keynumber = keynumber;
}


