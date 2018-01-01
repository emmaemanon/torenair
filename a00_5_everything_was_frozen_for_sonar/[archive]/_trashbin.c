
/******************************************************************************
 * display.c
 *****************************************************************************/
//

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
//
	// sprintf((char*) lcd_buffer1, "Please Wait         ");
	// sprintf((char*) lcd_buffer2, "Please Wait         ");
//

void display_lcd(void);

void display_lcd(void)
{
	lcd_gotoxy(0, 0);
	LCDstring(lcd_buffer1, strlen((char*) lcd_buffer1));
	lcd_gotoxy(0, 1);
	LCDstring(lcd_buffer2, strlen((char*) lcd_buffer2));
}

// Display 1st line in LCD depending on system state
void display_line1(void) {
	// uint8_t char_offset = 0;
	lcd_gotoxy(0, 0);
	// switch(sys_state) {
		// case IDLE:				sprintf((char*) lcd_buffer, "Digital Stethoscope "); break;
		// case PRE_CAPTURE:		sprintf((char*) lcd_buffer, "Record Mode         "); break;
		// case CAPTURE:			sprintf((char*) lcd_buffer, "Recording...        "); break;
		// case POST_CAPTURE:	sprintf((char*) lcd_buffer, "Recording: Done!    "); break;
		// case DELETED:			sprintf((char*) lcd_buffer, "Delete: Done!       "); break;
		// case PRE_PLAYBACK:	sprintf((char*) lcd_buffer, "Playback Mode       "); break;
		// case PLAYBACK:			sprintf((char*) lcd_buffer, "Playing...          "); break;
		// case POST_PLAYBACK:	sprintf((char*) lcd_buffer, "Playback: Done!     "); break;
		// case VOLUME_SELECT:	sprintf((char*) lcd_buffer, "Volume Select       "); break;
		// case SETTINGS:			sprintf((char*) lcd_buffer, "Settings            "); break;
		// case FLASH_UTILITY:  sprintf((char*) lcd_buffer, "Flash Utility       "); break;
		// case ERASE_FLASH:    sprintf((char*) lcd_buffer, "Erasing Flash...    "); break;
		// case ERROR:				sprintf((char*) lcd_buffer, "Error:              "); break;
	// }
	LCDstring(lcd_buffer1, strlen((char*) lcd_buffer1));
}

// Display 2nd line in LCD depending on system state
void display_line2(void) {
	// uint8_t char_offset = 0;
	lcd_gotoxy(0, 1);
	// switch(sys_state) {
		// case IDLE:				sprintf((char*) lcd_buffer, "                    "); break;
		// case PRE_CAPTURE:		sprintf((char*) lcd_buffer, "                    "); break;
		// case CAPTURE:			sprintf((char*) lcd_buffer, "Please Wait         "); break;
		// case POST_CAPTURE:	sprintf((char*) lcd_buffer, "Stored in ID: %u     ", (uint8_t) rec_id); break;
		// case DELETED:			sprintf((char*) lcd_buffer, "                    "); break;
		// case PRE_PLAYBACK:	sprintf((char*) lcd_buffer, "                    "); break;
		// case PLAYBACK:			sprintf((char*) lcd_buffer, "Please Wait         "); break;
		// case POST_PLAYBACK:	sprintf((char*) lcd_buffer, "A: Delete   B: Play "); break;
		// case VOLUME_SELECT:	sprintf((char*) lcd_buffer, "Current Volume: %u  ", (uint8_t) cur_volume); break;
		// case SETTINGS:			sprintf((char*) lcd_buffer, "Volume: %d          ", cur_volume); break;
		// case FLASH_UTILITY:  sprintf((char*) lcd_buffer, "                    "); break;
		// case ERASE_FLASH:    sprintf((char*) lcd_buffer, "                    "); break;
		// case ERROR:				sprintf((char*) lcd_buffer, "                    "); break;
	// }
	LCDstring(lcd_buffer2, strlen((char*) lcd_buffer2));
}

/ Update and display user input based on system state and keypad button
void update_input(void) {
	// LCDGotoXY(1,3);
	// switch(sys_state) {
		// // Idle: Choose one of the features/operating modes
		// case IDLE:
			// switch (keynumber) {
				// case 1: strcpy(user_input, mode1str); LCDstring((uint8_t *) "1: Record Waveform ", 19); break;
				// case 2: strcpy(user_input, mode2str); LCDstring((uint8_t *) "2: Play Waveform   ", 19); break;
				// case 3: strcpy(user_input, mode3str); LCDstring((uint8_t *) "3: Volume Control  ", 19); break;
				// case 4: strcpy(user_input, mode4str); LCDstring((uint8_t *) "4: View Settings   ", 19); break;
				// case 5: strcpy(user_input, mode5str); LCDstring((uint8_t *) "5: Flash Utility   ", 19); break;
			// }
			// break;
		// // Pre-capture: Start recording or exit
		// case PRE_CAPTURE:
			// switch (keynumber) {
				// case SPECIAL_A: strcpy(user_input, start);   LCDstring((uint8_t *) "A: Start           ", 19); break;
				// case SPECIAL_D: strcpy(user_input, exit1);   LCDstring((uint8_t *) "D: Exit            ", 19); break;
			// }
			// break;
		// // Saved: Delete, play, or exit
		// case POST_CAPTURE:
			// switch (keynumber) {
				// case SPECIAL_A: strcpy(user_input, delete1); LCDstring((uint8_t *) "A: Delete          ", 19); break;
				// case SPECIAL_B: strcpy(user_input, play1);   LCDstring((uint8_t *) "B: Play            ", 19); break;
				// case SPECIAL_D: strcpy(user_input, exit1);   LCDstring((uint8_t *) "D: Exit            ", 19); break;
			// }
			// break;
		// // Deleted: Exit
		// case DELETED:
			// if (keynumber != ENTER) {
				// strcpy(user_input, exit2); LCDstring((uint8_t *) "Exit               ", 19);
			// }
			// break;
		// // Pre-playback: Choose a stored waveform to play
		// case PRE_PLAYBACK:
			// switch (keynumber) {
				// case 0:
					// if (is_rec_valid(0)) { strcpy(user_input, "0                  "); LCDstring((uint8_t *) "0                  ", 19); }
					// else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					// break;
				// case 1:
					// if (is_rec_valid(1)) { strcpy(user_input, "1                  "); LCDstring((uint8_t *) "1                  ", 19); }
					// else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					// break;
				// case 2:
					// if (is_rec_valid(2)) { strcpy(user_input, "2                  "); LCDstring((uint8_t *) "2                  ", 19); }
					// else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					// break;
				// case 3:
					// if (is_rec_valid(3)) { strcpy(user_input, "3                  "); LCDstring((uint8_t *) "3                  ", 19); }
					// else						{ strcpy(user_input, "                   ");	LCDstring((uint8_t *) "                   ", 19); }
					// break;
				// case 4:
					// if (is_rec_valid(4)) { strcpy(user_input, "4                  "); LCDstring((uint8_t *) "4                  ", 19); }
					// else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					// break;
				// case 5:
					// if (is_rec_valid(5)) { strcpy(user_input, "5                  "); LCDstring((uint8_t *) "5                  ", 19); }
					// else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					// break;
				// case 6:
					// if (is_rec_valid(6)) { strcpy(user_input, "6                  "); LCDstring((uint8_t *) "6                  ", 19); }
					// else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					// break;
				// case 7:
					// if (is_rec_valid(7)) { strcpy(user_input, "7                  "); LCDstring((uint8_t *) "7                  ", 19); }
					// else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					// break;
				// case 8:
					// if (is_rec_valid(8)) { strcpy(user_input, "8                  "); LCDstring((uint8_t *) "8                  ", 19); }
					// else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					// break;
				// case 9:
					// if (is_rec_valid(9)) { strcpy(user_input, "9                  "); LCDstring((uint8_t *) "9                  ", 19); }
					// else						{ strcpy(user_input, "                   "); LCDstring((uint8_t *) "                   ", 19); }
					// break;
				// case SPECIAL_D: strcpy(user_input, exit1);			 LCDstring((uint8_t *) "D: Exit            ", 19); break;
			// }
			// break;
		// // Post-playback: Delete, play again, or exit
		// case POST_PLAYBACK:
			// switch (keynumber) {
				// case SPECIAL_A: strcpy(user_input, delete1); LCDstring((uint8_t *) "A: Delete          ", 19); break;
				// case SPECIAL_B: strcpy(user_input, play1);   LCDstring((uint8_t *) "B: Play            ", 19); break;
				// case SPECIAL_D: strcpy(user_input, exit1);   LCDstring((uint8_t *) "D: Exit            ", 19); break;
			// }
			// break;
		// // Volume Select: Choose volume level or exit
		// case VOLUME_SELECT:
			// switch (keynumber) {
				// case 0:  strcpy(user_input, "0                  "); LCDstring((uint8_t *) "0                  ", 19); break;
				// case 1:  strcpy(user_input, "1                  "); LCDstring((uint8_t *) "1                  ", 19); break;
				// case 2:  strcpy(user_input, "2                  "); LCDstring((uint8_t *) "2                  ", 19); break;
				// case 3:  strcpy(user_input, "3                  "); LCDstring((uint8_t *) "3                  ", 19); break;
				// case 4:  strcpy(user_input, "4                  "); LCDstring((uint8_t *) "4                  ", 19); break;
				// case 5:  strcpy(user_input, "5                  "); LCDstring((uint8_t *) "5                  ", 19); break;
				// case 6:  strcpy(user_input, "6                  "); LCDstring((uint8_t *) "6                  ", 19); break;
				// case 7:  strcpy(user_input, "7                  "); LCDstring((uint8_t *) "7                  ", 19); break;
				// case 8:  strcpy(user_input, "8                  "); LCDstring((uint8_t *) "8                  ", 19); break;
				// case 9:  strcpy(user_input, "9                  "); LCDstring((uint8_t *) "9                  ", 19); break;
				// case SPECIAL_D: strcpy(user_input, exit1);			 LCDstring((uint8_t *) "D: Exit            ", 19); break;
			// }
			// break;
		// // Settings: Exit
		// case SETTINGS:
			// switch (keynumber) {
				// case SPECIAL_D: strcpy(user_input, exit1); LCDstring((uint8_t *) "D: Exit            ", 19); break;
			// }
			// break;
		// // Flash Utility: Erase FLASH or exit
		// case FLASH_UTILITY:
			// switch (keynumber) {
				// case SPECIAL_A: strcpy(user_input, erase); LCDstring((uint8_t *) "A: Erase Memory    ", 19); break;
				// case SPECIAL_D: strcpy(user_input, exit1); LCDstring((uint8_t *) "D: Exit            ", 19); break;
			// }
			// break;
	// }
}

extern volatile uint16_t year;
extern volatile uint8_t month;
extern volatile uint8_t day;
extern volatile uint8_t hour;
extern volatile uint8_t minute;
extern volatile uint8_t second;

extern volatile uint8_t count;

extern volatile uint16_t hr_t1;
extern volatile uint16_t hr_t0;
extern volatile uint8_t mn_t1;
extern volatile uint8_t mn_t0;
extern volatile uint8_t sc_t1;
extern volatile uint8_t sc_t0;

extern volatile uint16_t yr_t3;
extern volatile uint16_t yr_t2;
extern volatile uint16_t yr_t1;
extern volatile uint16_t yr_t0;
extern volatile uint8_t mt_t1;
extern volatile uint8_t mt_t0;
extern volatile uint8_t dy_t1;
extern volatile uint8_t dy_t0;
extern volatile uint64_t date;

extern volatile uint8_t relay_mode;

extern volatile uint32_t sonar_result;


// void display_main(volatile uint8_t *month, volatile uint8_t *day, volatile uint8_t *hour, volatile uint8_t *minute, volatile uint8_t *second, volatile uint8_t *rate, volatile uint32_t *sonar_result, volatile uint8_t *relay, volatile uint8_t *relay_mode)
// {
	// char buf1[20];
	// char buf2[20];
	// char buf_m[4];
	// char buf_r[4];
	// char buf_r_m;
	
	// uint32_t water_height;
	
	// switch(*month){
		// case 1: strcpy(buf_m, "Jan\0"); break;
		// case 2: strcpy(buf_m, "Feb\0"); break;
		// case 3: strcpy(buf_m, "Mar\0"); break;
		// case 4: strcpy(buf_m, "Apr\0"); break;
		// case 5: strcpy(buf_m, "May\0"); break;
		// case 6: strcpy(buf_m, "Jun\0"); break;
		// case 7: strcpy(buf_m, "Jul\0"); break;
		// case 8: strcpy(buf_m, "Aug\0"); break;
		// case 9: strcpy(buf_m, "Sep\0"); break;
		// case 10: strcpy(buf_m, "Oct\0"); break;
		// case 11: strcpy(buf_m, "Nov\0"); break;
		// case 12: strcpy(buf_m, "Dec\0"); break;
		// default: strcpy(buf_m, "   \0");
	// }
	// switch(*relay){
		// case 0: strcpy(buf_r, "OFF\0"); break;
		// case 1: strcpy(buf_r, " ON\0"); break;
		// default: strcpy(buf_r, "   \0");
	// }
	// switch(*relay_mode){
		// case 0: buf_r_m = 0x41; break;
		// case 1: buf_r_m = 0x4D; break;
		// default: buf_r_m = 0xFF; break;
	// }
	
	// if (*sonar_result >= (uint32_t)TANK_HEIGHT_CM) {
		// water_height = 0;
	// }
	// else {
		// water_height = (uint32_t)TANK_HEIGHT_CM - *sonar_result;
	// }
	
	// //taruh parameter
	// sprintf(buf1, "%c%c%c-%2d|%2d:%2d:%2d%c", buf_m[0], buf_m[1], buf_m[2], *day, *hour, *minute, *second, buf_r_m);
	// sprintf(buf2, "%3ds|%4lucm|%c%c%c%c", *rate, (unsigned long)water_height, buf_r[0], buf_r[1], buf_r[2], buf_r_m);
	
	// lcd_gotoxy(0, 0);
	// lcd_puts(buf1);
	// // uart_puts(buf1);
	// // uart_puts("\r\n");
	
	// lcd_gotoxy(0, 1);
	// lcd_puts(buf2);
	// // uart_puts(buf2);
	// // uart_puts("\r\n");
// }

// void display_B0_date(void)
// {
	// char buf1[20];
	// char buf2[20];
	
	// sprintf(buf1, "YYYY - MM - DD %d", count);
	// sprintf(buf2, "%8ld |", (long)date);
	
	// lcd_gotoxy(0, 0);
	// lcd_puts(buf1);
	
	// lcd_gotoxy(0, 1);
	// lcd_puts(buf2);
// }


/******************************************************************************
 * display.h
 *****************************************************************************/
void display_lcd(char buf1, char buf2);
void display_main(volatile uint8_t *month,volatile uint8_t *day,volatile uint8_t *hour,volatile uint8_t *minute,volatile uint8_t *second,volatile uint8_t *rate,volatile uint32_t *sonar_result,volatile uint8_t *relay,volatile uint8_t *relay_mode);
void display_B0_date(void);



/******************************************************************************
 * fsm.c
 *****************************************************************************/
//
// void fsm(volatile uint8_t *input);
// void fsm(volatile uint8_t *input, volatile TickType_t xLastWakeTime_T1);


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <util/delay.h>
#include "fsm.h"
#include "display.h"

/* Scheduler include files. */
#include "FreeRTOS/include/FreeRTOS.h"
#include "FreeRTOS/include/task.h"
#include "FreeRTOS/include/queue.h"
#include "FreeRTOS/include/semphr.h"

/* this is from avr/time.h, but it is not released into the main stream currently */
#include "FreeRTOS/include/time.h"

typedef enum {
	S_HOME,
	S_MENU_A0,
	S_MENU_B0,
	S_MENU_B1
} my_states;

//	S_MENU_TINGGI,
//	S_TINGGI_TR,
//	S_TINGGI_AIR,
//	S_PROMPT_TR	,
//	S_PROMPT_AIR,
//	S_MENU_LOG,
//	S_PROMPT_LOG,
//	S_MENU_RELAY_MANUAL,
//	S_UBAH_STAT_RELAY,
//	S_PROMPT_RELAY,
//	S_MENU_RELAY_AUTO,
//	S_PROMPT_RELAY_AUTO

extern volatile uint32_t year;
extern volatile uint8_t month;
extern volatile uint8_t day;
extern volatile uint8_t hour;
extern volatile uint8_t minute;
extern volatile uint8_t second;
extern volatile uint32_t rate;
extern volatile uint8_t relay;
extern volatile uint8_t relay_mode;

extern volatile uint8_t count;
extern volatile uint16_t delay_T1;

extern volatile uint16_t hr_t1;
extern volatile uint16_t hr_t0;
extern volatile uint8_t mn_t1;
extern volatile uint8_t mn_t0;
extern volatile uint8_t sc_t1;
extern volatile uint8_t sc_t0;

extern volatile uint16_t yr_t3;
extern volatile uint16_t yr_t2;
extern volatile uint16_t yr_t1;
extern volatile uint16_t yr_t0;
extern volatile uint8_t mt_t1;
extern volatile uint8_t mt_t0;
extern volatile uint8_t dy_t1;
extern volatile uint8_t dy_t0;

extern volatile uint32_t sonar_result;
extern volatile uint64_t date;
// extern volatile uint8_t key;

// extern volatile char buf1[20];
// extern volatile char buf2[20];

extern uint8_t lcd_buffer1[17];
extern uint8_t lcd_buffer2[17];

my_states state_var = S_HOME;

void print_lcd_home(void) {
	sprintf((char*) lcd_buffer1, "%2d-%2d|%2d:%2d:%2d %c", month, day, hour, minute, second, relay_mode);
	sprintf((char*) lcd_buffer2, "%4lis|%4licm|%d %c", rate, sonar_result, relay, relay_mode);
}

void fsm(volatile uint8_t *input) {
	switch (state_var) {
	case S_HOME:
		count = 0;
		date = 0;
		print_lcd_home();
		display_lcd();
		delay_T1 = 20;
		if(*input == KEY_A) {
			state_var = S_MENU_A0;
		}
		if(*input == KEY_B) {
			state_var = S_MENU_B0;
		}
		// if(*input == KEY_C) {
			// state_var = S_MENU_C0;
		// }
		// if(*input == KEY_D) {
			// state_var = S_MENU_D0;
		// }
	break;
	
	// case S_MENU_B0:
		// display_B0_date();
		
		// if(old_key != key) {
			// if(*input <= 16) {
				// if (count == 0) {
					// date = *input;
				// }
				// state_var = S_MENU_B1;
				// if (count == 8) {
					// state_var = S_HOME;
				// }
			// }
			// old_key = key;
		// }
		// else if(*input == KEY_str) {
			// state_var = S_HOME;
		// }
		// else
	// break;
	
	// case S_MENU_B1:
		// count++;
		// display_B0_date();
		// if(old_key != key) {
			// if(*input <= 16) {
				// if (count > 0) {
					// date = (date << 8*count) | *input;
				// }
				// if (count == 8) {
					// state_var = S_HOME;
				// }
			// }
			// old_key = key;
		// }
		// else if(*input == KEY_str) {
			// state_var = S_HOME;
		// }
		// else
	// break;
	
	default: state_var = S_HOME; break;
	}
}


/******************************************************************************
 * main.c
 *****************************************************************************/
//

/* this is from avr/time.h, but it is not released into the main stream currently */

/* serial interface include file. */
//#include "FreeRTOS/include/serial.h"

// volatile uint32_t systime; // system time

// volatile uint32_t task_time_serial_xfer = TASK_PERIOD_SERIAL_XFER_MS;	// time left before next byte of data is transfered (in 

volatile uint16_t hr_t1;
volatile uint16_t hr_t0;
volatile uint8_t mn_t1;
volatile uint8_t mn_t0;
volatile uint8_t sc_t1;
volatile uint8_t sc_t0;

volatile uint16_t yr_t3;
volatile uint16_t yr_t2;
volatile uint16_t yr_t1;
volatile uint16_t yr_t0;
volatile uint8_t mt_t1;
volatile uint8_t mt_t0;
volatile uint8_t dy_t1;
volatile uint8_t dy_t0;
volatile uint64_t date;

// volatile uint8_t key;
volatile uint8_t count;
volatile uint16_t delay_T1;


// Keypad scanning and debounce variables
uint8_t button_state;
uint8_t button_flag;
unsigned char key;				// The raw keyscan
unsigned char butnum;			// The decoded button number
unsigned char last_butnum;		// Last stored keypad number
uint8_t keynumber;				// Actual numerical value of button number
uint8_t last_keynumber;
uint8_t edit_state = EDIT_ON;	// Enable user input
char user_input[17] = "               ";
// uint8_t keytbl[16]={ 0xee, 0xed, 0xeb, 0xe7,
							// 0xde, 0xdd, 0xdb, 0xd7,
						   // 0xbe, 0xbd, 0xbb, 0xb7,
						   // 0x7e, 0x7d, 0x7b, 0x77};
// uint8_t keymap[16]={ 1, 2,  3, 12,
							// 4, 5,  6, 13,
							// 7, 8,  9, 14,
						  // 10, 0, 11, 15};
// asdf

//
    xTaskCreate(
		TaskDisplay
		,  (const portCHAR *)"TaskDisplay" // Main Arduino Mega 2560, Freetronics EtherMega (Red) LED Blink
		,  256				// Tested 9 free @ 208
		,  NULL
		,  3
		,  NULL ); // */

 
static void TaskDisplay(void *pvParameters) // Main Red LED Flash
{
	
    (void) pvParameters;
	
	
    for(;;)
    {
		display_msg();
		// fsm(&key);
		// _delay_ms(delay_T1);
		vTaskDelay( 20 / portTICK_PERIOD_MS);
    }

}

/*-----------------------------------------------------------*/


/******************************************************************************
 * statemachine.c
 *****************************************************************************/
 //
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <util/delay.h>
#include "statemachine.h"
#include "display.h"

/* Scheduler include files. */
#include "FreeRTOS/include/FreeRTOS.h"
#include "FreeRTOS/include/task.h"
#include "FreeRTOS/include/queue.h"
#include "FreeRTOS/include/semphr.h"

/* this is from avr/time.h, but it is not released into the main stream currently */
#include "FreeRTOS/include/time.h"

typedef enum {
	S_HOME,
	S_MENU_A0,
	S_MENU_B0,
	S_MENU_B1
} my_states;

//	S_MENU_TINGGI,
//	S_TINGGI_TR,
//	S_TINGGI_AIR,
//	S_PROMPT_TR	,
//	S_PROMPT_AIR,
//	S_MENU_LOG,
//	S_PROMPT_LOG,
//	S_MENU_RELAY_MANUAL,
//	S_UBAH_STAT_RELAY,
//	S_PROMPT_RELAY,
//	S_MENU_RELAY_AUTO,
//	S_PROMPT_RELAY_AUTO

extern volatile uint32_t year;
extern volatile uint8_t month;
extern volatile uint8_t day;
extern volatile uint8_t hour;
extern volatile uint8_t minute;
extern volatile uint8_t second;
extern volatile uint32_t rate;
extern volatile uint8_t relay;
extern volatile uint8_t relay_mode;

extern volatile uint8_t count;
extern volatile uint16_t delay_T1;

extern volatile uint16_t hr_t1;
extern volatile uint16_t hr_t0;
extern volatile uint8_t mn_t1;
extern volatile uint8_t mn_t0;
extern volatile uint8_t sc_t1;
extern volatile uint8_t sc_t0;

extern volatile uint16_t yr_t3;
extern volatile uint16_t yr_t2;
extern volatile uint16_t yr_t1;
extern volatile uint16_t yr_t0;
extern volatile uint8_t mt_t1;
extern volatile uint8_t mt_t0;
extern volatile uint8_t dy_t1;
extern volatile uint8_t dy_t0;

extern volatile uint32_t sonar_result;
extern volatile uint64_t date;
// extern volatile uint8_t key;

// extern volatile char buf1[20];
// extern volatile char buf2[20];

extern uint8_t lcd_buffer1[17];
extern uint8_t lcd_buffer2[17];

my_states state_var = S_HOME;

void print_lcd_home(void) {
	sprintf((char*) lcd_buffer1, "%2d-%2d|%2d:%2d:%2d %c", month, day, hour, minute, second, relay_mode);
	sprintf((char*) lcd_buffer2, "%4lis|%4licm|%d %c", rate, sonar_result, relay, relay_mode);
}

void change_state(void) {
	
}


	sprintf((char*) lcd_buffer1, "%2d-%2d|%2d:%2d:%2d.%d", month, day, hour, minute, second, systime_ms/100);


/******************************************************************************
 * keypad.c
 *****************************************************************************/
//

// uint8_t keymap[16]={ 1, 2,  3, 12,
							// 4, 5,  6, 13,
							// 7, 8,  9, 14,
						  // 10, 0, 11, 15};


/******************************************************************************
 * ___.c
 *****************************************************************************/
//
