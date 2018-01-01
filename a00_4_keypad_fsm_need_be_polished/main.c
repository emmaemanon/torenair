//MAIN CODE

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
#include "keypad.h"
#include "statemachine.h"
#include "relay.h"
#include "ultrasonic.h"
#include "torenair.h"

/* Scheduler include files. */
#include "FreeRTOS/include/FreeRTOS.h"
#include "FreeRTOS/include/task.h"
#include "FreeRTOS/include/queue.h"
#include "FreeRTOS/include/semphr.h"
#include "FreeRTOS/include/time.h"

volatile uint16_t systime_ms; // system state
volatile uint8_t sys_state = IDLE; // system state

// Task time variables
volatile uint32_t task_time_debounce = TASK_PERIOD_DEBOUNCE_MS; // time left before next button debounce (in ms)

uint32_t year = 2018;
uint8_t month = 1;
uint8_t day = 1;

uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;

uint32_t rate = 0;
uint8_t relay = 1;
uint8_t relay_mode = 0;

// variabel untuk ultrasonik
uint32_t sonar_result = 0;
uint32_t sonar_timerCounter = 0;
unsigned char sonar_up = 0;
unsigned char sonar_running = 0;

uint8_t lcd_buffer1[17] = {1};//{{0},{11111}};
uint8_t lcd_buffer2[17] = {1};//{{0},{11111}};

// Keypad scanning and debounce variables
uint8_t butnum; // The decoded button number
uint8_t keynumber; // Actual numerical value of button number
uint8_t last_butnum; // Actual numerical value of button number
uint8_t button_flag;
uint8_t last_keynumber;

static void TaskMain(void *pvParameters); //
static void TaskDisplay(void *pvParameters); //
static void TaskSonar(void *pvParameters); //

//program utama
int main (void)
{
	cli();
    xTaskCreate(
		TaskMain
		,  (const portCHAR *)"TaskMain" // Main Arduino Uno 328p (Green) LED Blink
		,  256				// Tested 9 free @ 208
		,  NULL
		,  3
		,  NULL ); // */
	
    xTaskCreate(
		TaskDisplay
		,  (const portCHAR *)"TaskDisplay" // Main Arduino Uno 328p (Green) LED Blink
		,  256				// Tested 9 free @ 208
		,  NULL
		,  3
		,  NULL ); // */
	
    xTaskCreate(
		TaskSonar
		,  (const portCHAR *)"TaskSonar" // Main Arduino Uno 328p (Green) LED Blink
		,  256				// Tested 9 free @ 208
		,  NULL
		,  1
		,  NULL ); // */
	
	vTaskStartScheduler();
}

static void TaskMain(void *pvParameters) // Main Green LED Flash
{
    (void) pvParameters;
    // TickType_t xLastWakeTime;
	// /* The xLastWakeTime variable needs to be initialised with the current tick
	// count.  Note that this is the only time we access this variable.  From this
	// point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	// API function. */
	// xLastWakeTime = xTaskGetTickCount();
	// display_msg();
	initialize();
    for(;;)
    {
		// display_lcd();
		// key = keymap(GetKeyPressed());	//Get the keycode of pressed key
				
		// Debounce keypad buttons every 30 ms
		if (task_time_debounce == 0) {
			butnum = read_keypad();	// Scan keypad
			check_button();	// Clear button state and flag if necessary
			if (butnum != NO_BUTTON_PUSHED) { // Debounce button if valid keypad state was detected
				debounce_button();
			}
			keynumber = keymap(butnum);
			task_time_debounce = TASK_PERIOD_DEBOUNCE_MS;	// Reset task
		}

		// // Check for button press
		if (button_flag == PUSHED_FLAG) {
			button_action();
		}
		change_state();
		display_msg();
		set_relay(&relay);
    }
}

static void TaskDisplay(void *pvParameters) // Main Green LED Flash
{
    (void) pvParameters;
    TickType_t xLastWakeTime;
	// /* The xLastWakeTime variable needs to be initialised with the current tick
	// count.  Note that this is the only time we access this variable.  From this
	// point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	// API function. */
	xLastWakeTime = xTaskGetTickCount();
	// initialize();
	// sprintf((char*) lcd_buffer1, "Press Any Button");
	// sprintf((char*) lcd_buffer2, "Press Any Button");
	// display_msg();
    for(;;)
    {
		// display_msg();
		vTaskDelayUntil( &xLastWakeTime, ( 30 / portTICK_PERIOD_MS ));
    }
}

static void TaskSonar(void *pvParameters) // Main Green LED Flash
{
    (void) pvParameters;
    TickType_t xLastWakeTime;
	/* The xLastWakeTime variable needs to be initialised with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();

    for(;;)
    {
		if (sonar_running == 0) { // launch only when next iteration can happen
			// create a delay between tests, to compensate for old echoes
			// vTaskDelayUntil( &xLastWakeTime, ( DELAY_BETWEEN_TESTS_MS / portTICK_PERIOD_MS ));
			_delay_ms(DELAY_BETWEEN_TESTS_MS);
			
			TRIG_DDR &= ~(1 << TRIG_PIN);//0x00; // clear to zero for 1 us
			_delay_us(5);
			TRIG_PORT |= (1 << TRIG_PIN);//= 0x01; // set high for 10us
			sonar_running = 1;  // sonar launched
			_delay_us(10);
			TRIG_PORT &= ~(1 << TRIG_PIN); // clear 
		}
		vTaskDelayUntil( &xLastWakeTime, ( 200 / portTICK_PERIOD_MS ));
    }
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    portCHAR *pcTaskName )
{
	// DDRB  |= _BV(DDB5);
	// PORTB |= _BV(PORTB5);       // main (red PB5) LED on. Arduino LED on and die.
	while(1);
}

/*-----------------------------------------------------------*/

ISR(TIMER2_COMPA_vect)
{
	systime_ms++;
	
	// // decrement debouncing task time
	if (task_time_debounce > 0) {
		task_time_debounce--;
	}
}

//interrupt penghitung waktu
ISR(TIMER1_COMPA_vect)
{
	second++;
	
	if (second > 59) {
		second = 0;
		minute++;
	}
	if (minute > 59) {
		minute = 0;
		hour++;
	}
	if (hour > 23) {
		hour = 0;
		day++;
	}
	if ((month == 2) && (((year%4 == 0) && (year%100 != 0)) || (year%400 == 0)) && (day > 29)) {
		day = 1;
		month++;
	}
	if ((month == 2) && ((year%4 != 0) || ((year%100 == 0) && (year%400 != 0))) && (day > 28)) {
		day = 1;
		month++;
	}
	if (((month == 1) || (month == 3) || (month == 5) || (month == 7) || (month == 8) || (month == 10) || (month == 12)) && (day > 31)) {
		day = 1;
		month++;
	}
	if (((month == 4) || (month == 6) || (month == 9) || (month == 11)) && (day > 30)) {
		day = 1;
		month++;
	}
	if (month > 12) {
		month = 1;
		year++;
	}
}

// timer overflow interrupt, each time when timer value passes 255 value
ISR(TIMER0_OVF_vect)
{
	if (sonar_up) {       // voltage rise was detected previously
	
		sonar_timerCounter++; // count the number of overflows
		
		// dont wait too long for the sonar end response, stop if time for measuring the distance exceeded limits
		uint32_t ticks = sonar_timerCounter * 256 + TCNT0;
		uint32_t max_ticks = (uint32_t)MAX_RESP_TIME_MS * INSTR_PER_MS; // this could be replaced with a value instead of multiplying
		if (ticks > max_ticks) {
				// timeout
				sonar_up = 0;          // stop counting timer values
				sonar_running = 0; // ultrasound scan done
				sonar_result = 0; // show that measurement failed with a timeout (could return max distance here if needed)
		}
	}
}

// interrupt for INT0 pin, to detect high/low voltage changes
/**
        We assume, that high voltage rise comes before low drop and not vice versa -
        however this should be implemented more correctly using both interrupts INT0/INT1,
        (i.e. INT0 configured for high rise, and INT0 - for low rise, thus the code must be separated also)    
*/
ISR(INT0_vect)
{
	uint32_t sonar_new;
	if (sonar_running) { //accept interrupts only when sonar was started
			if (sonar_up == 0) { // voltage rise, start time measurement
					sonar_up = 1;
					sonar_timerCounter = 0;
					TCNT0 = 0; // reset timer counter
			} else {
					// voltage drop, stop time measurement
					sonar_up = 0;
					// convert from time to distance(millimeters): d = [ time_s * 340m/s ] / 2 = time_us/58
					// sonar_result = (sonar_timerCounter * 256 + TCNT0) / (58 * INSTR_PER_US);
					sonar_new = (sonar_timerCounter * 256 + TCNT0) / (58 * INSTR_PER_US);
					if ((abs(sonar_new - sonar_result) < SONAR_DELTA_LIMIT) || (sonar_new < sonar_result)) {
						sonar_result = sonar_new;
					}
					else {
						sonar_result = sonar_result;
					}
					sonar_running = 0;
			}
	}
}

