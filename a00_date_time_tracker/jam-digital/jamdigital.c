// Jam Digital
// Dasi Edimas Akbar (13214068)
// Mukhtar Amin (13214123)

#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>;

//variabel hours untuk menyimpan nilai jam
//variabel minutes untuk menyimpan nilai detik
//variabel seconds untuk menyimpan nilai detik
unsigned char hours = 0;
unsigned char minutes = 0;
unsigned char seconds = 0;


//prosedur untuk dekode bilangan desimal ke output seven segment
unsigned char digit7seg(int digit)
{
	unsigned char segVal;
	
    switch (digit){
    case 0: segVal = 0b00111111; break;
    case 1: segVal = 0b00000110; break;
    case 2: segVal = 0b01011011; break;
    case 3: segVal = 0b01001111; break;
    case 4: segVal = 0b01100110; break;
    case 5: segVal = 0b01101101; break;
    case 6: segVal = 0b01111101; break;
    case 7: segVal = 0b00000111; break;
    case 8: segVal = 0b01111111; break;
    case 9: segVal = 0b01101111; break;
    }
	return segVal;
}

//prosedur untuk mengaktifkan digit pertama (digit paling kiri)
void digit1 (void)
{
	PORTD = ~0x10;
}

//prosedur untuk mengaktifkan digit kedua (digit kedua dari kiri)
void digit2 (void)
{
	PORTD = ~0x20;
}

//prosedur untuk mengaktifkan digit ketiga (digit ketiga dari kiri)
void digit3 (void)
{
	PORTD = ~0x40;
}

//prosedur untuk mengaktifkan digit keempat (digit paling kanan)
void digit4 (void)
{
	PORTD = ~0x80;
}

//prosedur untuk mapping nilai dekoder seven segment ke port keluaran
void showNum (int number)
{
	
	PORTB = digit7seg(number);
	PORTC = ((digit7seg(number)>>6)<<PC4);
}

//interrupt pertama untuk adjust menit
ISR(INT0_vect)
{
	if(minutes < 59)
		minutes++;
	else
		minutes = 0;
}

//interrupt kedua untuk adjust detik
ISR(INT1_vect)
{
	if(seconds < 59)
		seconds++;
	else
		seconds = 0;
}

//program utama
int main (void)
{
	DDRD = 0xF0; //mengaktifkan PD0-3 sebagai input, PD4-7 sebagai output
	DDRC = 0x10; //mengaktifkan PC4 sebagai output
	DDRB = 0xFF; //mengaktifkan seluruh pin port B sebagai output
	PORTD = 0xF0; //output PD4-7 default HIGH
	
	EICRA = 0x0F; //rising edge INT0 dan INT1 memberikan interrupt
	EIMSK = 0x03; //mengaktifkan INT0 dan INT1
	
	TCCR1B = (1<<CS12)|(1<<WGM12); //CS12 untuk prescaler 256, WGM12 untuk mode operasi CTC
	OCR1A = 62500-1; //nilai OCRA dengan F_CPU 16MHz dan waktu countdown 1 detik
	TIMSK1 = (1<<OCIE1A); //mengaktifkan Timer/Counter2 Compare Match A
	sei(); //set global interrupt enable
	
	while(1)
	{
		PORTD = 0xF0; //mematikan seluruh digit display seven segment
		showNum(minutes/10); //setting keluaran untuk digit puluhan menit
		digit1(); //nyalakan digit1
		_delay_us(100); //delay 100us atau 0.1ms
		
		//mematikan seluruh digit display
		//set keluaran untuk digit satuan menit
		//nyalakan digit2 dan lakukan delay sebesar 100us
		PORTD = 0xF0;
		showNum(minutes%10);
		digit2();
		_delay_us(100);
		
		//mematikan seluruh digit display
		//set keluaran untuk digit puluhan detik
		//nyalakan digit3 dan lakukan delay sebesar 100us
		PORTD = 0xF0;
		showNum(seconds/10);
		digit3();
		_delay_us(100);
		
		//mematikan seluruh digit display
		//set keluaran untuk digit satuan detik
		//nyalakan digit4 dan delay sebesar 100us
		PORTD = 0xF0;
		showNum(seconds%10);
		digit4();
		_delay_us(100);
	}
}

//interrupt penghitung waktu
ISR(TIMER1_COMPA_vect)
{
	//tambah detik setiap masuk interrupt
	seconds++;
	
	if(seconds == 60) //kembalikan detik ke 0 setiap 60 hitungan
	{
		seconds = 0;
		minutes++;
	}
	if(minutes == 60) //kembalikan menit ke 0 setiap 60 hitungan
	{
		minutes = 0;
		hours++;		
	}
	if(hours > 23) //kembalikan jam ke 0 setiap 24 hitungan
		hours = 0;
}
