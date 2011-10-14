#ifndef KEY_H
#define KEY_H

#include <stdint.h>

#define KEY_DOWN 0xFF
#define KEY_UP   0x00

typedef struct key_in_event {
	int state;
	int time;
} key_in_event_t;

int  key_init();

/* Blocks until an event is received, or the duration specified by timeout
 * (in milliseconds) has passed.
 * Returns 0 on success, or -1 on timeout */
int key_next(key_in_event_t *evt, int timeout);


#endif
