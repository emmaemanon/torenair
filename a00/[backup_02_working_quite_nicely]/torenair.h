#ifndef TORENAIRCONF_H
#define	TORENAIRCONF_H

#define MAX_WATER_HEIGHT	190 // ketinggian air maksimum dalam cm
#define TANK_HEIGHT_CM		200

//#define F_CPU	16000000UL
#define UART_BAUD_RATE 9600


// Serial Transfer
#define XFER_FREQ			100		// in Hz
#define XFER_NODATA		0
#define XFER_RDY			1

void initialize(void);
void init_time_keypad(void);

#endif	/* TORENAIRCONF_H */