
#include <avr/io.h>
#include <util/delay.h>

//row1-4:PD4-7
//col1-4:PB0-1

//#include "lcd.h"
//#include "myutils.h"

#define SCSIBITS 

#define KEYPAD_PIN

#define KEYPAD_PIN_ROW ((PIN_R1 << 3) | (PIN_R2 << 2) | (PIN_R3 << 1) | (PIN_R4))
#define KEYPAD_PIN_COL ((PIN_C1 << 3) | (PIN_C2 << 2) | (PIN_C3 << 1) | (PIN_C4))

((((uint8_t)PORTB & B00011111) << 3) | (((uint8_t)PORTD & B11100000) >> 5)))

void KeypadPort(uint8_t byteValue)
{
	asdf
}

uint8_t GetKeyPressed()
{
	uint8_t r, c;
	
	PORT_ROW1 |= (1 << PIN_ROW1); 
	PORT_ROW2 |= (1 << PIN_ROW2); 
	PORT_ROW3 |= (1 << PIN_ROW3); 
	PORT_ROW4 |= (1 << PIN_ROW4); 

	for(c=0;c<4;c++)
	{
		//KEYPAD_DDR&=~(0X7F);
		DDR_COL1 |= (1 << PIN_COL1);
		PORT_COL1 &= ~(1 << PIN_COL1);
		
		if
		
		for (byte r=0; r<sizeKpd.rows; r++) {
			bitWrite(bitMap[r], c, !pin_read(rowPins[r]));  // keypress is active low so invert to high.
		}
		
		//KEYPAD_DDR|=(0X40>>c);
		//for(r=0;r<4;r++)
		//{
		//	if(!(KEYPAD_PIN & (0X08>>r)))
		//	{
		//		return (r*3+c);
		//	}
		//}
	}

	return 0XFF;//Indicate No keypressed
}
	

void main()
{
	//Wait for LCD To Start
	//_delay_loop_2(0);
	
	//Now initialize the module
	//LCDInit(LS_NONE);

	uint8_t key;

	while(1)
	{
		key=GetKeyPressed();	//Get the keycode of pressed key

		LCDWriteIntXY(0,0,key,3);	//Print it at location 0,0 on LCD.
	}

}

