#ifndef TORENAIRCONF_H
#define	TORENAIRCONF_H

#define MAX_WATER_HEIGHT	190 // ketinggian air maksimum dalam cm
#define TANK_HEIGHT_CM		200

//#define F_CPU	16000000UL
#define UART_BAUD_RATE 9600

// System States
#define IDLE					0
#define PRE_CAPTURE			1
#define CAPTURE				2
#define POST_CAPTURE			3
#define DELETED				4
#define PRE_PLAYBACK			5
#define PLAYBACK				6
#define POST_PLAYBACK		7
#define VOLUME_SELECT		8
#define SETTINGS				9
#define FLASH_UTILITY		10
#define ERASE_FLASH			11
#define ERROR					12


void initialize(void);
void init_time_keypad(void);

#endif	/* TORENAIRCONF_H */