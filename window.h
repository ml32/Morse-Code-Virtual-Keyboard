#ifndef WINDOW_H
#define WINDOW_H

int window_init(int*, char***);
void window_main();

#include "morsetree.h"
void window_move_items(morse_sym_t*, uint8_t, int, int, int, int);

#endif
