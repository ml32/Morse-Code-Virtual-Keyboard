#include "morse.h"

#include "key.h"
#include "window.h"
#include "morsetree.h"

static int element_time = 250;

int morse_init() {
	return morse_tree_init();
}

void morse_set_wpm(float wpm) {
	element_time = (int)(1200.0f / wpm);
}

void morse_main() {
	key_in_event_t evt;
	int            time, duration;
	morse_sym_t    sym;

	/* it makes sense to use 'goto' for simple finite-state machines */

	reset:

	/* hide display */
	do {
		key_next(&evt, -1);
	} while (evt.state != KEY_DOWN);
	/* show display */


	active:

	time = evt.time;
	do {
		key_next(&evt, -1);
	} while (evt.state != KEY_UP);
	duration = evt.time - time;
	printf("Duration: %dms\n", duration);
	if (duration < 2 * element_time) {
		printf("Element: . (dit)\n");
		morse_tree_enter_dit();
		window_update(element_time);
	} else {
		printf("Element: - (dah)\n");
		morse_tree_enter_dah();
		window_update(element_time);
	}
	time = evt.time;

	do {
		if (key_next(&evt, 2 * element_time) < 0) goto finish;
	} while (evt.state != KEY_DOWN);
	goto active;


	finish:

	sym = morse_tree_end_character();
	window_update(element_time);
	do {
		/* we're already two time units in -- 2 + 3 = 5 */
		if (key_next(&evt, 3 * element_time) < 0) goto reset;
	} while (evt.state != KEY_DOWN);
	/* enter a space */
	goto active;	
}
