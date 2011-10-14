#include <stdio.h>
#include "key.h"
#include "window.h"
#include "morsetree.h"

int main(int argc, char **argv) {
	/*
	char        c;
	morse_sym_t symlist[MORSE_SYMLIST_SIZE];
	uint8_t     n, i;
	*/
	if (morse_tree_init() != 0) { return -1; }
	/*
	for (;;){
		c = getchar();
		switch (c) {
			case '.':
				morse_tree_enter_dit();
				break;
			case '-':
				morse_tree_enter_dah();
				break;
			default:
				break;
		}

		morse_tree_get_hidden(symlist, &n);
		printf("hidden: ");
		for (i=0; i<n; i++) {
			putchar(morse_sym_to_char(symlist[i]));
		}

		morse_tree_get_dits(symlist, &n);
		printf("\ndit: ");
		for (i=0; i<n; i++) {
			putchar(morse_sym_to_char(symlist[i]));
		}

		morse_tree_get_dahs(symlist, &n);
		printf("\ndah: ");
		for (i=0; i<n; i++) {
			putchar(morse_sym_to_char(symlist[i]));
		}

		printf("\nselected: %c\n\n", morse_sym_to_char(morse_tree_get_selected()));
	}
	*/

	if (key_init() != 0) {
		return -1;
	}
	if (window_init(&argc, &argv) != 0) {
		return -1;
	}

	key_in_event_t evt;
	for (;;) {
		if (key_next(&evt, 50) < 0) {
			printf("Timeout!\n");
		} else {
			printf("Event: %i: %s\n", evt.time, evt.state == KEY_DOWN ? "PRESSED" : "RELEASED");
		}
		//window_main();
	}
	return 0;
}


