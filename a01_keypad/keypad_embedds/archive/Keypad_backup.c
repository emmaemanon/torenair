
#include <avr/io.h>
#include <util/delay.h>

//#include "lcd.h"
//#include "myutils.h"

uint8_t GetKeyPressed()
{
	uint8_t r, c;
	
	PORT_ROW1 |= (1 << PIN_ROW1); 
	PORT_ROW2 |= (1 << PIN_ROW2); 
	PORT_ROW3 |= (1 << PIN_ROW3); 
	PORT_ROW4 |= (1 << PIN_ROW4); 
	
	//KEYPAD_PORT|= 0X0F;

	for(c=0;c<4;c++)
	{
		//KEYPAD_DDR&=~(0X7F);
		
		KEYPAD_DDR|=(0X40>>c);
		for(r=0;r<4;r++)
		{
			if(!(KEYPAD_PIN & (0X08>>r)))
			{
				return (r*3+c);
			}
		}
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

