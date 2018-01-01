
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <avr/io.h>
// #include <avr/wdt.h>
// #include <avr/interrupt.h>  /* for sei() */
// #include <util/delay.h>     /* for _delay_ms() */

// #include "ultrasonic.h"

// //extern volatile unsigned char running;
// extern volatile uint32_t height;

// generate an impulse for the Trig output (starts the sonar)
// void sonar(void) {
        // TRIG_DDR &= ~(1 << TRIG_PIN);//0x00; // clear to zero for 1 us
        // _delay_us(1);
        // TRIG_PORT |= (1 << TRIG_PIN);//= 0x01; // set high for 10us
        // height = 1;  // sonar launched
        // _delay_us(10);
        // TRIG_PORT &= ~(1 << TRIG_PIN); // clear 
		// //ECHO_PORT &= ~(1 << ECHO_PIN); // clear
// }

// void init_ultrasonic(void) {
	// //uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	// // ------------------- ultrasonic init code --------------------
	// TRIG_DDR |= (1 << TRIG_PIN); // PD3 output - connected to Trig
	// TRIG_PORT &= ~(1 << TRIG_PIN); // clear
	// //ECHO_PORT &= ~(1 << ECHO_PIN); // clear
	
	// // turn on interrupts for INT0, connect Echo to INT0
	// EICRA |= (0 << ISC01) | (1 << ISC00); // enable interrupt on any(rising/droping) edge
	// EIMSK |= (1 << INT0);      // Turns on INT0
	
	// // setup 8 bit timer & enable interrupts, timer increments to 255 and interrupts on overflow
	// TCCR0B = (0 << CS02) | (0 << CS01) | (1 << CS00); // select internal clock with no prescaling
	// TCNT0 = 0; // reset counter to zero
	// TIMSK0 = 1 << TOIE0; // enable timer interrupt
// }

// int main(void)
// {
	// char buf[10];
	
	// uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	// // ------------------- ultrasonic init code --------------------
	// DDRD |= (1 << PD4); // PD4 output - connected to Trig
	// PORTD = 0; // clear
	
	// // turn on interrupts for INT1, connect Echo to INT1
	// EICRA |= (0 << ISC11) | (1 << ISC10); // enable interrupt on any(rising/droping) edge
	// EIMSK |= (1 << INT1);      // Turns on INT1
	
	// // setup 8 bit timer & enable interrupts, timer increments to 255 and interrupts on overflow
	// TCCR0B = (0<<CS02)|(0<<CS01)|(1<<CS00); // select internal clock with no prescaling
	// TCNT0 = 0; // reset counter to zero
	// TIMSK0 = 1<<TOIE0; // enable timer interrupt
	
	// sei(); // enable all(global) interrupts
	
	// for(;;){  /* main program loop */
		// // other code here...
		// if (running == 0) { // launch only when next iteration can happen
			// // create a delay between tests, to compensate for old echoes
			// _delay_ms(DELAY_BETWEEN_TESTS_MS);
			
			// sonar(); // launch measurement!
		// }
		// sprintf(buf, "%ld", result);
		// uart_puts(buf);
		// uart_puts("\r\n");
		
		// _delay_ms(1000);
		// // other code here...
	// }
	// return(0);
// }