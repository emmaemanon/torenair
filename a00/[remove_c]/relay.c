
#include <avr/io.h>
#include "torenair.h"

void set_relay(volatile uint8_t *relay)
{
	//Init
	RELAY_DDR |= (1 << RELAY_PIN); // Set Relay Pin for Output
	
	if(*relay == 1) {
		RELAY_PORT |= (1 << RELAY_PIN);
	}
	else
	{
		RELAY_PORT &= ~(1 << RELAY_PIN);
	}
}