/*
 * lab5_stethoscope.c
 *
 * Created: 4/11/2012 12:30:00 PM
 * Authors: Michael Wu (myw9) and Garen Der-Khachadourian (gdd9)
 * Description: Digital Stethoscope
 *					 Pin B4 <-> SS_BAR
 *					 Pin B5 <-> MOSI
 *					 Pin B6 <-> MISO
 *					 Pin B7 <-> SCLK
 * 				 Port B <-> Keypad
 *              Port C <-> LCD
 * 				 Pin  D.7 <-> RC circuit and microphone input
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <math.h>
#include "uart.h"
#include "lcd_lib.h"
#include "flash.h"
#include "lab5_stethoscope.h"

// System variables
volatile uint32_t systime;									// system time
volatile uint8_t sys_state = IDLE;						// system state
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);	// UART file descriptor - putchar and getchar are in uart.c

// Task time variables
volatile uint32_t task_time_debounce = TASK_PERIOD_DEBOUNCE_MS;			// time left before next button debounce (in ms)
volatile uint32_t task_time_serial_xfer = TASK_PERIOD_SERIAL_XFER_MS;	// time left before next byte of data is transfered (in ms)

// Heart Rate Variables
uint16_t profile_time1=0;
uint16_t profile_time2=0;

// Serial transfer variables
uint8_t xfer_flag = XFER_NODATA;	// flag to indicate whether data is ready to be sent via UART
uint8_t xfer_val = 0;					// measured stethoscope value to transfer via UART
uint8_t xfer_counter = 0;						// counter used in Timer 1 Compare A ISR to take samples at 100 Hz for serial transfer

// Flash memory variables
uint32_t rec_id_mapping[NUM_RECORDINGS] = {REC0_ADDR, REC1_ADDR, REC2_ADDR, REC3_ADDR, REC4_ADDR,		// Address mapping for recording id's
														 REC5_ADDR, REC6_ADDR, REC7_ADDR, REC8_ADDR, REC9_ADDR};
flash_buffer flash_write_buffer;								// Flash write buffer
flash_buffer flash_read_buffers[NUM_FLASH_READ_BUF];	// Flash read buffers
uint32_t bytes_written=0;			// number of bytes written to flash in current recording
uint32_t bytes_read=0;				// number of bytes read from flash in playback
uint16_t current_read_buffer=0;	// index of current read buffer for playback
uint16_t next_read_page=0;			// index of next flash page to read in playback

// User interface variables
uint8_t rec_id=0;
uint8_t cur_volume=2;
uint8_t acq_mode[10] = "Diaphragm";

// LCD variables
uint8_t lcd_buffer[21];			// LCD display buffer
uint8_t lcd_lines[4][20];		// Complete LCD state

// Keypad scanning and debounce variables
uint8_t button_state;
uint8_t button_flag;
unsigned char key;				// The raw keyscan
unsigned char butnum;			// The decoded button number
unsigned char last_butnum;		// Last stored keypad number
uint8_t keynumber;				// Actual numerical value of button number
uint8_t last_keynumber;
uint8_t edit_state = EDIT_ON;	// Enable user input
char user_input[20] = "                   ";
uint8_t keytbl[16]={ 0xee, 0xed, 0xeb, 0xe7,
							0xde, 0xdd, 0xdb, 0xd7,
						   0xbe, 0xbd, 0xbb, 0xb7,
						   0x7e, 0x7d, 0x7b, 0x77};
uint8_t keymap[16]={ 1, 2,  3, 12,
							4, 5,  6, 13,
							7, 8,  9, 14,
						  10, 0, 11, 15};

int main(void) {
	// Initialize system
	initialize();

	while(1) {
		// Send stethoscope data every 10 ms (at 100 Hz)
		if (task_time_serial_xfer == 0) {
			if (xfer_flag == XFER_RDY) {
				fprintf(stdout, "%u\n\r", (uint8_t) xfer_val);
				task_time_serial_xfer = TASK_PERIOD_SERIAL_XFER_MS;
				xfer_flag = XFER_NODATA;
			}
		}

		// Debounce keypad buttons every 30 ms
		if (task_time_debounce == 0) {
			butnum = read_keypad();		// Scan keypad
			check_button();				// Clear button state and flag if necessary
			if (butnum != 0) {  			// Debounce button if valid keypad state was detected
				debounce_button();
			}
			keynumber = keymap[butnum-1];
			task_time_debounce = TASK_PERIOD_DEBOUNCE_MS;	// Reset task
		}

		// Check for button press
		if (button_flag == PUSHED_FLAG) {
			button_action();
		}
	}
}

/******************************************************************************
 * Initialization Routines
 *****************************************************************************/

// General system initialize
void initialize(void) {
	init_systime();		// initialize the system time counter
	init_lcd();				// initialize lcd
	init_adc();				// initialize adc module
	init_pwm();				// initialize the PWM output and sampling
	init_sampling();		// initialize stethoscope sampling
	init_spi();				// initialize spi communication for Flash memory
	init_flash();			// initialize flash buffers

	//init the UART -- uart_init() is in uart.c
	uart_init();
	stdout = stdin = stderr = &uart_str;

	sei();					// enable interrupts
}

// System Time Setup (millisecond precision)
void init_systime(void) {
	// set up timer 0 for 1 mSec ticks
	TIMSK0 = (1<<OCIE0A);				// turn on timer 0 cmp match ISR
	OCR0A = 249;  							// set the compare reg to 250 time ticks
	TCCR0A = (1<<WGM01);					// turn on clear-on-match
	TCCR0B = (1<<CS01) + (1<<CS00);	// clock prescalar to 64

	// reset system time to 0
	systime = 0;
}

// LCD setup
void init_lcd(void) {
	LCDinit();			// initialize the display
	LCDcursorOFF();	// turn cursor off
	LCDclr();			// clear the display
	LCDGotoXY(0,0);	// reset lcd position
	display_msg();
}

// ADC setup
void init_adc(void) {
	//init the A to D converter, channel zero/ left adj /EXTERNAL Aref,
	//!!!Connect Aref jumper!!!!
	ADMUX = (1<<REFS0) +(1<<ADLAR);

	//enable ADC and set prescaler to 1/128*16MHz=125,000, clear interrupt enable, and DONT start conversion
	//ADCSRA = (1<<ADEN) + (1<<ADPS2) + (1<<ADPS1);
	//ADCSRA = (1<<ADEN) + 5;
	enable_adc();
}

// Initialize PWM (Timer 2)
void init_pwm(void) {
	TIMSK2 = 0;				// Turn off timer 2 overflow ISR
	TCCR2A = (1<<COM2A1) | (1<<WGM20) | (1<<WGM21);	// Fast PWM, toggle OC2A (pin D7), 16 us per PWM cycle
	enable_pwm();
}

// Initialize sampling of stethoscope at 8 kHz (Timer 1)
void init_sampling(void) {
	enable_sampling();
}

// Initialize flash memory structure
void init_flash(void) {
	uint8_t i=0;
	uint32_t signature=0;

	// Initialize flash buffers
	flash_write_buffer.size = 0;
	for (i=0; i<NUM_FLASH_READ_BUF; i++) {
		flash_read_buffers[i].size = 0;
	}

	// Check if signature is written in memory
	// If not, clear chip and initialize flash structure
	flash_cmd_read_data(SIGNATURE_ADDR, flash_read_buffers[0].buffer, 4);
	signature = (((uint32_t) flash_read_buffers[0].buffer[0]) << 24) |
					(((uint32_t) flash_read_buffers[0].buffer[1]) << 16) |
					(((uint32_t) flash_read_buffers[0].buffer[2]) << 8)  |
					(((uint32_t) flash_read_buffers[0].buffer[3]));
	if (signature != FLASH_SIGNATURE) {
		flash_chip_format();
	}
}

/******************************************************************************
 * LCD Functions
 *****************************************************************************/

// Display message on the LCD screen
void display_msg(void) {
	display_line1();
	display_line2();
	display_line3();
	display_line4();
}

// Display 1st line in LCD depending on system state
void display_line1(void) {
	uint8_t char_offset = 0;
	LCDGotoXY(0, 0);
	switch(sys_state) {
		case IDLE:				sprintf((char*) lcd_buffer, "Digital Stethoscope "); break;
		case PRE_CAPTURE:		sprintf((char*) lcd_buffer, "Record Mode         "); break;
		case CAPTURE:			sprintf((char*) lcd_buffer, "Recording...        "); break;
		case POST_CAPTURE:	sprintf((char*) lcd_buffer, "Recording: Done!    "); break;
		case DELETED:			sprintf((char*) lcd_buffer, "Delete: Done!       "); break;
		case PRE_PLAYBACK:	sprintf((char*) lcd_buffer, "Playback Mode       "); break;
		case PLAYBACK:			sprintf((char*) lcd_buffer, "Playing...          "); break;
		case POST_PLAYBACK:	sprintf((char*) lcd_buffer, "Playback: Done!     "); break;
		case VOLUME_SELECT:	sprintf((char*) lcd_buffer, "Volume Select       "); break;
		case SETTINGS:			sprintf((char*) lcd_buffer, "Settings            "); break;
		case FLASH_UTILITY:  sprintf((char*) lcd_buffer, "Flash Utility       "); break;
		case ERASE_FLASH:    sprintf((char*) lcd_buffer, "Erasing Flash...    "); break;
		case ERROR:				sprintf((char*) lcd_buffer, "Error:              "); break;
	}
	LCDstring(lcd_buffer, strlen((char*) lcd_buffer));
}

// Display 2nd line in LCD depending on system state
void display_line2(void) {
	uint8_t char_offset = 0;
	LCDGotoXY(0, 1);
	switch(sys_state) {
		case IDLE:				sprintf((char*) lcd_buffer, "                    "); break;
		case PRE_CAPTURE:		sprintf((char*) lcd_buffer, "                    "); break;
		case CAPTURE:			sprintf((char*) lcd_buffer, "Please Wait         "); break;
		case POST_CAPTURE:	sprintf((char*) lcd_buffer, "Stored in ID: %u     ", (uint8_t) rec_id); break;
		case DELETED:			sprintf((char*) lcd_buffer, "                    "); break;
		case PRE_PLAYBACK:	sprintf((char*) lcd_buffer, "                    "); break;
		case PLAYBACK:			sprintf((char*) lcd_buffer, "Please Wait         "); break;
		case POST_PLAYBACK:	sprintf((char*) lcd_buffer, "A: Delete   B: Play "); break;
		case VOLUME_SELECT:	sprintf((char*) lcd_buffer, "Current Volume: %u  ", (uint8_t) cur_volume); break;
		case SETTINGS:			sprintf((char*) lcd_buffer, "Volume: %d          ", cur_volume); break;
		case FLASH_UTILITY:  sprintf((char*) lcd_buffer, "                    "); break;
		case ERASE_FLASH:    sprintf((char*) lcd_buffer, "                    "); break;
		case ERROR:				sprintf((char*) lcd_buffer, "                    "); break;
	}
	LCDstring(lcd_buffer, strlen((char*) lcd_buffer));
}

// Display 3rd line in LCD depending on system state
void display_line3(void) {
	uint8_t char_offset = 0;
	LCDGotoXY(0, 2);
	switch(sys_state) {
		case IDLE:				sprintf((char*) lcd_buffer, "Enter Command:      "); break;
		case PRE_CAPTURE:		sprintf((char*) lcd_buffer, "Press 'A' to start: "); break;
		case CAPTURE:			sprintf((char*) lcd_buffer, "                    "); break;
		case POST_CAPTURE:	sprintf((char*) lcd_buffer, "A: Delete  B: Play  "); break;
		case DELETED:			sprintf((char*) lcd_buffer, "Press any key:      "); break;
		case PRE_PLAYBACK:	sprintf((char*) lcd_buffer, "ID: (0-9)           "); break;
		case PLAYBACK:			sprintf((char*) lcd_buffer, "                    "); break;
		case POST_PLAYBACK:	sprintf((char*) lcd_buffer, "                    "); break;
		case VOLUME_SELECT:	sprintf((char*) lcd_buffer, "New Volume:         "); break;
		case SETTINGS:			sprintf((char*) lcd_buffer, "Waveforms Saved: %u ", fetch_waveforms_saved()); break;
		case FLASH_UTILITY:  sprintf((char*) lcd_buffer, "Enter Command:      "); break;
		case ERASE_FLASH:    sprintf((char*) lcd_buffer, "Please Wait         "); break;
		case ERROR:				sprintf((char*) lcd_buffer, "Press any key:      "); break;
	}
	LCDstring(lcd_buffer, strlen((char*) lcd_buffer));
}

// Display 4th line in LCD depending on system state
void display_line4(void) {
	uint8_t char_offset = 0;
	LCDGotoXY(0, 3);
	switch(sys_state) {
		case IDLE:				sprintf((char*) lcd_buffer, ">%s", user_input); break;
		case PRE_CAPTURE:		sprintf((char*) lcd_buffer, ">%s", user_input); break;
		case CAPTURE:			sprintf((char*) lcd_buffer, "                    "); break;
		case POST_CAPTURE:	sprintf((char*) lcd_buffer, ">%s", user_input); break;
		case DELETED:			sprintf((char*) lcd_buffer, ">%s", user_input); break;
		case PRE_PLAYBACK:	sprintf((char*) lcd_buffer, ">%s", user_input);	break;
		case PLAYBACK:			sprintf((char*) lcd_buffer, "                    "); break;
		case POST_PLAYBACK:	sprintf((char*) lcd_buffer, ">%s", user_input); break;
		case VOLUME_SELECT:	sprintf((char*) lcd_buffer, ">%s", user_input); break;
		case SETTINGS:			sprintf((char*) lcd_buffer, ">%s", user_input); break;
		case FLASH_UTILITY:	sprintf((char*) lcd_buffer, ">%s", user_input); break;
		case ERASE_FLASH:    sprintf((char*) lcd_buffer, "                    "); break;
		case ERROR:				sprintf((char*) lcd_buffer, ">%s", user_input); break;
	}
	LCDstring(lcd_buffer, strlen((char*) lcd_buffer));
}

// Update and display user input based on system state and keypad button
void update_input(void) {
	LCDGotoXY(1,3);
	switch(sys_state) {
		// Idle: Choose one of the features/operating modes
		case IDLE:
			switch (keynumber) {
				case 1: strcpy(user_input, mode1str); LCDstring((uint8_t *) "1: Record Waveform ", 19); break;
				case 2: strcpy(user_input, mode2str); LCDstring((uint8_t *) "2: Play Waveform   ", 19); break;
				case 3: strcpy(user_input, mode3str); LCDstring((uint8_t *) "3: Volume Control  ", 19); break;
				case 4: strcpy(user_input, mode4str); LCDstring((uint8_t *) "4: View Settings   ", 19); break;
				case 5: strcpy(user_input, mode5str); LCDstring((uint8_t *) "5: Flash Utility   ", 19); break;
			}
			break;
		// Pre-capture: Start recording or exit
		case PRE_CAPTURE:
			switch (keynumber) {
				case SPECIAL_A: strcpy(user_input, start);   LCDstring((uint8_t *) "A: Start           ", 19); break;
				case SPECIAL_D: strcpy(user_input, exit1);   LCDstring((uint8_t *) "D: Exit            ", 19); break;
			}
			break;
		// Saved: Delete, play, or exit
		case POST_CAPTURE:
			switch (keynumber) {
				case SPECIAL_A: strcpy(user_input, delete1); LCDstring((uint8_t *) "A: Delete          ", 19); break;
				case SPECIAL_B: strcpy(user_input, play1);   LCDstring((uint8_t *) "B: Play            ", 19); break;
				case SPECIAL_D: strcpy(user_input, exit1);   LCDstring((uint8_t *) "D: Exit            ", 19); break;
			}
			break;
		// Deleted: Exit
		case DELETED:
			if (keynumber != ENTER) {
				strcpy(user_input, exit2); LCDstring((uint8_t *) "Exit               ", 19);
			}
			break;
		// Pre-playback: Choose a stored waveform to play
		case PRE_PLAYBACK:
			switch (keynumber) {
				case 0:
					if (is_rec_valid(0)) { strcpy(user_input, "0                  "); LCDstring((uint8_t *) "0                  ", 19); }
					else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					break;
				case 1:
					if (is_rec_valid(1)) { strcpy(user_input, "1                  "); LCDstring((uint8_t *) "1                  ", 19); }
					else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					break;
				case 2:
					if (is_rec_valid(2)) { strcpy(user_input, "2                  "); LCDstring((uint8_t *) "2                  ", 19); }
					else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					break;
				case 3:
					if (is_rec_valid(3)) { strcpy(user_input, "3                  "); LCDstring((uint8_t *) "3                  ", 19); }
					else						{ strcpy(user_input, "                   ");	LCDstring((uint8_t *) "                   ", 19); }
					break;
				case 4:
					if (is_rec_valid(4)) { strcpy(user_input, "4                  "); LCDstring((uint8_t *) "4                  ", 19); }
					else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					break;
				case 5:
					if (is_rec_valid(5)) { strcpy(user_input, "5                  "); LCDstring((uint8_t *) "5                  ", 19); }
					else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					break;
				case 6:
					if (is_rec_valid(6)) { strcpy(user_input, "6                  "); LCDstring((uint8_t *) "6                  ", 19); }
					else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					break;
				case 7:
					if (is_rec_valid(7)) { strcpy(user_input, "7                  "); LCDstring((uint8_t *) "7                  ", 19); }
					else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					break;
				case 8:
					if (is_rec_valid(8)) { strcpy(user_input, "8                  "); LCDstring((uint8_t *) "8                  ", 19); }
					else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					break;
				case 9:
					if (is_rec_valid(9)) { strcpy(user_input, "9                  "); LCDstring((uint8_t *) "9                  ", 19); }
					else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					break;
				case SPECIAL_D: strcpy(user_input, exit1);			 LCDstring((uint8_t *) "D: Exit            ", 19); break;
			}
			break;
		// Post-playback: Delete, play again, or exit
		case POST_PLAYBACK:
			switch (keynumber) {
				case SPECIAL_A: strcpy(user_input, delete1); LCDstring((uint8_t *) "A: Delete          ", 19); break;
				case SPECIAL_B: strcpy(user_input, play1);   LCDstring((uint8_t *) "B: Play            ", 19); break;
				case SPECIAL_D: strcpy(user_input, exit1);   LCDstring((uint8_t *) "D: Exit            ", 19); break;
			}
			break;
		// Volume Select: Choose volume level or exit
		case VOLUME_SELECT:
			switch (keynumber) {
				case 0:  strcpy(user_input, "0                  "); LCDstring((uint8_t *) "0                  ", 19); break;
				case 1:  strcpy(user_input, "1                  "); LCDstring((uint8_t *) "1                  ", 19); break;
				case 2:  strcpy(user_input, "2                  "); LCDstring((uint8_t *) "2                  ", 19); break;
				case 3:  strcpy(user_input, "3                  "); LCDstring((uint8_t *) "3                  ", 19); break;
				case 4:  strcpy(user_input, "4                  "); LCDstring((uint8_t *) "4                  ", 19); break;
				case 5:  strcpy(user_input, "5                  "); LCDstring((uint8_t *) "5                  ", 19); break;
				case 6:  strcpy(user_input, "6                  "); LCDstring((uint8_t *) "6                  ", 19); break;
				case 7:  strcpy(user_input, "7                  "); LCDstring((uint8_t *) "7                  ", 19); break;
				case 8:  strcpy(user_input, "8                  "); LCDstring((uint8_t *) "8                  ", 19); break;
				case 9:  strcpy(user_input, "9                  "); LCDstring((uint8_t *) "9                  ", 19); break;
				case SPECIAL_D: strcpy(user_input, exit1);			 LCDstring((uint8_t *) "D: Exit            ", 19); break;
			}
			break;
		// Settings: Exit
		case SETTINGS:
			switch (keynumber) {
				case SPECIAL_D: strcpy(user_input, exit1); LCDstring((uint8_t *) "D: Exit            ", 19); break;
			}
			break;
		// Flash Utility: Erase FLASH or exit
		case FLASH_UTILITY:
			switch (keynumber) {
				case SPECIAL_A: strcpy(user_input, erase); LCDstring((uint8_t *) "A: Erase Memory    ", 19); break;
				case SPECIAL_D: strcpy(user_input, exit1); LCDstring((uint8_t *) "D: Exit            ", 19); break;
			}
			break;
	}
}

/******************************************************************************
 * Keypad Scanning and Debouncing
 *****************************************************************************/

// Mapping a button press to a number
uint8_t read_keypad(void) {
	// Backup last stored button number
	last_butnum = butnum;

	// Get upper nibble
  	KEYPAD_UPPER_PORT_DIR &= 0x0f;
  	KEYPAD_UPPER_PORT_DATA |= 0xf0;
	KEYPAD_LOWER_PORT_DIR |= 0x0f;
	KEYPAD_LOWER_PORT_DATA &= 0xf0;
  	_delay_us(5);
  	key = (KEYPAD_UPPER_PORT_IN & 0xf0);

  	// Get lower nibble
  	KEYPAD_LOWER_PORT_DIR &= 0xf0;
  	KEYPAD_LOWER_PORT_DATA |= 0x0f;
	KEYPAD_UPPER_PORT_DIR |= 0xf0;
  	KEYPAD_UPPER_PORT_DATA &= 0x0f;
	_delay_us(5);
  	key = key | (KEYPAD_LOWER_PORT_IN & 0x0f);

  	// Find matching keycode in keytbl
  	if (key == 0xff) {
	    butnum = NO_BUTTON_PUSHED;
	}
	else {
  	    for (butnum=0; butnum<MAXKEYS; butnum++)
  	    {
  	  	    if (keytbl[butnum]==key)  break;
  	    }

  	    if (butnum==MAXKEYS) butnum = MULTIPLE_PUSHED;
  	    else butnum++;	   //adjust by one to make range 1-16
  	}

	return butnum;
}

// Check button pressed and appropriately set flags
void check_button(void) {
    if (butnum == 0 || last_butnum != butnum) {
        button_state = RELEASED_STATE;
        button_flag = NOT_PUSHED_FLAG;
		  strcpy(user_input, "                   ");
	}
}

// Button debouncing routine - run every 30 ms
void debounce_button(void) {
	switch(button_state) {
		case RELEASED_STATE:
			if (butnum != 0) {
				button_state = MAYBE_PUSH_STATE;
			}
			break;

		case MAYBE_PUSH_STATE:
			if (butnum != 0) {
				button_state = PUSHED_STATE;
				button_flag = PUSHED_FLAG;
			}
			else {
				button_state = RELEASED_STATE;
			}
			break;

		case PUSHED_STATE:
			if (butnum != 0) {
				button_state = MAYBE_REL_STATE;
			}
			break;

		case MAYBE_REL_STATE:
			if (butnum != 0) {
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
	if (edit_state == EDIT_ON) {
		update_input();
	}

	// Change state based on setting button press
	change_state();
	display_msg();
	button_flag = NOT_PUSHED_FLAG;
	last_keynumber = keynumber;
}

/******************************************************************************
 * System State Functions
 *****************************************************************************/

void change_state(void) {
	// State transitions determined by user input
	if (keynumber == ENTER) {
		switch (sys_state) {
			// Idle: Go to other modes if correct buttons pressed
			case IDLE:
				if (last_keynumber == 1) { sys_state = PRE_CAPTURE; }
				else if (last_keynumber == 2) {
					disable_capturing();
					sys_state = PRE_PLAYBACK;
				}
				else if (last_keynumber == 3) { sys_state = VOLUME_SELECT; }
				else if (last_keynumber == 4) { sys_state = SETTINGS; }
				else if (last_keynumber == 5) { sys_state = FLASH_UTILITY; }
				break;
			// Pre-capture: Capture if A is pressed or reset to IDLE
			case PRE_CAPTURE:
				if (last_keynumber == SPECIAL_D) { sys_state = IDLE; }
				else if (last_keynumber == SPECIAL_A) {
					// Find next available recording id or erase id=0
					rec_id = fetch_rec_id();
					if (rec_id == NUM_RECORDINGS) {
						flash_cmd_block_erase_64kb(REC0_ADDR);
						rec_id = 0;
					}

					sys_state = CAPTURE;
				}
				break;
			// Saved: Display confirmation and allow deletion or playback
			case POST_CAPTURE:
				if (last_keynumber == SPECIAL_D) {
					enable_capturing();
					sys_state = IDLE;
				}
				else if (last_keynumber == SPECIAL_A) {
					// Erase recording data
					flash_erase_rec(rec_id);
					sys_state = DELETED;
				}
				else if (last_keynumber == SPECIAL_B) {
					// Initialize playback by reading two pages of recording data
					flash_init_rec_read(rec_id);
					enable_playback();
					sys_state = PLAYBACK;
				}
				break;
			// Deleted: Display confirmation and go back to IDLE
			case DELETED:
				enable_capturing();
				sys_state = IDLE;
				break;
			// Pre-playback: Select saved waveform to play
			case PRE_PLAYBACK:
				if (last_keynumber == SPECIAL_D) {
					enable_capturing();
					sys_state = IDLE;
				}
				else if (last_keynumber >= 0 && last_keynumber < NUM_RECORDINGS && is_rec_valid(last_keynumber))  {
					// Initialize playback by reading two pages of recording data
					rec_id = last_keynumber;
					flash_init_rec_read(rec_id);
					enable_playback();
					sys_state = PLAYBACK;
				}
				break;
			// Post-playback: Delete or play again
			case POST_PLAYBACK:
				if (last_keynumber == SPECIAL_D) {
					enable_capturing();
					sys_state = IDLE;
				}
				else if (last_keynumber == SPECIAL_A) {
					flash_erase_rec(rec_id);
					sys_state = DELETED;
				}
				else if (last_keynumber == SPECIAL_B) {
					// Initialize playback by reading two pages of recording data
					flash_init_rec_read(rec_id);
					enable_playback();
					sys_state = PLAYBACK;
				}
				break;
			// Volume select: Set the volume for the stethoscope
			case VOLUME_SELECT:
				if (last_keynumber == SPECIAL_D) {
					sys_state = IDLE;
				}
				else if (last_keynumber >= 0 && last_keynumber <= MAX_VOLUME) {
					cur_volume = last_keynumber;
				}
				break;
			// Settings: Displays the current settings
			case SETTINGS:
				if (last_keynumber == SPECIAL_D) {
					sys_state = IDLE;
				}
				break;
			// Flash Utility: Erase the FLASH memory
			case FLASH_UTILITY:
				if (last_keynumber == SPECIAL_D) {
					sys_state = IDLE;
				}
				else if (last_keynumber == SPECIAL_A) {
					flash_chip_format();
					sys_state = ERASE_FLASH;
					display_msg();
					flash_wait_ready();
					sys_state = FLASH_UTILITY;
				}
				break;
		}
	}
}

// Enable adc, pwm, and timer 1 interrupt
void enable_capturing(void) {
	enable_adc();
	enable_pwm();
	enable_sampling();
}

// Disable adc, pwm, and timer 1 interrupt
void disable_capturing(void) {
	disable_adc();
	disable_pwm();
	disable_sampling();
}

// Enable pwm and timer 1 interrupt
void enable_playback(void) {
	disable_adc();
	enable_pwm();
	enable_sampling();
}

// Disable pwm and timer 1 interrupt
void disable_playback(void) {
	disable_adc();
	disable_pwm();
	disable_sampling();
}

// Enable ADC
void enable_adc(void) {
	//enable ADC and set prescaler to 1/128*16MHz=125,000, clear interrupt enable, and DONT start conversion
	ADCSRA = (1<<ADEN) + 5;
}

// Disable ADC
void disable_adc(void) {
	//disable ADC
	ADCSRA = 0;
}

// Enable PWM output pin
void enable_pwm(void) {
	TCCR2B = 1;				// Prescalar = 1
	OCR2A = 128 ;			// set PWM to half full scale
	DDRD = (1<<PIND7); 	// set pin D.7 as output for PWM generator
}

// Disable PWM output pin
void disable_pwm(void) {
	TCCR2B = 0;					// Prescalar = 0
	DDRD = 0;					// set pin D.7 as output for PWM generator
}

// Enable sampling of stethoscope at 8 kHz (Timer 1)
void enable_sampling(void) {
	TCCR1B = (1<<WGM12) + (1<<CS10);	// Full speed; clear-on-match
	TIMSK1 = (1<<OCIE1A);	// Timer 1 output compare A match interrupt enable
	TCCR1A = 0;					// Turn off pwm and oc lines
	OCR1A = 1999 ;				// Timer 1 ticks at 8000 Hz or 125 microsecs period=2000 ticks
}

// Disable sampling of stethoscope at 8 kHz (Timer 1)
void disable_sampling(void) {
	TCCR1B = 0;					// Disabled
	TIMSK1 = 0;					// Timer 1 output compare A match interrupt disable
	TCCR1A = 0;					// Turn off pwm and oc lines
	OCR1A = 1999 ;				// Timer 1 ticks at 8000 Hz or 125 microsecs period=2000 ticks
}

/******************************************************************************
 * Signal Capturing Functions
 *****************************************************************************/

// Read adc value
uint8_t read_adc(void) {
	uint8_t result = 0;
	ADCSRA |= (1<<ADSC);						// start conversion
	while((ADCSRA & (1<<ADSC)) != 0);	// wait for conversion to finish
	result = ADCH;								// store converted value
	return result;
}

/******************************************************************************
 * Flash Functions (Application Specific)
 *****************************************************************************/

// Erases and formats the flash memory
void flash_chip_format(void) {
	// erase entire flash memory
	flash_cmd_chip_erase();

	// initialize recording status register
	flash_write_buffer.buffer[0] = 0x00;
	flash_write_buffer.buffer[1] = 0x00;
	flash_cmd_page_prog(REC_SR_ADDR, flash_write_buffer.buffer, 2);

	// write signature
	flash_write_buffer.buffer[0] = (uint8_t) ((FLASH_SIGNATURE & 0xff000000) >> 24);
	flash_write_buffer.buffer[1] = (uint8_t) ((FLASH_SIGNATURE & 0x00ff0000) >> 16);
	flash_write_buffer.buffer[2] = (uint8_t) ((FLASH_SIGNATURE & 0x0000ff00) >> 8);
	flash_write_buffer.buffer[3] = (uint8_t) (FLASH_SIGNATURE & 0x000000ff);
	flash_cmd_page_prog(SIGNATURE_ADDR, flash_write_buffer.buffer, 4);
}

// Initialize playback by reading two pages of recording data
void flash_init_rec_read(uint8_t id) {
	flash_read_buffers[0].size = 0;
	flash_read_buffers[1].size = 0;
	flash_cmd_read_data(rec_id_mapping[id], flash_read_buffers[0].buffer, FLASH_PAGE_SIZE);
	flash_cmd_read_data(rec_id_mapping[id]+FLASH_PAGE_SIZE, flash_read_buffers[1].buffer, FLASH_PAGE_SIZE);
	next_read_page=2;
	current_read_buffer=0;
	bytes_read=0;
}

// Erase recording data from flash
void flash_erase_rec(uint8_t id) {
	uint16_t rec_sr = 0xffff;
	flash_cmd_block_erase_64kb(rec_id_mapping[id]);
	rec_sr = flash_read_rec_sr();
	rec_sr &= ~(((uint16_t) 1) << rec_id);
	flash_write_rec_sr(rec_sr);
}

// Return the number of saved waveforms in memory
uint8_t fetch_waveforms_saved(void) {
	uint16_t rec_sr = 0xffff;
	uint8_t num_recordings=0;
	uint8_t i=0;

	rec_sr = flash_read_rec_sr();
	for(i=0; i<NUM_RECORDINGS; i++) {
		if (rec_sr & (((uint16_t) 1) << i)) {
			num_recordings++;
		}
	}
	return num_recordings;
}

// Return the next available recording id or NUM_RECORDINGS if all are in use
uint8_t fetch_rec_id(void) {
	uint16_t rec_sr=0xffff;
	uint8_t i=0;

	// find first available recording id
	rec_sr = flash_read_rec_sr();
	for (i=0; i<NUM_RECORDINGS; i++) {
		if ((rec_sr & (((uint16_t) 1) << i)) == 0) {
			return i;
		}
	}
	return NUM_RECORDINGS;
}

// Read recording status register
uint16_t flash_read_rec_sr(void) {
	uint16_t rec_sr=0xffff;
	flash_cmd_read_data(REC_SR_ADDR, flash_read_buffers[0].buffer, 2);
	rec_sr = (((uint16_t) flash_read_buffers[0].buffer[0]) |
				(((uint16_t) flash_read_buffers[0].buffer[1]) << 8));
	return rec_sr;
}

// Write recording status register
void flash_write_rec_sr(uint16_t rec_sr_val) {
	// Erase data
	flash_cmd_sector_erase(REC_SR_ADDR);

	// Write data
	flash_write_buffer.buffer[0] = (uint8_t) (rec_sr_val & 0x00ff);
	flash_write_buffer.buffer[1] = (uint8_t) ((rec_sr_val & 0xff00) >> 8);
	flash_cmd_page_prog(REC_SR_ADDR, flash_write_buffer.buffer, 2);
}

// Returns 1 if the recording id is valid, 0 otherwise
uint8_t is_rec_valid(uint8_t id) {
	uint16_t rec_sr = 0xffff;
	rec_sr = flash_read_rec_sr();
	if (rec_sr & (((uint16_t) 1) << id)) {
		return 1;
	}
	else {
		return 0;
	}
}

/******************************************************************************
 * Interrupt Service Routines
 *****************************************************************************/

// Timer 0 compare ISR
ISR (TIMER0_COMPA_vect) {
	// update system time
	systime++;

	// decrement debouncing task time
	if (task_time_debounce > 0) {
		task_time_debounce--;
	}

	// decrement serial transfer task time
	if (task_time_serial_xfer > 0) {
		task_time_serial_xfer--;
	}
}

// Timer 1 Compare A ISR (executes at 8 kHz)
ISR (TIMER1_COMPA_vect) {
	int16_t capture_val=0;		// raw data
	int16_t capture_val_mod=0;	// modified data
	uint8_t is_neg=0;
	uint16_t rec_sr;

	profile_time1 = TCNT1;

	if (sys_state == IDLE || sys_state == PRE_CAPTURE || sys_state == CAPTURE || sys_state == VOLUME_SELECT || sys_state == SETTINGS) {
		// Read ADC to capture microphone value
		capture_val = (int16_t) read_adc();

		// Update captured value based on volume
		if (capture_val < BIAS_OFFSET) {
			is_neg=1;
		}
		switch(cur_volume) {
			case 0:
				capture_val_mod = BIAS_OFFSET;
				break;
			case 1:
				if (is_neg) { capture_val_mod = BIAS_OFFSET - (((uint16_t) (BIAS_OFFSET-capture_val)) >> 1);	}
				else			{ capture_val_mod = BIAS_OFFSET + (((uint16_t) (capture_val-BIAS_OFFSET)) >> 1);	}
				break;
			case 2:
				capture_val_mod = capture_val;
				break;
			case 3:
				if (is_neg) { capture_val_mod = BIAS_OFFSET - ((((uint16_t) (BIAS_OFFSET-capture_val)) << 1) - (((uint16_t) (BIAS_OFFSET-capture_val)) >> 1)); }
				else			{ capture_val_mod = BIAS_OFFSET + ((((uint16_t) (capture_val-BIAS_OFFSET)) << 1) - (((uint16_t) (capture_val-BIAS_OFFSET)) >> 1)); }
				break;
			case 4:
				if (is_neg) { capture_val_mod = BIAS_OFFSET - (((uint16_t) (BIAS_OFFSET-capture_val)) << 1); }
				else			{ capture_val_mod = BIAS_OFFSET + (((uint16_t) (capture_val-BIAS_OFFSET)) << 1); }
				break;
			case 5:
				if (is_neg) { capture_val_mod = BIAS_OFFSET - ((((uint16_t) (BIAS_OFFSET-capture_val)) << 2) - (((uint16_t) (BIAS_OFFSET-capture_val)) >> 1)); }
				else			{ capture_val_mod = BIAS_OFFSET + ((((uint16_t) (capture_val-BIAS_OFFSET)) << 2) - (((uint16_t) (capture_val-BIAS_OFFSET)) >> 1)); }
				break;
			case 6:
				if (is_neg) { capture_val_mod = BIAS_OFFSET - (((uint16_t) (BIAS_OFFSET-capture_val)) << 2); }
				else			{ capture_val_mod = BIAS_OFFSET + (((uint16_t) (capture_val-BIAS_OFFSET)) << 2); }
				break;
			case 7:
				if (is_neg) { capture_val_mod = BIAS_OFFSET - ((((uint16_t) (BIAS_OFFSET-capture_val)) << 3) - (((uint16_t) (BIAS_OFFSET-capture_val)) >> 1)); }
				else			{ capture_val_mod = BIAS_OFFSET + ((((uint16_t) (capture_val-BIAS_OFFSET)) << 3) - (((uint16_t) (capture_val-BIAS_OFFSET)) >> 1)); }
				break;
			case 8:
				if (is_neg) { capture_val_mod = BIAS_OFFSET - (((uint16_t) (BIAS_OFFSET-capture_val)) << 3); }
				else			{ capture_val_mod = BIAS_OFFSET + (((uint16_t) (capture_val-BIAS_OFFSET)) << 3); }
				break;
			case 9:
				if (is_neg) { capture_val_mod = BIAS_OFFSET - ((((uint16_t) (BIAS_OFFSET-capture_val)) << 4) - (((uint16_t) (BIAS_OFFSET-capture_val)) >> 1)); }
				else			{ capture_val_mod = BIAS_OFFSET + ((((uint16_t) (capture_val-BIAS_OFFSET)) << 4) - (((uint16_t) (capture_val-BIAS_OFFSET)) >> 1)); }
				break;
			default:
				capture_val_mod = capture_val;
				break;
		}
		capture_val_mod = min(capture_val_mod, 255);
		capture_val_mod = max(capture_val_mod, 0);

		// Write data to Flash buffer if system is in capture mode
		if (sys_state == CAPTURE) {
			if (bytes_written < FLASH_BLOCK_SIZE_64KB) {
				if (flash_write_buffer.size < FLASH_PAGE_SIZE) {	// if buffer size >= FLASH_PAGE_SIZE, then an error occured
					flash_write_buffer.buffer[flash_write_buffer.size++] = (uint8_t) capture_val_mod;

					// Send page program command to flash memory
					if (flash_write_buffer.size == FLASH_PAGE_SIZE) {
						flash_cmd_page_prog(rec_id_mapping[rec_id] + bytes_written, flash_write_buffer.buffer, FLASH_PAGE_SIZE);
						bytes_written += FLASH_PAGE_SIZE;
						flash_write_buffer.size=0;
					}
				}
			}
			else {
				rec_sr = flash_read_rec_sr();
				flash_write_rec_sr(rec_sr | (((uint16_t) 1) << rec_id));
				bytes_written = 0;
				sys_state = POST_CAPTURE;
				disable_capturing();
				display_msg();
				// TODO: display message?
			}
		}

		// Update PWM output to speaker/headphones
		OCR2A = (uint8_t) capture_val_mod;

		// Update serial transfer value (at 100 Hz)
		xfer_counter++;
		if (xfer_counter >= (SAMPLE_FREQ/XFER_FREQ)) {
			// Serial Transfer Data
			xfer_val = (uint8_t) capture_val_mod;
			xfer_flag = XFER_RDY;
			xfer_counter = 0;
		}
	}
	else if (sys_state == PLAYBACK) {
		if (bytes_read < FLASH_BLOCK_SIZE_64KB) {
			// Very first data byte or intermediate bytes within a page
			if (bytes_read == 0 || (bytes_read % FLASH_PAGE_SIZE) != 0) {
				OCR2A = flash_read_buffers[current_read_buffer].buffer[bytes_read % FLASH_PAGE_SIZE];
				bytes_read++;
			}
			// Switch read buffers and read first byte
			else {
				OCR2A = flash_read_buffers[(current_read_buffer+1) % 2].buffer[0];
				flash_cmd_read_data(rec_id_mapping[rec_id] + (next_read_page*FLASH_PAGE_SIZE), flash_read_buffers[current_read_buffer].buffer, FLASH_PAGE_SIZE);
				next_read_page++;
				current_read_buffer = (current_read_buffer+1) % 2;
				//OCR2A = flash_read_buffers[current_read_buffer].buffer[0];
				bytes_read++;
			}

			// Finished playing waveform
			if (bytes_read == FLASH_BLOCK_SIZE_64KB) {
				sys_state = POST_PLAYBACK;
				disable_playback();
				display_msg();
			}
		}

		// Update serial transfer value (at 100 Hz)
		xfer_counter++;
		if (xfer_counter >= (SAMPLE_FREQ/XFER_FREQ)) {
			// Serial Transfer Data
			xfer_val = (uint8_t) OCR2A;
			xfer_flag = XFER_RDY;
			xfer_counter = 0;
		}
	}

	profile_time2 = TCNT1;
}
