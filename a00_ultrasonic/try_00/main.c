//#define F_CPU 16000000
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#define UART_BAUD_RATE 9600
#include "uart/uart.h"

#define INSTR_PER_US 16                   // instructions per microsecond (depends on MCU clock, 12MHz current)
#define INSTR_PER_MS 16000                // instructions per millisecond (depends on MCU clock, 12MHz current)
#define MAX_RESP_TIME_MS 200      // timeout - max time to wait for low voltage drop (higher value increases measuring distance at the price of slower sampling)
#define DELAY_BETWEEN_TESTS_MS 50 // echo cancelling time between sampling

volatile long result = 0;
volatile unsigned char up = 0;
volatile unsigned char running = 0;
volatile uint32_t timerCounter = 0;

// timer overflow interrupt, each time when timer value passes 255 value
ISR(TIMER0_OVF_vect)
{
	if (up) {       // voltage rise was detected previously
	
		timerCounter++; // count the number of overflows
		
		// dont wait too long for the sonar end response, stop if time for measuring the distance exceeded limits
		uint32_t ticks = timerCounter * 256 + TCNT0;
		uint32_t max_ticks = (uint32_t)MAX_RESP_TIME_MS * INSTR_PER_MS; // this could be replaced with a value instead of multiplying
		if (ticks > max_ticks) {
				// timeout
				up = 0;          // stop counting timer values
				running = 0; // ultrasound scan done
				result = -1; // show that measurement failed with a timeout (could return max distance here if needed)
		}
	}
}

// interrupt for INT1 pin, to detect high/low voltage changes
/**
        We assume, that high voltage rise comes before low drop and not vice versa -
        however this should be implemented more correctly using both interrupts INT0/INT1,
        (i.e. INT0 configured for high rise, and INT1 - for low rise, thus the code must be separated also)    
*/
ISR(INT1_vect)
{
	if (running) { //accept interrupts only when sonar was started
			if (up == 0) { // voltage rise, start time measurement
					up = 1;
					timerCounter = 0;
					TCNT0 = 0; // reset timer counter
			} else {
					// voltage drop, stop time measurement
					up = 0;
					// convert from time to distance(millimeters): d = [ time_s * 340m/s ] / 2 = time_us/58
					result = (timerCounter * 256 + TCNT0) / (58 * INSTR_PER_US);
					running = 0;
			}
	}
}

/**
        Sonar interfacing:
                1. Send high impulse to Trig input for minimum 10us
                2. Sonar automatically sends eight 40kHz inpulses
                3. Sonar rises high on Echo output and then after some time drops
                   output to low (can take a while on long distances! - must include timeouts)
                4. Based on output time difference deltaT = lowT-highT calculate:
                        distance = [ deltaT * sound_speed(340m/s) ] / 2
                5. Make a delay before starting the next cycle to compensate for late echoes
*/
// generate an impulse for the Trig input (starts the sonar)
void sonar() {
        PORTD &= ~(1 << 4);//0x00; // clear to zero for 1 us
        _delay_us(1);
        PORTD |= (1 << 4);//= 0x01; // set high for 10us
        running = 1;  // sonar launched
        _delay_us(10);
        PORTD = 0x00; // clear 
}

int main(void)
{
	char buf[10];
	
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	// ------------------- ultrasonic init code --------------------
	DDRD |= (1 << PD4); // PD4 output - connected to Trig
	PORTD = 0; // clear
	
	// turn on interrupts for INT1, connect Echo to INT1
	EICRA |= (0 << ISC11) | (1 << ISC10); // enable interrupt on any(rising/droping) edge
	EIMSK |= (1 << INT1);      // Turns on INT1
	
	// setup 8 bit timer & enable interrupts, timer increments to 255 and interrupts on overflow
	TCCR0B = (0<<CS02)|(0<<CS01)|(1<<CS00); // select internal clock with no prescaling
	TCNT0 = 0; // reset counter to zero
	TIMSK0 = 1<<TOIE0; // enable timer interrupt
	
	sei(); // enable all(global) interrupts
	
	for(;;){  /* main program loop */
		// other code here...
		if (running == 0) { // launch only when next iteration can happen
			// create a delay between tests, to compensate for old echoes
			_delay_ms(DELAY_BETWEEN_TESTS_MS);
			
			sonar(); // launch measurement!
		}
		sprintf(buf, "%ld", result);
		uart_puts(buf);
		uart_puts("\r\n");
		
		_delay_ms(1000);
		// other code here...
	}
	return(0);
}