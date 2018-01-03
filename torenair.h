#ifndef TORENAIR_H
#define	TORENAIR_H

// ---------- ---------- ---------- ---------- //

// ukur waktu eksekusi
#define DDR_EXEC	DDRB
#define PORT_EXEC	PORTB
#define PIN_EXEC	PB5
// RELAY_PORT |= (1 << RELAY_PIN);

// ---------- Default Settings --------------- //
#define MAX_WATER_HEIGHT	200 // ketinggian air maksimum dalam cm
#define TANK_HEIGHT_CM		200

// ---------- Serial Transfer ---------------- //
//#define F_CPU	16000000UL
#define UART_BAUD_RATE 9600

// Serial Transfer
#define XFER_FREQ			100		// in Hz
#define XFER_NODATA			0
#define XFER_RDY			1

// ---------- Keypad Definitions ------------- //
// Push Button States
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

// ---------- The State Machine -------------- //
#define KEY_A	65
#define KEY_B	66
#define KEY_C	67
#define KEY_D	68
#define KEY_str	42
#define KEY_krs	35

// System States
#define DEFAULT				0
#define HIDDENPAR			1
#define CHANGEDATE_Y3		2
#define CHANGEDATE_Y2		3
#define CHANGEDATE_Y1		4
#define CHANGEDATE_Y0		5
#define CHANGEDATE_M1		6
#define CHANGEDATE_M0		7
#define CHANGEDATE_D1		8
#define CHANGEDATE_D0		9
#define POST_CHANGEDATE		10
#define CHANGETIME_h1		11
#define CHANGETIME_h0		12
#define CHANGETIME_m1		13
#define CHANGETIME_m0		14
#define CHANGETIME_d1		15
#define CHANGETIME_d0		16
#define POST_CHANGETIME		17

#define PRE_WATERMAX_W3		18
#define PRE_WATERMAX_W2		19
#define PRE_WATERMAX_W1		20
#define PRE_WATERMAX_W0		21
#define POST_WATERMAX		22

#define CHANGERATE_R3		23
#define CHANGERATE_R2		24
#define CHANGERATE_R1		25
#define CHANGERATE_R0		26
#define POST_CHANGERATE		27

#define PRE_WATERCAL		28
#define PRE_TANKHEIGHT_X3	29
#define PRE_TANKHEIGHT_X2	30
#define PRE_TANKHEIGHT_X1	31
#define PRE_TANKHEIGHT_X0	32
#define POST_TANKHEIGHT		33
#define PRE_MODERELAY		34
#define MODERELAY_M			35
#define MODERELAY_A2M		36
#define POST_MODERELAY_M	37

// void date_and_time(void);s
void change_state(void);

// ---------- Ultrasonic Definitions --------- //
#define INSTR_PER_US 16 // instructions per microsecond (depends on MCU clock, 16MHz current)
#define INSTR_PER_MS 16000 // instructions per millisecond (depends on MCU clock, 16MHz current)
#define MAX_RESP_TIME_MS 200 // timeout - max time to wait for low voltage drop (higher value increases measuring distance at the price of slower sampling)
#define DELAY_BETWEEN_TESTS_MS 50 // echo canceling time between sampling

#define SONAR_DELTA_LIMIT 50 // batas perubahan nilai pengukuran sonar dalam cm

#define TRIG_PIN	PD2
#define TRIG_DDR	DDRB
#define TRIG_PORT	PORTB

#define ECHO_PIN	PD3
#define ECHO_DDR	DDRD
#define ECHO_PORT	PORTD

// ---------- Relay Definitions -------------- //

#define RELAY_PIN	PB4
#define RELAY_DDR	DDRB
#define RELAY_PORT	PORTB

#define RELAY_AUTO		65 // A
#define RELAY_MANUAL	77 // M

// ---------- Function Prototypes ------------ //

void initialize(void);
void init_time_keypad(void);

// ------ Function Prototypes display.h ------ //
void display_msg(void);
void display_line1(void);
void display_line2(void);
void put_char(uint8_t x, uint8_t y, char c);


#endif	/* TORENAIR_H */