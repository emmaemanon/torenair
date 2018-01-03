
#ifndef RELAY_H
#define	RELAY_H

#define RELAY_PIN	PB4
#define RELAY_DDR	DDRB
#define RELAY_PORT	PORTB

void set_relay(volatile uint8_t *relay);

#endif	/* RELAY_H */