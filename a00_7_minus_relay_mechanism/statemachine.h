
#ifndef STATEMACHINE_H
#define	STATEMACHINE_H

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

#define PRE_CHANGERATE		23
#define PRE_WATERCAL		24
#define PRE_TANKHEIGHT_X3	25
#define PRE_TANKHEIGHT_X2	26
#define PRE_TANKHEIGHT_X1	27
#define PRE_TANKHEIGHT_X0	28
#define POST_TANKHEIGHT		29
// #define DELETED				4
// #define PRE_PLAYBACK			5
// #define PLAYBACK				6
// #define POST_PLAYBACK		7
// #define VOLUME_SELECT		8
// #define SETTINGS				9
// #define FLASH_UTILITY		10
// #define ERASE_FLASH			11
// #define ERROR					12

void date_and_time(void);
void change_state(void);

#endif	/* STATEMACHINE_H */

