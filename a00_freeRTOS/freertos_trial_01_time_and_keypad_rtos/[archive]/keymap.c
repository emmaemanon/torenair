

#include "keypad.h"

uint8_t keymap(uint8_t input) {
	switch(input) {
		case 0:
			return(0xFF); //0xFF
		break;
		case 1:
			return(0x01); //1
		break;
		case 2:
			return(0x02); //2
		break;
		case 3:
			return(0x03); //3
		break;
		case 4:
			return(0x65); //A
		break;
		case 5:
			return(0x04); //4
		break;
		case 6:
			return(0x05); //5
		break;
		case 7:
			return(0x06); //6
		break;
		case 8:
			return(0x66); //B
		break;
		case 9:
			return(0x07); //7
		break;
		case 10:
			return(0x08); //8
		break;
		case 11:
			return(0x09); //9
		break;
		case 12:
			return(0x67); //C
		break;
		case 5:
			return(0x2A); //*
		break;
		case 6:
			return(0x00); //0
		break;
		case 7:
			return(0x23); //#
		break;
		case 8:
			return(0x68); //D
		break;
        default:
        {
			return(0xFF); //0xFF
        }
	}
}