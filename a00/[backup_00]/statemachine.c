
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "torenair.h"
#include "statemachine.h"
#include "display.h"
#include "keypad.h"


// extern uint16_t systime_ms; // system state

extern volatile uint8_t sys_state; // system state

extern uint32_t year;
extern uint8_t month;
extern uint8_t day;

extern uint8_t hour;
extern uint8_t minute;
extern uint8_t second;
// extern uint16_t systime_ms;

extern volatile uint16_t rate;
extern uint8_t relay;
extern uint8_t relay_mode;
uint8_t relay_set_manual;

// variabel untuk tanki
extern volatile uint16_t sonar_result;
extern uint16_t tank_height;
extern uint16_t water_height;
uint16_t water_height_max;

// untuk display
extern uint8_t lcd_buffer1[17];
extern uint8_t lcd_buffer2[17];

// untuk keypad
extern uint8_t butnum; // The decoded button number
extern uint8_t keynumber; // Actual numerical value of button number
extern uint8_t last_butnum; // The decoded button number
extern uint8_t last_keynumber;
extern uint8_t button_flag;
extern uint8_t button_state;

// untuk state machine
unsigned char count;
uint8_t input[8];

void date_and_time(void) {
	if (sonar_result <= tank_height) {
			water_height = tank_height - sonar_result;
	}
	else {
		water_height = tank_height;
	}
	if (((relay_mode == 0) && (water_height < water_height_max)) || ((relay_mode == 1) && (relay_set_manual == 1))) {
		relay = 1;
	}
	else {
		relay = 0;
	}
	count = 0;
	input[0] = 0; input[1] = 0; input[2] = 0; input[3] = 0;
	input[4] = 0; input[5] = 0; input[6] = 0; input[7] = 0;
	// sprintf((char*) lcd_buffer1, "%3d-%3d|bf: %2d  ", last_butnum, butnum, button_flag);
	// sprintf((char*) lcd_buffer2, "%3d-%3d|bS: %2d  ", last_keynumber, keynumber, button_state); 
	// date = 0x00000000;
	sprintf((char*) lcd_buffer1, "%2d-%2d|%2d:%2d:%2d|%d", month, day, hour, minute, second, button_flag);
	sprintf((char*) lcd_buffer2, "%4ds|%4ucm|%d %d", rate, (unsigned)water_height, relay, relay_mode);
}

void hidden_par(void) {
	sprintf((char*) lcd_buffer1, "%4lu | Wtr: %4u", (long unsigned)year, (unsigned)water_height_max);
	sprintf((char*) lcd_buffer2, "Tor: %4u cm    ", (unsigned)tank_height);
}

// void pre_change_date(void) {
	// sprintf((char*) lcd_buffer1, "Ubah Tanggal :  ");
	// sprintf((char*) lcd_buffer2, "%d%d%d%d            ", input[0], input[1], input[2], input[3]);
	// static int i = 0;
	// for (i = 0; i < 8; i++) {
		
	// }
	//sprintf((char*) lcd_buffer2, "%", (unsigned)tank_height);
// }

void change_state(void) {
	switch (sys_state) {
		// Idle: Go to other modes if correct buttons pressed
		case DEFAULT:
			date_and_time();
			if ((keynumber == KEY_str) && (button_state == MAYBE_PUSH_STATE)) { sys_state = HIDDENPAR; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGEDATE_Y3; }
			if ((keynumber == KEY_B) && (button_state == MAYBE_PUSH_STATE)) { sys_state = PRE_WATERMAX_W3; }
			if ((keynumber == KEY_C) && (button_state == MAYBE_PUSH_STATE)) { sys_state = PRE_WATERCAL; }
			if ((keynumber == KEY_D) && (button_state == MAYBE_PUSH_STATE)) { sys_state = PRE_MODERELAY; }
		break;
		case HIDDENPAR:
			hidden_par();
			if (((button_flag == PUSHED_FLAG) && (keynumber != KEY_str)) || ((keynumber == NO_BUTTON_PUSHED) && (button_flag == NOT_PUSHED_FLAG))) { sys_state = DEFAULT; }
		break;
		case CHANGEDATE_Y3:
			sprintf((char*) lcd_buffer1, "Ubah Tanggal :  ");
			sprintf((char*) lcd_buffer2, "%d%d%d%d - %d%d - %d%d  ", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = CHANGEDATE_Y2;
				count++;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGETIME_h1; }
		break;
		case CHANGEDATE_Y2:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d%d%d - %d%d - %d%d  ", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = CHANGEDATE_Y1;
				count++;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGETIME_h1; }
		break;
		case CHANGEDATE_Y1:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d%d%d - %d%d - %d%d  ", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = CHANGEDATE_Y0;
				count++;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGETIME_h1; }
		break;
		case CHANGEDATE_Y0:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d%d%d - %d%d - %d%d  ", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = CHANGEDATE_M1;
				count++;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGETIME_h1; }
		break;
		case CHANGEDATE_M1:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d%d%d - %d%d - %d%d  ", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = CHANGEDATE_M0;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGETIME_h1; }
		break;
		case CHANGEDATE_M0:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d%d%d - %d%d - %d%d  ", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = CHANGEDATE_D1;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGETIME_h1; }
		break;
		case CHANGEDATE_D1:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d%d%d - %d%d - %d%d  ", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = CHANGEDATE_D0;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGETIME_h1; }
		break;
		case CHANGEDATE_D0:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d%d%d - %d%d - %d%d  ", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = POST_CHANGEDATE;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; };
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGETIME_h1; }
		break;
		case POST_CHANGEDATE:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d%d%d - %d%d - %d%d ?", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7]);
			if ((keynumber == KEY_krs) && (button_state == MAYBE_PUSH_STATE)) {
				year = input[0]*1000 + input[1]*100 + input[2]*10 + input[3];
				month = input[4]*10 + input[5];
				day = input[6]*10 + input[7];
				sys_state = DEFAULT;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGETIME_h1; }
		break;
		case CHANGETIME_h1:
			input[0] = 0; input[1] = 0; input[2] = 0; input[3] = 0;
			input[4] = 0; input[5] = 0;
			sprintf((char*) lcd_buffer1, "Ubah Waktu :    ");
			sprintf((char*) lcd_buffer2, "%d%d - %d%d - %d%d    ", input[0], input[1], input[2], input[3], input[4], input[5]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = CHANGETIME_h0;
				count++;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = DEFAULT; }
		break;
		case CHANGETIME_h0:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d - %d%d - %d%d    ", input[0], input[1], input[2], input[3], input[4], input[5]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = CHANGETIME_m1;
				count++;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = DEFAULT; }
		break;
		case CHANGETIME_m1:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d - %d%d - %d%d    ", input[0], input[1], input[2], input[3], input[4], input[5]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = CHANGETIME_m0;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = DEFAULT; }
		break;
		case CHANGETIME_m0:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d - %d%d - %d%d    ", input[0], input[1], input[2], input[3], input[4], input[5]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = CHANGETIME_d1;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = DEFAULT; }
		break;
		case CHANGETIME_d1:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d - %d%d - %d%d    ", input[0], input[1], input[2], input[3], input[4], input[5]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = CHANGETIME_d0;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = DEFAULT; }
		break;
		case CHANGETIME_d0:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d - %d%d - %d%d    ", input[0], input[1], input[2], input[3], input[4], input[5]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = POST_CHANGETIME;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = DEFAULT; }
		break;
		case POST_CHANGETIME:
			// sprintf((char*) lcd_buffer1, "Ubah Tanggal : %d", count);
			sprintf((char*) lcd_buffer2, "%d%d - %d%d - %d%d ?  ", input[0], input[1], input[2], input[3], input[4], input[5]);
			if ((keynumber == KEY_krs) && (button_state == MAYBE_PUSH_STATE)) {
				hour = input[0]*10 + input[1];
				minute = input[2]*10 + input[3];
				second = input[4]*10 + input[5];
				sys_state = DEFAULT;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = DEFAULT; }
		break;
		case PRE_WATERMAX_W3:
			sprintf((char*) lcd_buffer1, "Set Tinggi Max  ");
			sprintf((char*) lcd_buffer2, "Air ke %d%d%d%d cm.-", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = PRE_WATERMAX_W2;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
			if ((keynumber == KEY_B) && (button_state == MAYBE_PUSH_STATE)) { sys_state = CHANGERATE_R3; }
		break;
		case PRE_WATERMAX_W2:
			sprintf((char*) lcd_buffer2, "Air ke %d%d%d%d cm.-", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = PRE_WATERMAX_W1;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case PRE_WATERMAX_W1:
			sprintf((char*) lcd_buffer2, "Air ke %d%d%d%d cm.-", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = PRE_WATERMAX_W0;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case PRE_WATERMAX_W0:
			sprintf((char*) lcd_buffer2, "Air ke %d%d%d%d cm.-", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = POST_WATERMAX;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case POST_WATERMAX:
			sprintf((char*) lcd_buffer2, "Air ke %d%d%d%d cm ?", input[0], input[1], input[2], input[3]);
			if ((keynumber == KEY_krs) && (button_state == MAYBE_PUSH_STATE)) {
				water_height_max = input[0]*1000 + input[1]*100 + input[2]*10 + input[3];
				sys_state = DEFAULT;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
	////////////////////////////////////////////////////////////////////
		case CHANGERATE_R3:
			sprintf((char*) lcd_buffer1, "Rate Transf Data");
			sprintf((char*) lcd_buffer2, "per %d%d%d%d detik  ", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = CHANGERATE_R2;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case CHANGERATE_R2:
			sprintf((char*) lcd_buffer2, "per %d%d%d%d detik  ", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = CHANGERATE_R1;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case CHANGERATE_R1:
			sprintf((char*) lcd_buffer2, "per %d%d%d%d detik  ", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = CHANGERATE_R0;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case CHANGERATE_R0:
			sprintf((char*) lcd_buffer2, "per %d%d%d%d detik  ", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = POST_CHANGERATE;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case POST_CHANGERATE:
			sprintf((char*) lcd_buffer2, "per %d%d%d%d detik ?", input[0], input[1], input[2], input[3]);
			if ((keynumber == KEY_krs) && (button_state == MAYBE_PUSH_STATE)) {
				rate = input[0]*1000 + input[1]*100 + input[2]*10 + input[3];
				sys_state = DEFAULT;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
	///////////////////////////////////////////////////////////////////////////
		case PRE_WATERCAL:
			sprintf((char*) lcd_buffer1, "Kalibrasi Tinggi");
			sprintf((char*) lcd_buffer2, "Air?------------");
			if ((keynumber == KEY_krs) && (button_state == MAYBE_PUSH_STATE)) {
				sonar_result = tank_height;
				sys_state = DEFAULT;
			}
			if ((keynumber == KEY_C) && (button_state == MAYBE_PUSH_STATE)) { sys_state = PRE_TANKHEIGHT_X3; };
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case PRE_TANKHEIGHT_X3:
			sprintf((char*) lcd_buffer1, "Ubah Tinggi Tank");
			sprintf((char*) lcd_buffer2, "ke %d%d%d%d cm.-----", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = PRE_TANKHEIGHT_X2;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case PRE_TANKHEIGHT_X2:
			sprintf((char*) lcd_buffer2, "ke %d%d%d%d cm.-----", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = PRE_TANKHEIGHT_X1;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case PRE_TANKHEIGHT_X1:
			sprintf((char*) lcd_buffer2, "ke %d%d%d%d cm.-----", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				count++;
				sys_state = PRE_TANKHEIGHT_X0;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case PRE_TANKHEIGHT_X0:
			sprintf((char*) lcd_buffer2, "ke %d%d%d%d cm.-----", input[0], input[1], input[2], input[3]);
			if ((keynumber < 10) && (button_state == MAYBE_PUSH_STATE)) {
				input[count] = keynumber;
				sys_state = POST_TANKHEIGHT;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; }
		break;
		case POST_TANKHEIGHT:
			sprintf((char*) lcd_buffer2, "ke %d%d%d%d cm ?----", input[0], input[1], input[2], input[3]);
			if ((keynumber == KEY_krs) && (button_state == MAYBE_PUSH_STATE)) {
				tank_height = input[0]*1000 + input[1]*100 + input[2]*10 + input[3];
				sys_state = DEFAULT;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; };
			if ((keynumber == KEY_A) && (button_state == MAYBE_PUSH_STATE)) { sys_state = DEFAULT; }
		break;
		case PRE_MODERELAY:
			if (relay_mode == 0) {
				// sprintf((char*) lcd_buffer1, "#-Ubah dari A>M?");
				// sprintf((char*) lcd_buffer2, "D-Mode M: ON/OFF");
				// if ((keynumber == KEY_krs)) {
					// relay_mode = 1;
					sys_state = MODERELAY_A2M;
				// }
			}
			if (relay_mode == 1) {
				sprintf((char*) lcd_buffer1, "#-Ubah dari M>A?");
				sprintf((char*) lcd_buffer2, "D-Mode M: ON/OFF");
				if ((keynumber == KEY_krs)) {
					relay_mode = 0;
					sys_state = DEFAULT;
				}
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; };
			if ((keynumber == KEY_D) && (button_state == MAYBE_PUSH_STATE)) { sys_state = MODERELAY_M; }
		break;
		case MODERELAY_A2M:
			sprintf((char*) lcd_buffer1, "#-Ubah dari A>M?");
			sprintf((char*) lcd_buffer2, "D-Mode M: ON/OFF");
			if ((keynumber == KEY_krs) && (button_state == MAYBE_PUSH_STATE)) {
				relay_mode = 1;
				sys_state = MODERELAY_M;
			}
			if ((keynumber == KEY_D) && (button_state == MAYBE_PUSH_STATE)) { sys_state = MODERELAY_M; }
			if (keynumber == KEY_str) { sys_state = DEFAULT; };
		break;
		case MODERELAY_M:
			sprintf((char*) lcd_buffer1, "1-Mode M: ON    ");
			sprintf((char*) lcd_buffer2, "0-Mode M: OFF   ");
			if ((keynumber == 1) && (button_state == MAYBE_PUSH_STATE)) {
				relay_set_manual = 1;
				relay_mode = 1;
				sys_state = DEFAULT;
			}
			if ((keynumber == 0) && (button_state == MAYBE_PUSH_STATE)) {
				relay_set_manual = 0;
				relay_mode = 1;
				sys_state = DEFAULT;
			}
			if (keynumber == KEY_str) { sys_state = DEFAULT; };
		break;
	default: break;
	}
}






















