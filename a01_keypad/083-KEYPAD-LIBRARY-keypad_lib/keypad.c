/*
 *******************************************************************************
 * PIC ADVANCED UNIVERSAL KEYPAD LIBRARY
 *******************************************************************************
 *
 * source code example for mikro C compiler
 * feel free to use this code at your own risks
 *
 * target : PIC16F877A, 8 Mhz crystal
 * HS clock, no watchdog.
 *
 * Author : Bruno Gavand, October 2007
 * see more details on http://www.micro-examples.com/
 *
 * This program shows how to use the library :
 *  4 bit LCD is connected to PORTD (EasyPIC4 board)
 *  edit keypad_cfg.h header file to configure your own keypad
 *
 * LCD Line 1 : welcome message and keypad status symbols
 * LCD Line 2 : string as keyed in by user
 *
 * type 123 + enter to toggle circular/linear buffer mode
 * type 321 + enter to toggle typematic mode on/off
 *
 *******************************************************************************
 */
#include        "keypad_lib.h"          // keypad library header, includes also user's settings

/*
 * LCD custom characters
 */
const char erase_char[] = {2,6,14,31,14,6,2,0} ;        // erase key typed
const char enter_char[] = {1,1,5,13,31,12,4,0} ;        // enter key typed
const char full_char[] = {0,10,21,10,21,10,0,0} ;       // buffer is full
const char hit_char[] = {0,4,4,4,31,14,4,0} ;           // a key is pressed
const char circ_char[] = {16,24,28,25,19,7,3,1} ;       // buffer is in circular mode
const char type_char[] = {31,21,21,4,4,4,4,14} ;        // typematic enabled

/*
 * print custom character pointed to by def at line pos_row column pos_char on LCD
 */
void CustomChar(const char *def, unsigned char n, char pos_row, char pos_char)
        {
        char    i ;

        LCD_Cmd(64 + n * 8) ;
        for(i = 0 ; i <= 7 ; i++)
                {
                LCD_Chr_Cp(def[i]) ;
                }
        LCD_Cmd(LCD_RETURN_HOME) ;
        LCD_Chr(pos_row, pos_char, n) ;
        }

/*
 * interrupt routine, called on each timer0 overflow
 */
void    interrupt(void)
        {
        if(INTCON.T0IF)                         // timer 0 overflow ?
                {
                kp_isr() ;                      // call keypad service routine

                INTCON.T0IF = 0 ;               // done
                }
        }

/*
 * program entry
 */
void    main()
        {
        PORTC = 0 ;
        TRISC = 0 ;
        
        /*
         * init LCD
         */
        LCD_Init(&PORTD) ;
        LCD_Cmd(LCD_CLEAR) ;
        LCD_Out(1, 1, "KeypadLib") ;
        LCD_Cmd(LCD_SECOND_ROW) ;

        /*
         * init keypad and library
         */
        kp_init() ;

        /*
         * configure timer0 rollover interrupt
         * period is Fosc / 4 / 256
         */
        OPTION_REG = 0x87 ;                     // start timer 0, no prescaler
        INTCON = 0xA0 ;                         // allow timer 0 interrupt

        for(;;)                                 // forever
                {
                if(kp_hit())                    // if a key is pressed
                        {
                        LCD_Cmd(LCD_CURSOR_OFF) ;                       // no cursor
                        CustomChar(hit_char, 1, 1, 12) ;                // display hit symbol

                        if(kp_enter())                                  // if enter key is pressed
                                {
                         	CustomChar(enter_char, 2, 1, 13) ;

                                if(strcmp(kp_buf, "123") == 0)          // if entry is 123
                                        {
                                        if(kp_circular())               // toggle buffer circular/linear mode
                                                {
                                                kp_setLinear() ;
                                                }
                                        else
                                                {
                                                kp_setCircular() ;
                                                }
                                        }
                                else if(strcmp(kp_buf, "321") == 0)          // if entry is 321
                                        {
                                        if(kp_typematic())               // toggle typematic (auto-repeat) mode
                                                {
                                                kp_unsetTypematic() ;
                                                }
                                        else
                                                {
                                                kp_setTypematic() ;
                                                }
                                        }
                                kp_flush() ;                            // clear entry buffer
                                }
                        else
                                {
                                LCD_Chr(1, 13, ' ') ;
                                }

                        /*
                         * buffer full symbol
                         */
                        if(kp_full())
                                {
                         	CustomChar(full_char, 0, 1, 11) ;
                                }
                        else
                                {
                                LCD_Chr(1, 11, ' ') ;
                                }

                        /*
                         * erase key symbol
                         */
                        if(kp_erase())
                                {
                         	CustomChar(erase_char, 3, 1, 14) ;
                                }
                        else
                                {
                                LCD_Chr(1, 14, ' ') ;
                                }

                        /*
                         * circular mode symbol
                         */
                        if(kp_circular())
                                {
                         	CustomChar(circ_char, 4, 1, 15) ;
                                }
                        else
                                {
                                LCD_Chr(1, 15, ' ') ;
                                }

                        /*
                         * typematic mode symbol
                         */
                        if(kp_typematic())
                                {
                         	CustomChar(type_char, 5, 1, 10) ;
                                }
                        else
                                {
                                LCD_Chr(1, 10, ' ') ;
                                }

                        LCD_Chr(1, 16, kp_last()) ;                     // display last char keyed in
                        LCD_Chr(1, 12, ' ') ;                           // clear hit symbol

                        LCD_Out(2, 1, "                ") ;             // clear old buffer display
                        LCD_Out(2, 1, kp_buf) ;                         // display current buffer

                        LCD_Cmd(LCD_BLINK_CURSOR_ON) ;                  // blink cursor
                        }
                }
        }