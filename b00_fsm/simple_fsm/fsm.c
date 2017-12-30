#include "../fsm/fsm.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * implementasi finite state machine dengan software
 * @param input input dari switch, active high
 * @param output output LED, active high
 * @param state state internal FSM. belum didefinisikan
 */

void fsm(uint8_t *input,
		uint8_t *year,
		uint8_t *month,
		uint8_t *day,
		uint8_t *hour,
		uint8_t *minute,
		uint8_t *second,
		uint8_t *rate,
		uint8_t *tank_height,
		uint8_t *ultrasonic_reading,
		uint8_t *relay,
		uint8_t *relay_mode,
		uint8_t *state) {
	
	switch (*state){
		case S_HOME:
		{
			if(*input == 16 && *relay_mode == 0)
			{
				*state = S_MENU_RELAY_MANUAL;
			}
			if(*input == 16 && *relay_mode == 1)
			{
				*state = S_MENU_RELAY_AUTO;
			}
			break;
		}
		case S_MENU_RELAY_MANUAL:
		{
			if(*input == 13) //13 adalah *, artinya Tidak
			{
				*state = S_HOME;
			}
			if(*input == 15) //15 adalah #, artinya Ya
			{
				*state = S_UBAH_STAT_RELAY;
			}
			break;
		}
		case S_UBAH_STAT_RELAY:
		{
			if((*input == 9) || (*input == 10)) //13 adalah *, artinya Tidak
			{
				*state = S_PROMPT_RELAY;
			}
		}
		case S_PROMPT_RELAY:
		{
			*state = S_HOME;
		}
		case S_MENU_RELAY_AUTO:
		{
			if(*input == 13) //13 adalah *, artinya Tidak
			{
				*state = S_HOME;
			}
			if(*input == 15) //15 adalah #, artinya Ya
			{
				*state = S_PROMPT_RELAY_AUTO;
			}
			break;
		}
		case S_PROMPT_RELAY_AUTO:
		{
			*state = S_HOME;
		}
        default:
        {
        }
	}
    // perhitungan output
	switch (*state){
		case S_HOME:
		{
			*o_bank = 0;
			*o_senter = 0;
			break;
		}
		case S_MENU_RELAY_MANUAL:
		{
		}
		case S_UBAH_STAT_RELAY:
		{
		}
		case S_PROMPT_RELAY:
		{
		}
		case S_MENU_RELAY_AUTO:
		{
		}
		case S_PROMPT_RELAY_AUTO:
		{
		}
        default:
        {
        }
	}
    switch (*state) {
		case S00:
		{
			*o_bank = 0;
			*o_senter = 0;
			break;
		}
		case S01:
		{
			*o_bank = 0;
			*o_senter = 1;
			break;
		}
		case S10:
		{
			*o_bank = 1;
			*o_senter = 0;
			break;
		}
		case S11:
		{
			*o_bank = 1;
			*o_senter = 1;
			break;
		}
        default:
        {

        }
    }
}

