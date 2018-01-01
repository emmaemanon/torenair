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
#include        "keypad_cfg.h"

/*
 * Keypad flags
 */
#define KP_FLAG_FULL            0x01
#define KP_FLAG_HIT             0x02
#define KP_FLAG_ENTER           0x04
#define KP_FLAG_ERASE           0x08
#define KP_FLAG_CIRCULAR        0x10
#define KP_FLAG_TYPEMATIC       0x20

#define kp_full()               (kp_flags & KP_FLAG_FULL)
#define kp_hit()                (kp_flags & KP_FLAG_HIT)
#define kp_enter()              (kp_flags & KP_FLAG_ENTER)
#define kp_erase()              (kp_flags & KP_FLAG_ERASE)
#define kp_last()               kp_last
#define kp_setCircular()        kp_flags |= KP_FLAG_CIRCULAR
#define kp_setLinear()          kp_flags &= ~KP_FLAG_CIRCULAR
#define kp_circular()           (kp_flags & KP_FLAG_CIRCULAR)
#define kp_setTypematic()       kp_flags |= KP_FLAG_TYPEMATIC
#define kp_unsetTypematic()     kp_flags &= ~KP_FLAG_TYPEMATIC
#define kp_typematic()          (kp_flags & KP_FLAG_TYPEMATIC)
#define kp_flush()              kp_idx = 0, *kp_buf = 0, kp_flags &= ~KP_FLAG_ENTER

extern  char            kp_buf[KP_MAX_LENGTH + 1] ;
extern  unsigned char   kp_mask ;
extern  unsigned char   kp_old ;
extern  unsigned char   kp_idx ;
extern  unsigned char   kp_flags ;
extern  unsigned char   kp_last ;
extern  unsigned int    kp_tmr ;
extern  const char      kp_tbl[] ;

#ifdef KP_BUTTON_MODE
extern  unsigned char   kp_cnt ;
extern  unsigned char   kp_row ;
extern  unsigned char   kp_first ;
#endif

extern void    kp_bit(unsigned char *reg, unsigned char b, unsigned char v) ;
extern void    kp_init() ;
extern void    kp_isr() ;