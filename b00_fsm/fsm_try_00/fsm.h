/* 
 * File:   fsm.h
 * Author: sangkuriang
 *
 * Created on October 26, 2015, 2:01 PM
 */

#ifndef FSM_H
#define	FSM_H

#define S_INIT				0
#define S_HOME				1
//#define S_HOME_R_AUTO		1 //bisa jadi satu ga ya? S_HOME gitu?
//#define S_HOME R_MANUAL		2

#define S_MENU_TINGGI		5
#define S_TINGGI_TR			6
#define S_TINGGI_AIR		7
#define S_PROMPT_TR			8
#define S_PROMPT_AIR		9

#define S_MENU_LOG			10
#define S_PROMPT_LOG		11

#define S_MENU_WAKTU_TGL	15
#define S_UBAH_WKT			16
#define S_UBAH_TGL			17
#define S_PROMPT_WKT		18
#define S_PROMPT_TGL		19

#define S_MENU_RELAY_MANUAL	20
#define S_UBAH_STAT_RELAY	21
#define S_PROMPT_RELAY		22

#define S_MENU_RELAY_AUTO	25
#define S_PROMPT_RELAY_AUTO	26

//hmm, inputnya dari
//1. tombol PB, nilainya 0 sama 1
//2. saklar toggle baterai, nilainya 0 sama 1
//3. saklar toggle arus, nilainya 0 sama 1
//outputnya ada
//1. led indikator power bank, nilainya 0 sama 1
//2. lampu senter power bank, nilainya 0 sama 1

//void fsm(int *i_button, int *i_baterai, int *i_arus, int *o_bank, int *o_senter, int *state);
void fsm(int *i_short, int *i_long, int *i_arus, int *i_baterai, int *o_bank, int *o_senter, int *state);
//void fsm(int input, int *output, int *state);

#endif	/* FSM_H */

