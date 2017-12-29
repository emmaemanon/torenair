/*
 *******************************************************************************
 * PIC DIGITAL THERMOMETER USING A MICROCHIP MCP9700A ANALOG SENSOR
 *******************************************************************************
 *
 * source code example for mikroC users
 * feel free to use this code at your own risks
 *
 * target : PIC16F877A, 8 Mhz crystal
 * HS clock, no watchdog.
 *
 * easyPIC4 settings :
 *      MCP9700A on DS1820 socket, see web page for more details.
 *
 * Author : Bruno Gavand, September 2007
 * see more details on http://www.micro-examples.com/
 *
 *******************************************************************************
 */
#define KEYPAD_MODE

/*
 * Keypad rows
 */
#ifdef KEYPAD_MODE
#define KP_ROW_PORT     PORTB   // row output PORT register
#define KP_ROW_TRIS     TRISB   // row output direction register
#define KP_ROW_BITNUM   0       // first bit number from 0 to 7, other bits must be next
#define KP_ROW_NUMBER   3       // number of rows, from 2 to 8
#endif

#define KP_MAX_LENGTH   16      // buffer length
#define KP_TMR_REPEAT   20      // auto-repeat

#define KP_ERASE        '*'     // erase key
#define KP_ENTER        '#'     // enter key

/*
 * Keypad columns
 */
#ifdef KEYPAD_MODE
#define KP_COL_PORT     PORTB   // column input PORT register
#define KP_COL_TRIS     TRISB   // column input direction register
#define KP_COL_BITNUM   3       // first bit number from 0 to 7, other bits must be next
#define KP_COL_NUMBER   4       // number of columns, from 1 to 8
#define KP_SCAN_CODES   "*7410852#963"  // scan codes
#else
#define KP_COL_PORT     PORTB   // column input PORT register
#define KP_COL_TRIS     TRISB   // column input direction register
#define KP_COL_BITNUM   0       // first bit number from 0 to 7, other bits must follow
#define KP_COL_NUMBER   8       // number of columns, from 2 to 8
#define KP_SCAN_CODES   "012345*#"  // scan codes
#endif