
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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
	S_UBAH_WKT,
	S_UBAH_WKT_HR0,
	S_UBAH_WKT_MN1,
	S_UBAH_WKT_MN0,
	S_UBAH_WKT_SC1,
	S_UBAH_WKT_SC0,
	S_UBAH_TGL,
	S_UBAH_TGL_YR2,
	S_UBAH_TGL_YR1,
	S_UBAH_TGL_YR0,
	S_UBAH_TGL_MT1,
	S_UBAH_TGL_MT0,
	S_UBAH_TGL_DY1,
	S_UBAH_TGL_DY0,
	S_PROMPT_WKT,
	S_PROMPT_TGL,
	S_MENU_C0,
	S_MENU_D0,
	S_MENU_D1_SET_RELAY,
	S_MENU_D11_PROMPT_RELAY,
	S_MENU_D111_PROMPT_RELAY_ON,
	S_MENU_D112_PROMPT_RELAY_OFF,
	S_MENU_D2_KAL_TOREN
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

extern volatile uint16_t year;
extern volatile uint8_t month;
extern volatile uint8_t day;
extern volatile uint8_t hour;
extern volatile uint8_t minute;
extern volatile uint8_t second;
extern volatile uint8_t rate;
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

my_states state_var = S_HOME;

void fsm(volatile uint8_t *input) {
	switch (state_var) {
	case S_HOME:
		display_main(&month, &day, &hour, &minute, &second, &rate, &sonar_result, &relay, &relay_mode);
		delay_T1 = 20;
		if(*input == KEY_A) {
			state_var = S_MENU_A0;
		}
		if(*input == KEY_B) {
			state_var = S_MENU_B0;
		}
		if(*input == KEY_C) {
			state_var = S_MENU_C0;
		}
		if(*input == KEY_D) {
			state_var = S_MENU_D0;
		}
	break;
	
	case S_MENU_A0:
		// display_menu_wkt_tgl();
		// count = 0;
		// if(*input == 1) {
			// state_var = S_UBAH_WKT;
		// }
		// if(*input == 2) {
			// state_var = S_UBAH_TGL;
		// }
		if(*input == KEY_str) {
			state_var = S_HOME;
		}
	break;
	
	case S_MENU_B0:
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
		if ((*input < 16) && (count <= 1)) {
			hr_t1 = *input;
			state_var = S_UBAH_WKT_HR0;
		}
		if(*input == KEY_krs) {
			state_var = S_PROMPT_WKT;
		}
		if(*input == KEY_str) {
			state_var = S_MENU_B0;
		}
	break;
	
		case S_UBAH_WKT_HR0:
			count = 2;
			hr_t0 = 0;
			display_ubah_waktu();
			if(*input < 16) {
				hr_t0 = *input;
				state_var = S_UBAH_WKT_MN1;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_WKT;
			}
		break;
		
		case S_UBAH_WKT_MN1:
			count = 3;
			mn_t1 = 0;
			display_ubah_waktu();
			if(*input < 16) {
				mn_t1 = *input;
				state_var = S_UBAH_WKT_MN0;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_WKT_HR0;
			}
		break;
		
		case S_UBAH_WKT_MN0:
			count = 4;
			mn_t0 = 0;
			display_ubah_waktu();
			if(*input < 16) {
				mn_t0 = *input;
				state_var = S_UBAH_WKT_SC1;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_WKT_MN1;
			}
		break;
		case S_UBAH_WKT_SC1:
			count = 5;
			sc_t1 = 0;
			display_ubah_waktu();
			if(*input < 16) {
				sc_t1 = *input;
				state_var = S_UBAH_WKT_SC0;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_WKT_MN0;
			}
		break;
		case S_UBAH_WKT_SC0:
			count = 6;
			sc_t0 = 0;
			display_ubah_waktu();
			if(*input < 16) {
				sc_t0 = *input;
				state_var = S_UBAH_WKT;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_WKT_SC1;
			}
		break;
	
	case S_UBAH_TGL:
		count = 1;
		display_ubah_tanggal();
		if ((*input < 16) && (count <= 1)) {
			yr_t3 = *input;
			state_var = S_UBAH_TGL_YR2;
		}
		if(*input == KEY_krs) {
			state_var = S_PROMPT_TGL;
		}
		if(*input == KEY_str) {
			state_var = S_MENU_B0;
		}
	break;
	
		case S_UBAH_TGL_YR2:
			count = 2;
			yr_t2 = 0;
			display_ubah_tanggal();
			if(*input < 16) {
				yr_t2 = *input;
				state_var = S_UBAH_TGL_YR1;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_TGL;
			}
		break;
		
		case S_UBAH_TGL_YR1:
			count = 3;
			yr_t1 = 0;
			display_ubah_tanggal();
			if(*input < 16) {
				yr_t1 = *input;
				state_var = S_UBAH_TGL_YR0;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_TGL_YR2;
			}
		break;
		
		case S_UBAH_TGL_YR0:
			count = 4;
			yr_t0 = 0;
			display_ubah_tanggal();
			if(*input < 16) {
				yr_t0 = *input;
				state_var = S_UBAH_TGL_MT1;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_TGL_YR1;
			}
		break;
		
		case S_UBAH_TGL_MT1:
			count = 5;
			mt_t1 = 0;
			display_ubah_tanggal();
			if(*input < 16) {
				mt_t1 = *input;
				state_var = S_UBAH_TGL_MT0;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_TGL_YR0;
			}
		break;
		
		case S_UBAH_TGL_MT0:
			count = 6;
			mt_t0 = 0;
			display_ubah_tanggal();
			if(*input < 16) {
				mt_t0 = *input;
				state_var = S_UBAH_TGL_DY1;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_TGL_MT1;
			}
		break;
		
		case S_UBAH_TGL_DY1:
			count = 7;
			dy_t1 = 0;
			display_ubah_tanggal();
			if(*input < 16) {
				dy_t1 = *input;
				state_var = S_UBAH_TGL_DY0;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_TGL_MT0;
			}
		break;
		
		case S_UBAH_TGL_DY0:
			count = 8;
			dy_t0 = 0;
			display_ubah_tanggal();
			if(*input < 16) {
				dy_t0 = *input;
				state_var = S_UBAH_TGL;
			}
			if(*input == KEY_str) {
				state_var = S_UBAH_TGL_DY1;
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
		year = yr_t3*1000 + yr_t2*100 + yr_t1*10 + yr_t0;
		month = mt_t1*10 + mt_t0;
		day = dy_t1*10 + dy_t0;
		display_prompt_tgl();
		delay_T1 = 1000;
		state_var = S_HOME;
	break;
	
	case S_MENU_C0:
		// display_menu_wkt_tgl();
		// count = 0;
		// if(*input == 1) {
			// state_var = S_UBAH_WKT;
		// }
		// if(*input == 2) {
			// state_var = S_UBAH_TGL;
		// }
		if(*input == KEY_str) {
			state_var = S_HOME;
		}
	break;
	
	case S_MENU_D0:
		display_menu_D0();
		count = 0;
		if (*input == 1) {
			state_var = S_MENU_D1_SET_RELAY;
		}
		if (*input == 2) {
			state_var = S_MENU_D2_KAL_TOREN;
		}
		if (*input == KEY_str) {
			state_var = S_HOME;
		}
	break;
	
	case S_MENU_D1_SET_RELAY:
		count = 1;
		if (relay_mode == 0) {
			// display_menu_D1_set_relay();
			relay_mode = 1;
			state_var = S_MENU_D11_PROMPT_RELAY;
		}
		if(relay_mode == 1) {
			relay_mode = 0;
			// display_prompt_D1_relay_A();
			state_var = S_HOME;
		}
		if(*input == KEY_str) {
			state_var = S_MENU_D0;
		}
	break;
	
	
	case S_MENU_D11_PROMPT_RELAY:
		count = 2;
		display_menu_D1_set_relay();
		if(*input == 1) {
			relay = 1;
			state_var = S_MENU_D111_PROMPT_RELAY_ON;
		}
		if(*input == 2) {
			relay = 0;
			state_var = S_MENU_D112_PROMPT_RELAY_OFF;
		}
	break;
	
	case S_MENU_D111_PROMPT_RELAY_ON:
		count = 3;
		relay = 1;
		state_var = S_HOME;
	break;
	
	case S_MENU_D112_PROMPT_RELAY_OFF:
		count = 3;
		relay = 0;
		state_var = S_HOME;
	break;
	
	// case S_MENU_D2_KAL_TOREN:
		// count = 1;
		// if(*input == KEY_str) {
			// state_var = S_HOME;
		// }
	// break;
	
	default: state_var = S_HOME; break;
	}
}