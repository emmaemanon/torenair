
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
#include "torenair.h"

extern volatile uint8_t sys_state; // system state

extern uint8_t lcd_buffer1[17];
extern uint8_t lcd_buffer2[17];

/******************************************************************************
 * LCD Functions
 *****************************************************************************/

// Display message on the LCD screen
void display_msg(void) {
	display_line1();
	display_line2();
}

// Display 1st line in LCD depending on system state
void display_line1(void) {
	lcd_gotoxy(0, 0);
	LCDstring(lcd_buffer1, strlen((char*) lcd_buffer1));
}

// Display 2nd line in LCD depending on system state
void display_line2(void) {
	lcd_gotoxy(0, 1);
	LCDstring(lcd_buffer2, strlen((char*) lcd_buffer2));
}

// Update and display user input based on system state and keypad button
void update_input(void) {
	// switch(sys_state) {
		// case IDLE:
			// switch (keynumber) {
			// }
			// break;
		// default: break;
	// }
}


