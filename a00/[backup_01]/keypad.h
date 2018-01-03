

#ifndef KEYPAD_H
#define KEYPAD_H

// ---------- Push Button States ------------- //
#define RELEASED_STATE		0
#define MAYBE_PUSH_STATE	1
#define PUSHED_STATE		2
#define MAYBE_REL_STATE		3

#define PORT_R1	PORTD
#define PORT_R2	PORTD
#define PORT_R3	PORTD
#define PORT_R4	PORTD
#define PORT_C1	PORTB
#define PORT_C2	PORTB
#define PORT_C3	PORTB
#define PORT_C4	PORTB

#define DDR_R1	DDRD
#define DDR_R2	DDRD
#define DDR_R3	DDRD
#define DDR_R4	DDRD
#define DDR_C1	DDRB
#define DDR_C2	DDRB
#define DDR_C3	DDRB
#define DDR_C4	DDRB

#define PIN_R1	PIND
#define PIN_R2	PIND
#define PIN_R3	PIND
#define PIN_R4	PIND
#define PIN_C1	PINB
#define PIN_C2	PINB
#define PIN_C3	PINB
#define PIN_C4	PINB

#define PD_R1	PD4
#define PD_R2	PD5
#define PD_R3	PD6
#define PD_R4	PD7
#define PD_C1	PB0
#define PD_C2	PB1
#define PD_C3	PB2
#define PD_C4	PB3

// Butnum Definitions
#define CANCEL				10
#define ENTER				11
#define SPECIAL_A			12
#define SPECIAL_B			13
#define SPECIAL_C			14
#define SPECIAL_D			15

// Push Button States
#define RELEASED_STATE		0
#define MAYBE_PUSH_STATE	1
#define PUSHED_STATE		2
#define MAYBE_REL_STATE		3

// Keypad Button Number
#define MAXKEYS				16
#define NO_BUTTON_PUSHED	0xFF
#define MULTIPLE_PUSHED		17

// Push Button Flags
#define NOT_PUSHED_FLAG		0
#define PUSHED_FLAG			1

// Task periods
#define TASK_PERIOD_DEBOUNCE_MS		30		// ms
#define TASK_PERIOD_SERIAL_XFER_MS	1000	// ms

// Edit Mode Definitions
#define EDIT_OFF			1
#define EDIT_ON				0
#define NO_INPUT			' '


uint8_t read_keypad(void);
uint8_t keymap(uint8_t input);
void check_button(void);
void debounce_button(void);
void button_action(void);

#endif