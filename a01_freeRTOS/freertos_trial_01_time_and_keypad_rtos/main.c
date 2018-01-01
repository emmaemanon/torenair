//TIME STAMPER

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

volatile uint16_t year = 2017;//1; //MAX 65536
volatile uint8_t month = 12;//1; //MAX 12
volatile uint8_t day = 31;//1; //MAX 31
volatile uint8_t hour = 6;//; //MAX 23
volatile uint8_t minute = 16;//; //MAX 60
volatile uint8_t second = 4; //MAX 60
volatile uint8_t rate = 0;
volatile uint8_t height = 0;
volatile uint8_t relay = 0;

static void TaskDisplay(void *pvParameters); // Main Arduino Mega 2560, Freetronics EtherMega (Red) LED Blink

static void TaskKeypad(void *pvParameters); // Main Arduino Uno 328p (Green) LED Blink

//interrupt penghitung waktu
ISR(TIMER1_COMPA_vect)
{
	//tambah detik setiap masuk interrupt
	second++;
	
	if(second == 60) //kembalikan detik ke 0 setiap 60 hitungan
	{
		second = 0;
		minute++;
	}
	if(minute == 60) //kembalikan menit ke 0 setiap 60 hitungan
	{
		minute = 0;
		hour++;
	}
	if(hour == 24) //kembalikan jam ke 0 setiap 24 hitungan
	{
		hour = 0;
		day++;
	}
	if ((month == 2) && (((year%4 == 0) && (year%100 != 0)) || (year%400 == 0)) && (day == 30)) //reset hari
	{
		day = 1;
		month++;
	}
	if ((month == 2) && ((year%4 != 0) || ((year%100 == 0) && (year%400 != 0))) && (day == 29))
	{
		day = 1;
		month++;
	}
	if (((month == 1) || (month == 3) || (month == 5) || (month == 7) || (month == 8) || (month == 10) || (month == 12)) && (day == 32))
	{
		day = 1;
		month++;
	}
	if (((month == 4) || (month == 6) || (month == 9) || (month == 11)) && (day == 31))
	{
		day = 1;
		month++;
	}
	if(month == 13) //kembalikan jam ke 0 setiap 24 hitungan
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
		,  3
		,  NULL ); // */

    xTaskCreate(
		TaskKeypad
		,  (const portCHAR *)"GreenLED" // Main Arduino Uno 328p (Green) LED Blink
		,  256				// Tested 9 free @ 208
		,  NULL
		,  3
		,  NULL ); // */
	
	vTaskStartScheduler();
	
}

static void TaskDisplay(void *pvParameters) // Main Red LED Flash
{
    (void) pvParameters;
    TickType_t xLastWakeTime;
	/* The xLastWakeTime variable needs to be initialised with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();
	
	//init uart
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	
    //init lcd
    lcd_init(LCD_DISP_ON);
    lcd_led(0); //set led
	
    //lcd go home
    lcd_home();

    for(;;)
    {
		display_main(&month, &day, &hour, &minute, &second, &rate, &height, &relay);
		vTaskDelayUntil( &xLastWakeTime, ( 20 / portTICK_PERIOD_MS ) );

//		xSerialxPrintf_P(&xSerialPort, PSTR("RedLED HighWater @ %u\r\n"), uxTaskGetStackHighWaterMark(NULL));
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
	
	char buf[10];
	uint8_t key = 0xFF;

    for(;;)
    {
		key = keymap(GetKeyPressed());	//Get the keycode of pressed key
		sprintf(buf, "asdf: %d", key);
		uart_puts(buf);
		uart_puts("\r\n");
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

/*--------------------------------------------*/


