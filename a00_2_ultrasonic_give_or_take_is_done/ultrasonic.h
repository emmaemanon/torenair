
#ifndef ULTRASONIC_H
#define	ULTRASONIC_H

#define INSTR_PER_US 16                   // instructions per microsecond (depends on MCU clock, 16MHz current)
#define INSTR_PER_MS 16000                // instructions per millisecond (depends on MCU clock, 16MHz current)
#define MAX_RESP_TIME_MS 200      // timeout - max time to wait for low voltage drop (higher value increases measuring distance at the price of slower sampling)
#define DELAY_BETWEEN_TESTS_MS 50 // echo cancelling time between sampling

#define TRIG_PIN	PD3
#define TRIG_DDR	DDRD
#define TRIG_PORT	PORTD

#define ECHO_PIN	PD2
#define ECHO_DDR	DDRD
#define ECHO_PORT	PORTD

// void sonar(void);
// void init_ultrasonic(void);

#endif	/* ULTRASONIC_H */