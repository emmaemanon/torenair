
#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include <stdint.h>
// #include <util/delay.h>

#include "torenair.h"
#include "statemachine.h"
#include "display.h"


// extern uint16_t systime_ms; // system state

extern volatile uint8_t sys_state; // system state

extern uint32_t year;
extern uint8_t month;
extern uint8_t day;

extern uint8_t hour;
extern uint8_t minute;
extern uint8_t second;
// extern uint16_t systime_ms;

extern uint32_t rate;
extern uint8_t relay;
extern uint8_t relay_mode;

extern volatile uint32_t sonar_result;

extern uint8_t lcd_buffer1[17];
extern uint8_t lcd_buffer2[17];

extern uint8_t butnum; // The decoded button number
extern uint8_t keynumber; // Actual numerical value of button number
extern uint8_t last_butnum; // The decoded button number
extern uint8_t last_keynumber;

void date_and_time(void) {
	sprintf((char*) lcd_buffer1, "%2d-%2d|%2d:%2d:%2d|%d", month, day, hour, minute, second, butnum%10);
	sprintf((char*) lcd_buffer2, "%4lis|%4lucm|%d %d", rate, sonar_result, relay, keynumber%10);
}

void change_state(void) {
	switch (sys_state) {
		// Idle: Go to other modes if correct buttons pressed
		case IDLE:
			// sprintf((char*) lcd_buffer1, "   |%3d|%2d:%2d   ", butnum, hour, minute);
			// sprintf((char*) lcd_buffer2, "%3d|%3d|%2d      ", last_keynumber, keynumber, second);
			date_and_time();
		break;
	default: break;
	}
}






















