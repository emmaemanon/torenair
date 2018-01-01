
#ifndef DISPLAY_H
#define	DISPLAY_H


#define UART_BAUD_RATE 9600


void display_main(volatile uint8_t *month,volatile uint8_t *day,volatile uint8_t *hour,volatile uint8_t *minute,volatile uint8_t *second,volatile uint8_t *rate,volatile uint32_t *sonar_result,volatile uint8_t *relay,volatile uint8_t *relay_mode);

void display_menu_wkt_tgl(void);
void display_ubah_waktu(void);
void display_ubah_tanggal(void);
void display_prompt_waktu(void);
void display_prompt_tgl(void);

void display_menu_D0(void);
void display_menu_D1_set_relay(void);
void display_prompt_D1_relay_A(void);

#endif	/* DISPLAY_H */