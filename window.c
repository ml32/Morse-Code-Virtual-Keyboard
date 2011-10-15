#include "window.h"

#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include "morsetree.h"

#define WINDOW_ITEM_WIDTH  24
#define WINDOW_ITEM_HEIGHT 24

static ClutterActor *window_stage = NULL;
static ClutterActor *window_items[MORSE_SYM_COUNT-1];

static const ClutterColor bgcolor = {
	.red   = 0xFF,
	.green = 0xFF,
	.blue  = 0xFF,
	.alpha = 0x00
};
static const ClutterColor bgcolor_left = {
	.red   = 0xFF,
	.green = 0x80,
	.blue  = 0x80,
	.alpha = 0x40
};
static const ClutterColor bgcolor_right = {
	.red   = 0x80,
	.green = 0x80,
	.blue  = 0xFF,
	.alpha = 0x40
};
static const ClutterColor bgcolor_selected = {
	.red   = 0x80,
	.green = 0xFF,
	.blue  = 0x80,
	.alpha = 0x40
};
static const ClutterColor bgcolor_disabled = {
	.red   = 0x40,
	.green = 0x40,
	.blue  = 0x40,
	.alpha = 0x40
};
static const ClutterColor fgcolor_item = {
	.red   = 0x00,
	.green = 0x00,
	.blue  = 0x00,
	.alpha = 0xFF
};
static const ClutterColor bgcolor_item = {
	.red   = 0xFF,
	.green = 0xFF,
	.blue  = 0xFF,
	.alpha = 0xC0
};

static void window_move_items(morse_sym_t *symlist, int n, int x, int y, int w, int h, int duration);
static void window_move_item(morse_sym_t sym, int x, int y, int duration);



int window_init(int *argc, char ***argv) {
	Display *display;
	Window   stagewin;
	XSetWindowAttributes stageattr;
	Atom     state;
	Atom     state_top;

	ClutterActor *leftarea;
	ClutterActor *rightarea;
	ClutterActor *selected;
	ClutterActor *disabled;

	ClutterLayoutManager *bin;
	ClutterActor         *item;
	ClutterActor         *label;
	char                  buf[2];

	g_thread_init(NULL);
	clutter_threads_init();
	clutter_x11_set_use_argb_visual(TRUE);
	clutter_init(argc, argv);
	window_stage = clutter_stage_get_default();
	display      = clutter_x11_get_default_display();
	stagewin     = clutter_x11_get_stage_window(CLUTTER_STAGE(window_stage));
	stageattr.override_redirect = True;
	XChangeWindowAttributes(display, stagewin, CWOverrideRedirect, &stageattr);
	state        = XInternAtom(display, "_NET_WM_STATE", False);
	state_top    = XInternAtom(display, "_NET_WM_STATE_STAYS_ON_TOP", False);
	XChangeProperty(display, stagewin, state, XA_ATOM, 32,
	                PropModeReplace, (unsigned char *)&state_top, 1);

	clutter_actor_set_size(window_stage, 880, 96);
	clutter_stage_set_use_alpha(CLUTTER_STAGE(window_stage), TRUE);
	clutter_stage_set_color(CLUTTER_STAGE(window_stage), &bgcolor);

	leftarea = clutter_rectangle_new_with_color(&bgcolor_left);
	clutter_actor_set_size(leftarea, 420, 48);
	clutter_actor_set_position(leftarea, 0, 0);
	clutter_container_add_actor(CLUTTER_CONTAINER(window_stage), leftarea);

	rightarea = clutter_rectangle_new_with_color(&bgcolor_right);
	clutter_actor_set_size(rightarea, 420, 48);
	clutter_actor_set_position(rightarea, 460, 0);
	clutter_container_add_actor(CLUTTER_CONTAINER(window_stage), rightarea);

	selected = clutter_rectangle_new_with_color(&bgcolor_selected);
	clutter_actor_set_size(selected, 40, 48);
	clutter_actor_set_position(selected, 420, 0);
	clutter_container_add_actor(CLUTTER_CONTAINER(window_stage), selected);

	disabled = clutter_rectangle_new_with_color(&bgcolor_disabled);
	clutter_actor_set_size(disabled, 880, 48);
	clutter_actor_set_position(disabled, 0, 48);
	clutter_container_add_actor(CLUTTER_CONTAINER(window_stage), disabled);

	for (int i=1; i<MORSE_SYM_COUNT; i++) {
		bin  = clutter_bin_layout_new(CLUTTER_BIN_ALIGNMENT_CENTER, CLUTTER_BIN_ALIGNMENT_CENTER);

		item = clutter_box_new(bin);
		clutter_actor_set_size(item, WINDOW_ITEM_WIDTH, WINDOW_ITEM_HEIGHT);
		clutter_box_set_color(CLUTTER_BOX(item), &bgcolor_item);

		buf[0] = morse_sym_to_char(i);
		buf[1] = 0;
		label  = clutter_text_new_full("Sans 16px", buf, &fgcolor_item);

		clutter_bin_layout_add(CLUTTER_BIN_LAYOUT(bin), label, CLUTTER_BIN_ALIGNMENT_CENTER, CLUTTER_BIN_ALIGNMENT_CENTER);

		clutter_container_add_actor(CLUTTER_CONTAINER(window_stage), item);

		window_items[i-1] = item;
	}

	clutter_actor_show_all(window_stage);

	window_update(50);

	return 0;
}

void window_update(int element_time) {
	static morse_sym_t symlist[MORSE_SYMLIST_SIZE];
	uint8_t n;

	clutter_threads_enter();

	morse_tree_get_hidden(symlist, &n);
	window_move_items(symlist, n, 0, 48, 880, 48, element_time / 2);
	morse_tree_get_dits(symlist, &n);
	window_move_items(symlist, n, 0, 0, 420, 48, element_time / 2);
	morse_tree_get_dahs(symlist, &n);
	window_move_items(symlist, n, 460, 0, 420, 48, element_time / 2);
	
	morse_sym_t sym = morse_tree_get_selected();
	window_move_item(sym, 420 + (40 - WINDOW_ITEM_WIDTH) / 2, (48 - WINDOW_ITEM_HEIGHT) / 2, element_time / 2);

	clutter_threads_leave();
}

void window_main() {
	clutter_main();
}

static void window_move_item(morse_sym_t sym, int x, int y, int duration) {
	printf("%d, %d, %d\n", sym, x, y);
	if (sym == MORSE_SYM_NULL) return;
	ClutterActor *item;
	item = window_items[sym-1];
	clutter_actor_animate(item, CLUTTER_EASE_IN_OUT_CUBIC, duration,
			      "x", (float)x,
			      "y", (float)y,
			      NULL);
}

static void window_move_items(morse_sym_t *symlist, int n, int x, int y, int w, int h, int duration) {
	int group_w = n * WINDOW_ITEM_WIDTH / 2;
	int group_h = 2 * WINDOW_ITEM_HEIGHT;
	int group_x = x + (w - group_w) / 2;
	int group_y = y + (h - group_h) / 2;
	ClutterActor *item;
	float item_x;
	float item_y;

	for (int i = 0; i < n; i++) {
		item   = window_items[symlist[i]-1];
		item_x = (float)group_x + (float)i * (float)WINDOW_ITEM_WIDTH / 2.0f;
		item_y = (float)group_y + (i % 2 == 0 ? 0.0f : (float)WINDOW_ITEM_HEIGHT);
		clutter_actor_animate(item, CLUTTER_EASE_IN_OUT_CUBIC, duration,
		                      "x", item_x,
				      "y", item_y,
				      NULL);
	}
}

