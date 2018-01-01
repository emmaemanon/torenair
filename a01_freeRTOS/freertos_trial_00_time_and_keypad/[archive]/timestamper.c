//TIME STAMPER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

//#define F_CPU	16000000UL

//variabel hour untuk menyimpan nilai jam
//variabel minute untuk menyimpan nilai menit
//variabel second untuk menyimpan nilai detik
volatile uint16_t year = 2100; //MAX 65536
volatile uint8_t month = 2; //MAX 12
volatile uint8_t day = 28; //MAX 31
volatile uint8_t hour = 23; //MAX 23
volatile uint8_t minute = 50; //MAX 60
volatile uint8_t second = 50; //MAX 60

void delay(int milli_seconds)
{
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds)
		;
}

//interrupt penghitung waktu
//ISR(TIMER1_COMPA_vect)
void time_date_tracker(void)
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
	if (((month == 2) && ((year%4 == 0) && (year%100 != 0)) || (year%400 == 0)) && (day == 30)) //reset hari
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
	while(1)
	{
		printf ("%d-%d-%d, %d:%d:%d\n", year, month, day, hour, minute, second);
		time_date_tracker();
		delay(1); //delay dalam ms
	}
	return(0);
}

