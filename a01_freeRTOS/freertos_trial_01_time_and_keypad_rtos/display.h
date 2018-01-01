
#ifndef DISPLAY_H
#define	DISPLAY_H


#define UART_BAUD_RATE 9600


void display_main(volatile uint8_t *month,volatile uint8_t *day,volatile uint8_t *hour,volatile uint8_t *minute,volatile uint8_t *second,volatile uint8_t *rate,volatile uint8_t *height,volatile uint8_t *relay);

#endif	/* DISPLAY_H */