/*
 * lab5_sound.h
 *
 * Created: 4/11/2012 12:30:00 PM
 * Authors: Michael Wu (myw9) and Garen Der-Khachadourian (gdd9)
 */

#ifndef LAB5_STETHOSCOPE_H_
#define LAB5_STETHOSCOPE_H_

/******************************************************************************
 * Macro Defines
 *****************************************************************************/

#define min(X,Y) (((X) < (Y)) ? (X) : (Y))
#define max(X,Y) (((X) > (Y)) ? (X) : (Y))

/******************************************************************************
 * Stethoscope Constants
 *****************************************************************************/

#define SAMPLE_FREQ			8000		// in Hz
#define BIAS_OFFSET			128	// 2.5V * 255 quanta / 5V = 128

// Serial Transfer
#define XFER_FREQ			100		// in Hz
#define XFER_NODATA		0
#define XFER_RDY			1

// Flash (Application Specific)
#define FLASH_PAGE_SIZE			256
#define FLASH_BLOCK_SIZE_64KB	65536

#define NUM_FLASH_READ_BUF	2
#define NUM_RECORDINGS		10

#define REC_SR_ADDR			0x004000
#define SIGNATURE_ADDR		0x005000
#define REC0_ADDR				0x010000
#define REC1_ADDR				0x020000
#define REC2_ADDR				0x030000
#define REC3_ADDR				0x040000
#define REC4_ADDR				0x050000
#define REC5_ADDR				0x060000
#define REC6_ADDR				0x070000
#define REC7_ADDR				0x080000
#define REC8_ADDR				0x090000
#define REC9_ADDR				0x0A0000

#define FLASH_SIGNATURE		0xDEADBEEF

/******************************************************************************
 * User Interface Constants
 *****************************************************************************/

// Port Mapping
#define KEYPAD_UPPER_PORT_DIR		DDRA
#define KEYPAD_UPPER_PORT_DATA	PORTA
#define KEYPAD_UPPER_PORT_IN		PINA
#define KEYPAD_LOWER_PORT_DIR		DDRB
#define KEYPAD_LOWER_PORT_DATA	PORTB
#define KEYPAD_LOWER_PORT_IN		PINB

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
#define PUSHED_STATE			2
#define MAYBE_REL_STATE		3

// Keypad Button Number
#define MAXKEYS 16
#define NO_BUTTON_PUSHED	0
#define MULTIPLE_PUSHED		17

// Push Button Flags
#define NOT_PUSHED_FLAG		0
#define PUSHED_FLAG			1

// Task periods
#define TASK_PERIOD_DEBOUNCE_MS		30		// ms
#define TASK_PERIOD_SERIAL_XFER_MS	10		// ms

// Edit Mode Definitions
#define EDIT_OFF				0
#define EDIT_ON				1
#define NO_INPUT				' '

#define MAX_VOLUME			9

/******************************************************************************
 * Text Constants
 *****************************************************************************/

char mode1str[] = "1: Record Waveform ";
char mode2str[] = "2: Play Waveform   ";
char mode3str[] = "3: Volume Control  ";
char mode4str[] = "4: View Settings   ";
char mode5str[] = "5: Flash Utility   ";
char start[]    = "A: Start           ";
char stop[]     = "B: Stop            ";
char exit1[]    = "D: Exit            ";
char exit2[]    = "Exit               ";
char save[]     = "A: Save            ";
char delete1[]  = "A: Delete          ";
char play1[]    = "B: Play            ";
char erase[]    = "A: Erase Memory    ";

/******************************************************************************
 * Struct Definitions
 *****************************************************************************/

// Flash buffer for writing/reading to/from memory
typedef struct {
	uint8_t buffer[FLASH_PAGE_SIZE];
	uint16_t size;
} flash_buffer;

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/

void initialize(void);
void init_systime(void);
void init_lcd(void);
void init_adc(void);
void init_pwm(void);
void init_sampling(void);
void init_flash(void);

void display_msg(void);
void display_line1(void);
void display_line2(void);
void display_line3(void);
void display_line4(void);
void update_input(void);

uint8_t read_keypad(void);
void check_button(void);
void debounce_button(void);
void button_action(void);

void change_state(void);
void enable_capturing(void);
void disable_capturing(void);
void enable_playback(void);
void disable_playback(void);
void enable_adc(void);
void disable_adc(void);
void enable_pwm(void);
void disable_pwm(void);
void enable_sampling(void);
void disable_sampling(void);

void flash_chip_format(void);
void flash_init_rec_read(uint8_t id);
void flash_erase_rec(uint8_t id);
uint8_t fetch_waveforms_saved(void);
uint8_t fetch_rec_id(void);
uint16_t flash_read_rec_sr(void);
void flash_write_rec_sr(uint16_t rec_sr_val);
uint8_t is_rec_valid(uint8_t id);

#endif /* LAB5_STETHOSCOPE */
