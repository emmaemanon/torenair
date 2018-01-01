/* 
 * File:   fsm.h
 * Author: sangkuriang
 *
 * Created on October 26, 2015, 2:01 PM
 */

#ifndef FSM_H
#define	FSM_H

#define KEY_A	65
#define KEY_B	66
#define KEY_C	67
#define KEY_D	68
#define KEY_str	42
#define KEY_krs	35

void fsm(volatile uint8_t *input);
// void fsm(volatile uint8_t *input);
// void fsm(volatile uint8_t *input, volatile TickType_t xLastWakeTime_T1);

#endif	/* FSM_H */

