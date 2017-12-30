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
		uint8_t *relay_mode) {
	
	switch (*state){
		case S_HOME:
		{
			if(b_long && b_baterai)
			{
				*state = S01;
			}
			else if((b_short && b_baterai) || (b_arus && b_baterai))
			{
				*state = S10;
			}
			else
			{
				*state = *state;
			}
			break;
		}
		case S_MENU_TINGGI:
		{
		}
		case S10:
		{
		}
		case S11:
		{
		}
        default:
        {
        }
	}
    // perhitungan output
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

