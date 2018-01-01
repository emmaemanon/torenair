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
volatile uint8_t height = 0;
volatile uint8_t relay = 0;
volatile uint8_t relay_mode = 0;

volatile uint8_t key;
// volatile uint8_t key_last = 0xFF;
volatile uint16_t hr_t1;
volatile uint16_t hr_t0;
volatile uint8_t mn_t1;
volatile uint8_t mn_t0;
volatile uint8_t sc_t1;
volatile uint8_t sc_t0;
volatile uint8_t count;

volatile uint16_t delay_T1;

// volatile uint8_t dy_t;//1; //MAX 31
// volatile uint8_t h_t;//; //MAX 23
// volatile uint8_t m_t;//; //MAX 60
// volatile uint8_t s_t; //MAX 60

static void TaskDisplay(void *pvParameters); //
static void TaskKeypad(void *pvParameters); //

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

//program utama
int main (void)
{
	//DDRA = 0x00;
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	//PORTA = 0xFF;
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;
	
	TCCR1B = (1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10);//(1<<WGM12)|(1<<CS12);//(1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10); //CS12 untuk prescaler 1024, WGM12 untuk mode operasi CTC
	OCR1A = 15624;//62499; //nilai OCRA dengan F_CPU 16MHz dan untuk waktu countdown 1 detik
	TIMSK1 = (1<<OCIE1A); //mengaktifkan Timer/Counter2 Compare Match A
	sei(); //set global interrupt enable
	
    xTaskCreate(
		TaskDisplay
		,  (const portCHAR *)"RedLED" // Main Arduino Mega 2560, Freetronics EtherMega (Red) LED Blink
		,  256				// Tested 9 free @ 208
		,  NULL
		,  2
		,  NULL ); // */

    xTaskCreate(
		TaskKeypad
		,  (const portCHAR *)"GreenLED" // Main Arduino Uno 328p (Green) LED Blink
		,  256				// Tested 9 free @ 208
		,  NULL
		,  2
		,  NULL ); // */
	
	vTaskStartScheduler();
	
}

static void TaskDisplay(void *pvParameters) // Main Red LED Flash
{
    (void) pvParameters;
	
	//init uart
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
    //init lcd
    lcd_init(LCD_DISP_ON);
    lcd_led(0); //set led
	
    //lcd go home
    lcd_home();

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
		// sprintf(buf, "key_last: %d\tkey_now: %d\tcount: %d", key_last, key_now, count);
		// uart_puts(buf);
		// uart_puts("\r\n");
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


