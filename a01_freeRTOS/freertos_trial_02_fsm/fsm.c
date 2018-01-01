
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

extern volatile uint16_t delay_T1;
volatile unsigned long ul;

my_states state_var = S_HOME;

void fsm(volatile uint8_t *input, volatile unsigned long *ul) {
	switch (state_var) {
	case S_HOME:
		display_main(&month, &day, &hour, &minute, &second, &rate, &height, &relay);
		delay_T1 = 20;
		if(*input == KEY_B) {
			state_var = S_MENU_WAKTU_TGL;
		}
	break;
	
	case S_MENU_WAKTU_TGL:
		//kode display di sini
		display_menu_wkt_tgl();
		delay_T1 = 20;
		
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
		//kode display di sini
		display_ubah_waktu();
		delay_T1 = 20;
		
		if(*input == KEY_krs) {
			state_var = S_PROMPT_WKT;
		}
		if(*input == KEY_str) {
			state_var = S_MENU_WAKTU_TGL;
		}
	break;
	
	case S_UBAH_TGL:
		//kode display di sini
		display_ubah_tanggal();
		delay_T1 = 20;
		
		if(*input == KEY_krs) {
			state_var = S_PROMPT_TGL;
		}
		if(*input == KEY_str) {
			state_var = S_MENU_WAKTU_TGL;
		}
	break;
	
	case S_PROMPT_WKT:
		display_prompt_waktu();
		delay_T1 = 3000;
		state_var = S_HOME;
	break;
	
	case S_PROMPT_TGL:
		display_prompt_tgl();
		delay_T1 = 3000;
		
		/* Delay for a period. */
		for( *ul = 0; *ul < 0x0f; *ul++ )
		{
			/* This loop is just a very crude delay implementation. There is
			nothing to do in here. Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
		
        //printf("DEACTIVATING\n");
		//delay(1000);
		state_var = S_HOME;
	break;
	
	default:
	break; //never happens (I hope!)
	}
}