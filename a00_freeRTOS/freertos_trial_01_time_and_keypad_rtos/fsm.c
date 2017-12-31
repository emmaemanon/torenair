
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//#include <time.h>
#include "fsm.h"

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
