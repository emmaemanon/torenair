/*
 *******************************************************************************
 * PIC ADVANCED UNIVERSAL KEYPAD LIBRARY
 *******************************************************************************
 *
 * source code for mikroC compiler
 * feel free to use this code at your own risks
 *
 * target : any PIC
 *
 * Author : Bruno Gavand, October 2007
 * Release : 1.0
 * see more details on http://www.micro-examples.com/
 *
 *******************************************************************************
 */
#include        "keypad_lib.h"

char            kp_buf[KP_MAX_LENGTH + 1] ;             // input buffer
unsigned char   kp_mask = 0 ;                           // current scan mask
unsigned char   kp_old = 0xff ;                         // previous key
unsigned char   kp_idx = 0 ;                            // buffer index
unsigned char   kp_flags = 0 ;                          // status flags
unsigned char   kp_last = 0 ;                           // last input char
unsigned int    kp_tmr = 0 ;                            // timer
const char      kp_tbl[] = KP_SCAN_CODES ;              // scan code to character lookup table

#ifdef KEYPAD_MODE
unsigned char   kp_cnt = 0 ;                            // row counter
unsigned char   kp_row = 0 ;                            // current row mask
unsigned char   kp_first = 0 ;                          // first row mask
#endif

/*
 * private function :
 * set bit b to value v in register pointer to by reg
 */
void    kp_bit(unsigned char *reg, unsigned char b, unsigned char v)
        {
        unsigned char m ;

        m = 1 << b ;
        if(v)
                {
                *reg |= m ;
                }
        else
                {
                *reg &= ~m ;
                }
        }

/*
 * public function :
 * init keypad
 */
void    kp_init()
        {
        unsigned int i ;

#ifdef KEYPAD_MODE
        /*
         * prepare row direction register and port register
         */
        for(i = 0 ; i < KP_ROW_NUMBER ; i++)
                {
                kp_bit(&KP_ROW_TRIS, i + KP_ROW_BITNUM, 1) ;
                kp_bit(&KP_ROW_PORT, i + KP_ROW_BITNUM, 1) ;
                }

        /*
         * prepare row masks
         */
        kp_bit(&kp_first, KP_ROW_BITNUM, 1) ;
        kp_row = kp_first ;
#endif

        /*
         * prepare column mask
         */
        for(i = 0 ; i < KP_COL_NUMBER ; i++)
                {
                kp_bit(&KP_COL_TRIS, i + KP_COL_BITNUM, 1) ;
                kp_bit(&kp_mask, i + KP_COL_BITNUM, 1) ;
                }

        /*
         * clear input buffer
         */
        *kp_buf = 0 ;
        kp_tmr = 0 ;
        }

/*
 * public function :
 * interrupt routine, called on each timer0 overflow
 */
void    kp_isr(void)
        {
        unsigned char kp_tmp, kp_key ;

        if(kp_tmp = (KP_COL_PORT & kp_mask))
                {
                unsigned char ctr ;

                kp_flags |= KP_FLAG_HIT ;               // there is a hit

                /*
                 * calculate key index
                 */
#ifdef KEYPAD_MODE
                kp_key = kp_cnt * KP_COL_NUMBER ;
#else
                kp_key = 0 ;
#endif
                kp_tmp >>= KP_COL_BITNUM ;
                for(ctr = 0 ; ctr <= KP_COL_NUMBER ; ctr++)
                        {
                        if(kp_tmp & 1)
                                {
                                kp_key += ctr ;
                                }
                        kp_tmp >>= 1 ;
                        }
                }
        else
                {
                kp_flags &= ~KP_FLAG_HIT ;              // no hit
                kp_key = 255 ;

#ifdef KEYPAD_MODE
                /*
                 * keypad row activation
                 */
                KP_ROW_TRIS |= kp_row ;         // set current row to high Z
                kp_cnt++ ;
                /*
                 * check for last row
                 */
                if(kp_cnt == KP_ROW_NUMBER)
                        {
                        /*
                         * back to first
                         */
                        kp_row = kp_first ;
                        kp_cnt = 0 ;
                        }
                else
                        {
                        kp_row <<= 1 ;
                        }

                /*
                 * activate new row
                 */
                KP_ROW_TRIS &= ~kp_row ;
                KP_ROW_PORT = kp_row ;
#endif
                }

        if(kp_tmr)
                {
                /*
                 * debounce and typematic delay
                 */
                kp_tmr-- ;
                if(kp_key == 255)
                        {
                        kp_tmr /= 2 ;
                        }
                }
        else
                {
                /*
                 * if not waiting for buffer flush
                 */
                if(!(kp_flags & KP_FLAG_ENTER))
                        {
                        /*
                         * if key has changed
                         */
                        if((kp_key != kp_old) || (kp_flags & KP_FLAG_TYPEMATIC))
                                {
                                /*
                                 * if a key is pressed
                                 */
                                if(kp_key != 255)
                                        {
                                        /*
                                         * get char from table
                                         */
                                        kp_last = kp_tbl[kp_key] ;

                                        /*
                                         * erase char
                                         */
#ifdef KP_ERASE
                                        if(kp_last == KP_ERASE)
                                                {
                                                kp_flags |= KP_FLAG_ERASE ;
                                                if(kp_idx)
                                                        {
                                                        kp_idx-- ;
                                                        }
                                                }
                                        else
#endif
                                                {
                                                kp_flags &= ~KP_FLAG_ERASE ;

                                                /*
                                                 * enter char
                                                 */
#ifdef KP_ENTER
                                                if(kp_last == KP_ENTER)
                                                        {
                                                        kp_flags |= KP_FLAG_ENTER ;
                                                        }
                                                else
#endif
                                                        {
                                                        /*
                                                         * if buffer not full
                                                         */
                                                        if(kp_idx < KP_MAX_LENGTH)
                                                                {
                                                                kp_buf[kp_idx] = kp_last ;
                                                                kp_idx++ ;
                                                                }
                                                        else
                                                                {
                                                                /*
                                                                 * when buffer full, check for circular mode
                                                                 */
                                                                if(kp_flags & KP_FLAG_CIRCULAR)
                                                                        {
                                                                        /*
                                                                         * shift buffer
                                                                         */
                                                                        for(kp_tmp = 0 ; kp_tmp < KP_MAX_LENGTH - 1 ; kp_tmp++)
                                                                                {
                                                                                kp_buf[kp_tmp] = kp_buf[kp_tmp + 1] ;
                                                                                }
                                                                        kp_buf[kp_tmp] = kp_last ;
                                                                        }
                                                                }
                                                        }
                                                }
                                        /*
                                         * string terminator
                                         */
                                        kp_buf[kp_idx] = 0 ;

                                        kp_tmr = KP_TMR_REPEAT ;
                                        }

                                kp_old = kp_key ;
                                }
                        }

                /*
                 * check for buffer full to set/clear flag
                 */
                if(kp_idx < KP_MAX_LENGTH)
                        {
                        kp_flags &= ~KP_FLAG_FULL ;
                        }
                else
                        {
                        kp_flags |= KP_FLAG_FULL ;
                        }
                }
        }