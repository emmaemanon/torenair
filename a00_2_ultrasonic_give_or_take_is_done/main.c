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
#include "fsm.h"
#include "relay.h"
#include "ultrasonic.h"

/* Scheduler include files. */
#include "FreeRTOS/include/FreeRTOS.h"
#include "FreeRTOS/include/task.h"
#include "FreeRTOS/include/queue.h"
#include "FreeRTOS/include/semphr.h"

/* this is from avr/time.h, but it is not released into the main stream currently */
#include "FreeRTOS/include/time.h"

/* serial interface include file. */
//#include "FreeRTOS/include/serial.h"

//#define F_CPU	16000000UL

#define UART_BAUD_RATE 9600

volatile uint16_t year = 2018;//1; //MAX 65536
volatile uint8_t month = 1;//1; //MAX 12
volatile uint8_t day = 1;//1; //MAX 31
volatile uint8_t hour = 0;//; //MAX 23
volatile uint8_t minute = 0;//; //MAX 60
volatile uint8_t second = 0; //MAX 60
volatile uint8_t rate = 0;
volatile uint32_t height = 0;
volatile uint8_t relay = 1;
volatile uint8_t relay_mode = 0;

volatile uint8_t key;
volatile uint8_t count;
volatile uint16_t delay_T1;

volatile uint16_t hr_t1;
volatile uint16_t hr_t0;
volatile uint8_t mn_t1;
volatile uint8_t mn_t0;
volatile uint8_t sc_t1;
volatile uint8_t sc_t0;

volatile uint16_t yr_t3;
volatile uint16_t yr_t2;
volatile uint16_t yr_t1;
volatile uint16_t yr_t0;
volatile uint8_t mt_t1;
volatile uint8_t mt_t0;
volatile uint8_t dy_t1;
volatile uint8_t dy_t0;

// volatile uint8_t dy_t;//1; //MAX 31
// volatile uint8_t h_t;//; //MAX 23
// volatile uint8_t m_t;//; //MAX 60
// volatile uint8_t s_t; //MAX 60

// variabel untuk ultrasonik
volatile long result = 0;
volatile unsigned char up = 0;
volatile unsigned char running = 0;
volatile uint32_t timerCounter = 0;

static void TaskDisplay(void *pvParameters); //
static void TaskKeypad(void *pvParameters); //
static void TaskSonar(void *pvParameters); //

//interrupt penghitung waktu
ISR(TIMER1_COMPA_vect)
{
	//tambah detik setiap masuk interrupt
	second++;
	
	if(second > 59) //kembalikan detik ke 0 setiap 60 hitungan
	{
		second = 0;
		minute++;
	}
	if(minute > 59) //kembalikan menit ke 0 setiap 60 hitungan
	{
		minute = 0;
		hour++;
	}
	if(hour > 23) //kembalikan jam ke 0 setiap 24 hitungan
	{
		hour = 0;
		day++;
	}
	if ((month == 2) && (((year%4 == 0) && (year%100 != 0)) || (year%400 == 0)) && (day > 29)) //reset hari
	{
		day = 1;
		month++;
	}
	if ((month == 2) && ((year%4 != 0) || ((year%100 == 0) && (year%400 != 0))) && (day > 28))
	{
		day = 1;
		month++;
	}
	if (((month == 1) || (month == 3) || (month == 5) || (month == 7) || (month == 8) || (month == 10) || (month == 12)) && (day > 31))
	{
		day = 1;
		month++;
	}
	if (((month == 4) || (month == 6) || (month == 9) || (month == 11)) && (day > 30))
	{
		day = 1;
		month++;
	}
	if(month > 12) //kembalikan jam ke 0 setiap 24 hitungan
	{
		month = 1;
		year++;
	}
}

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
				height = -1; // show that measurement failed with a timeout (could return max distance here if needed)
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
	if (running) { //accept interrupts only when sonar was started
			if (up == 0) { // voltage rise, start time measurement
					up = 1;
					timerCounter = 0;
					TCNT0 = 0; // reset timer counter
			} else {
					// voltage drop, stop time measurement
					up = 0;
					// convert from time to distance(millimeters): d = [ time_s * 340m/s ] / 2 = time_us/58
					height = (timerCounter * 256 + TCNT0) / (58 * INSTR_PER_US);
					running = 0;
			}
	}
}

// void init_timer(void) {
	// TCCR1A = (1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10);//(1<<WGM12)|(1<<CS12);//(1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10); //CS12 untuk prescaler 1024, WGM12 untuk mode operasi CTC
	// OCR1A = 15624;//62499; //nilai OCRA dengan F_CPU 16MHz dan untuk waktu countdown 1 detik
	// TIMSK1 = (1<<OCIE1A); //mengaktifkan Timer/Counter2 Compare Match A
// }

//program utama
int main (void)
{
	//height = result;
	
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	PORTB = 0xFF;
	PORTC = 0xFF;
	//PORTD = 0xFF;
	
	// init_timer();
	// init_ultrasonic();
	
	cli();
	
    xTaskCreate(
		TaskDisplay
		,  (const portCHAR *)"TaskDisplay" // Main Arduino Mega 2560, Freetronics EtherMega (Red) LED Blink
		,  256				// Tested 9 free @ 208
		,  NULL
		,  3
		,  NULL ); // */

    xTaskCreate(
		TaskKeypad
		,  (const portCHAR *)"TaskKeypad" // Main Arduino Uno 328p (Green) LED Blink
		,  256				// Tested 9 free @ 208
		,  NULL
		,  2
		,  NULL ); // */
	
    xTaskCreate(
		TaskSonar
		,  (const portCHAR *)"TaskSonar" // Main Arduino Uno 328p (Green) LED Blink
		,  256				// Tested 9 free @ 208
		,  NULL
		,  3
		,  NULL ); // */
	
	vTaskStartScheduler();
}

static void TaskDisplay(void *pvParameters) // Main Red LED Flash
{
	
    (void) pvParameters;
	
	TCCR1B = (1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10);//(1<<WGM12)|(1<<CS12);//(1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10); //CS12 untuk prescaler 1024, WGM12 untuk mode operasi CTC
	OCR1A = 15624;//62499; //nilai OCRA dengan F_CPU 16MHz dan untuk waktu countdown 1 detik
	TIMSK1 = (1<<OCIE1A); //mengaktifkan Timer/Counter2 Compare Match A
	
	
    //init lcd
    lcd_init(LCD_DISP_ON);
    lcd_led(0); //set led
	
    //lcd go home
    lcd_home();
	
	sei(); //set global interrupt enable
	
	
	// //init uart
	// uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
    // //init lcd
    // lcd_init(LCD_DISP_ON);
    // lcd_led(0); //set led
	
    // //lcd go home
    // lcd_home();

    for(;;)
    {
		fsm(&key);
		vTaskDelay(delay_T1 / portTICK_PERIOD_MS);
    }

}

/*-----------------------------------------------------------*/
static void TaskKeypad(void *pvParameters) // Main Green LED Flash
{
    (void) pvParameters;
    // TickType_t xLastWakeTime;
	// /* The xLastWakeTime variable needs to be initialised with the current tick
	// count.  Note that this is the only time we access this variable.  From this
	// point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	// API function. */
	// xLastWakeTime = xTaskGetTickCount();
	
	//char buf[100];

    for(;;)
    {
		key = keymap(GetKeyPressed());	//Get the keycode of pressed key
		set_relay(&relay);
		// sprintf(buf, "key_last: %d\tkey_now: %d\tcount: %d", key_last, key_now, count);
		// uart_puts(buf);
		// uart_puts("\r\n");
    }
}

static void TaskSonar(void *pvParameters) // Main Green LED Flash
{
    (void) pvParameters;
	char bufsonar[10];
    TickType_t xLastWakeTime;
	/* The xLastWakeTime variable needs to be initialised with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();
	
	
	// ------------------- ultrasonic init code --------------------
	TRIG_DDR |= (1 << TRIG_PIN); // PD3 output - connected to Trig
	TRIG_PORT &= ~(1 << TRIG_PIN); // clear
	//ECHO_PORT &= ~(1 << ECHO_PIN); // clear
	
	// turn on interrupts for INT0, connect Echo to INT0
	EICRA |= (0 << ISC01) | (1 << ISC00); // enable interrupt on any(rising/droping) edge
	EIMSK |= (1 << INT0);      // Turns on INT0
	
	// setup 8 bit timer & enable interrupts, timer increments to 255 and interrupts on overflow
	TCCR0B = (0 << CS02) | (0 << CS01) | (1 << CS00); // select internal clock with no prescaling
	TCNT0 = 0; // reset counter to zero
	TIMSK0 = 1 << TOIE0; // enable timer interrupt
	
	
	//init uart
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
	sei();
	
	
	//char buf[100];

    for(;;)
    {
		if (running == 0) { // launch only when next iteration can happen
			// create a delay between tests, to compensate for old echoes
			vTaskDelay(DELAY_BETWEEN_TESTS_MS / portTICK_PERIOD_MS);
			//_delay_ms(DELAY_BETWEEN_TESTS_MS);
			
			TRIG_DDR &= ~(1 << TRIG_PIN);//0x00; // clear to zero for 1 us
			//delayMicroseconds(2);
			_delay_us(10);
			// vTaskDelayUntil( &xLastWakeTime, ( 1 / portTICK_PERIOD_MS ) );
			TRIG_PORT |= (1 << TRIG_PIN);//= 0x01; // set high for 10us
			running = 1;  // sonar launched
			// vTaskDelayUntil( &xLastWakeTime, ( 1 / portTICK_PERIOD_MS ) );
			//delayMicroseconds(10);
			_delay_us(10);
			TRIG_PORT &= ~(1 << TRIG_PIN); // clear 
			//ECHO_PORT &= ~(1 << ECHO_PIN); // clear
			//sonar(); // launch measurement!
		}
		sprintf(bufsonar, "%ld", height);
		uart_puts(bufsonar);
		uart_puts("\r\n");
		vTaskDelayUntil( &xLastWakeTime, ( 200 / portTICK_PERIOD_MS ) );
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


