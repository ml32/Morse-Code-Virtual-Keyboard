#ifndef WINDOW_H
#define WINDOW_H

int window_init(int*, char***);
void window_main();
void window_update(int element_time); /* to be called from the morse_main only */

#endif
