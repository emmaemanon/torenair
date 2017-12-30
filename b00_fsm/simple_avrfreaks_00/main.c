
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

typedef enum {
	INACTIVE,
	INITIALISING,
	FLASHING,
	DEACTIVATING 
} my_states;

my_states state_var = INACTIVE;

unsigned char flash_count = 0;

void delay(int milli_seconds)
{
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds)
		;
}

void process_state_machine(void) {
	switch (state_var) {

	case INACTIVE:
        printf("INACTIVE\n");
		delay(1000);
		state_var = INITIALISING;
		break;

	case INITIALISING:
        printf("INITIALISING\n");
		delay(1000);
		state_var = FLASHING;
		break;

	case FLASHING:
        printf("FLASHING\n");
		flash_count++;
		if (flash_count < 20) {
			printf("STAT: %d\n", flash_count);
			delay(1000);
		}
		else {
			flash_count = 0;
			state_var = DEACTIVATING;
		}
		break;

	case DEACTIVATING:
        printf("DEACTIVATING\n");
		delay(1000);
		state_var = INACTIVE;
		break;

	default:
		break; //never happens (I hope!)
	}
}

int main(void) {
	printf("BEGIN\n");
	while (1) {
		process_state_machine();
	}
	return(0);
}
