
#ifndef DISPLAY_H
#define	DISPLAY_H

#define UART_BAUD_RATE 9600

void display_msg(void);
void display_line1(void);
void display_line2(void);
void update_input(void);


void put_char(volatile uint8_t x, uint8_t y, char c);


#endif	/* DISPLAY_H */