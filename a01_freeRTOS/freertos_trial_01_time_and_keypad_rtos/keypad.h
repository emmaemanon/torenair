

#ifndef KEYPAD_H
#define KEYPAD_H


#define PORT_R1	PORTB
#define PORT_R2	PORTB
#define PORT_R3	PORTB
#define PORT_R4	PORTB
#define PORT_C1	PORTD
#define PORT_C2	PORTD
#define PORT_C3	PORTD
#define PORT_C4	PORTD

#define DDR_R1	DDRB
#define DDR_R2	DDRB
#define DDR_R3	DDRB
#define DDR_R4	DDRB
#define DDR_C1	DDRD
#define DDR_C2	DDRD
#define DDR_C3	DDRD
#define DDR_C4	DDRD

#define PIN_R1	PINB
#define PIN_R2	PINB
#define PIN_R3	PINB
#define PIN_R4	PINB
#define PIN_C1	PIND
#define PIN_C2	PIND
#define PIN_C3	PIND
#define PIN_C4	PIND

#define PD_R1	PB3
#define PD_R2	PB2
#define PD_R3	PB1
#define PD_R4	PB0
#define PD_C1	PD7
#define PD_C2	PD6
#define PD_C3	PD5
#define PD_C4	PD4

uint8_t GetKeyPressed();
uint8_t keymap(uint8_t input);

#endif