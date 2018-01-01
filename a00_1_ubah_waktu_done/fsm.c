
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//#include <time.h>
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
	S_MENU_WAKTU_TGL,
	S_UBAH_WKT,
	S_UBAH_WKT_H1,
	S_UBAH_WKT_H0,
	S_UBAH_WKT_M1,
	S_UBAH_WKT_M0,
	S_UBAH_WKT_S1,
	S_UBAH_WKT_S0,
	S_UBAH_TGL,
	S_PROMPT_WKT,
	S_PROMPT_TGL
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

extern volatile uint16_t year;//1; //MAX 65536
extern volatile uint8_t month;//1; //MAX 12
extern volatile uint8_t day;//1; //MAX 31
extern volatile uint8_t hour;//; //MAX 23
extern volatile uint8_t minute;//; //MAX 60
extern volatile uint8_t second; //MAX 60
extern volatile uint8_t rate;
extern volatile uint8_t height;
extern volatile uint8_t relay;
extern volatile uint8_t relay_mode;


// extern volatile uint8_t key_now;
// extern volatile uint8_t key_last;
extern volatile uint16_t hr_t1;
extern volatile uint16_t hr_t0;
extern volatile uint8_t mn_t1;
extern volatile uint8_t mn_t0;
extern volatile uint8_t sc_t1;
extern volatile uint8_t sc_t0;
extern volatile uint8_t count;
extern volatile uint16_t delay_T1;

my_states state_var = S_HOME;

void fsm(volatile uint8_t *input) {
	switch (state_var) {
	case S_HOME:
		display_main(&month, &day, &hour, &minute, &second, &rate, &height, &relay, &relay_mode);
		delay_T1 = 20;
		if(*input == KEY_B) {
			state_var = S_MENU_WAKTU_TGL;
		}
	break;
	
	case S_MENU_WAKTU_TGL:
		display_menu_wkt_tgl();
		count = 0;
		if(*input == 1) {
			state_var = S_UBAH_WKT;
		}
		if(*input == 2) {
			state_var = S_UBAH_TGL;
		}
		if(*input == KEY_str) {
			state_var = S_HOME;
		}
	break;
	
	case S_UBAH_WKT:
		count = 1;
		display_ubah_waktu();
		//if (key_now != key_last) {
		if ((*input < 16) && (count <= 1)) {
			hr_t1 = *input;
			state_var = S_UBAH_WKT_H0;
		}
		//	key_last = key_now;
		//}
		if(*input == KEY_krs) {
			state_var = S_PROMPT_WKT;
		}
		if(*input == KEY_str) {
			state_var = S_MENU_WAKTU_TGL;
		}
	break;
	
	case S_UBAH_WKT_H0:
		count = 2;
		hr_t0 = 0;
		display_ubah_waktu();
		if(*input < 16) {
			hr_t0 = *input;
			state_var = S_UBAH_WKT_M1;
		}
		if(*input == KEY_str) {
			state_var = S_UBAH_WKT;
		}
	break;
	
	case S_UBAH_WKT_M1:
		count = 3;
		mn_t1 = 0;
		display_ubah_waktu();
		if(*input < 16) {
			mn_t1 = *input;
			state_var = S_UBAH_WKT_M0;
		}
		if(*input == KEY_str) {
			state_var = S_UBAH_WKT_H0;
		}
	break;
	
	case S_UBAH_WKT_M0:
		count = 4;
		mn_t0 = 0;
		display_ubah_waktu();
		if(*input < 16) {
			mn_t0 = *input;
			state_var = S_UBAH_WKT_S1;
		}
		if(*input == KEY_str) {
			state_var = S_UBAH_WKT_M1;
		}
	break;
	case S_UBAH_WKT_S1:
		count = 5;
		sc_t1 = 0;
		display_ubah_waktu();
		if(*input < 16) {
			sc_t1 = *input;
			state_var = S_UBAH_WKT_S0;
		}
		if(*input == KEY_str) {
			state_var = S_UBAH_WKT_M0;
		}
	break;
	case S_UBAH_WKT_S0:
		count = 6;
		sc_t0 = 0;
		display_ubah_waktu();
		if(*input < 16) {
			sc_t0 = *input;
			state_var = S_UBAH_WKT;
		}
		if(*input == KEY_str) {
			state_var = S_UBAH_WKT_S1;
		}
	break;
	
	case S_UBAH_TGL:
		display_ubah_tanggal();
		if(*input == KEY_krs) {
			state_var = S_PROMPT_TGL;
		}
		if(*input == KEY_str) {
			state_var = S_MENU_WAKTU_TGL;
		}
	break;
	
	case S_PROMPT_WKT:
		hour = hr_t1*10 + hr_t0;
		minute = mn_t1*10 + mn_t0;
		second = sc_t1*10 + sc_t0;
		display_prompt_waktu();
		delay_T1 = 1000;
		state_var = S_HOME;
	break;
	
	case S_PROMPT_TGL:
		display_prompt_tgl();
		delay_T1 = 1000;
		state_var = S_HOME;
	break;
	
	default:
	break; //never happens (I hope!)
	}
}