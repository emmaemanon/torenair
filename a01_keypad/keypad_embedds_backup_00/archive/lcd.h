

#include <avr/io.h>

#include <util/delay.h>

#include "myutils.h"

#ifndef _LCD_H
#define _LCD_H
/*_________________________________________________________________________________________*/

/************************************************
	LCD CONNECTIONS
*************************************************/

#define LCD_DATA B	//Port PB0 TO PB3 are connected to D4-D7

#define LCD_E B //Enable/strobe signal
#define LCD_E_POS	PB4	//Position of enable in above port

#define LCD_RS D	
#define LCD_RS_POS PD3

#define LCD_RW D
#define LCD_RW_POS PD6

#define LCD_DATA_POS 0


#define LCD202	0 //For 20 Chars by 2 lines
#define LCD204	1 //For 20 Chars by 4 lines
#define LCD162	2 //For 16 Chars by 2 lines
#define LCD164	3 //For 16 Chars by 4 lines

/***********************************************
LCD Type Selection
************************************************/

#define LCD_TYPE LCD162



//************************************************

#define LS_BLINK 0B00000001
#define LS_ULINE 0B00000010
#define LS_NONE	 0B00000000



/***************************************************
			F U N C T I O N S
****************************************************/



void LCDInit(uint8_t style);
void LCDWriteString(const char *msg);
void LCDWriteInt(int val,unsigned int field_length);
void LCDGotoXY(uint8_t x,uint8_t y);

//Low level
void LCDByte(uint8_t,uint8_t);
#define LCDCmd(c) (LCDByte(c,0))
#define LCDData(d) (LCDByte(d,1))

void LCDBusyLoop();





/***************************************************
			F U N C T I O N S     E N D
****************************************************/


/***************************************************
	M A C R O S
***************************************************/
#define LCDClear() LCDCmd(0b00000001)
#define LCDHome() LCDCmd(0b00000010);

#define LCDWriteStringXY(x,y,msg) {\
 LCDGotoXY(x,y);\
 LCDWriteString(msg);\
}

#define LCDWriteIntXY(x,y,val,fl) {\
 LCDGotoXY(x,y);\
 LCDWriteInt(val,fl);\
}
/***************************************************/




/*_________________________________________________________________________________________*/
#endif




