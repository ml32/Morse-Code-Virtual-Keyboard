static uint16_t element_time = 500;
static uint32_t lastevent;

void morse_main() {
	key_in_event_t evt;
	uint32_t       duration;
	morse_sym_t    sym;
	for (;;) {
		if (lastevent.state == KEY_DOWN) {
			key_next(&evt, -1);
		} else {
			if (key_next(&evt, 3*morse_element_time) != 0) {
				sym = morse_tree_end_character();
				continue;
			}
		}
		if (evt.state == KEY_UP) {
			duration = evt.time - morse_last_event;
			if (duration < 2*morse_element_time) {
				morse_tree_enter_dit();
			} else {
				morse_tree_enter_dah();
			}
		}
		lastevent = evt;
	}
}
