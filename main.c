#include <stdio.h>
#include "key.h"
#include "window.h"
#include "morse.h"
#include <pthread.h>

static pthread_t morse_thread;

static void* morse_thread_main(void *_) {
	morse_main();
	return NULL;
}

int main(int argc, char **argv) {
	if (morse_init() < 0) {
		return -1;
	}
	if (key_init() < 0) {
		return -1;
	}
	if (window_init(&argc, &argv) < 0) {
		return -1;
	}

	pthread_create(&morse_thread, NULL, &morse_thread_main, NULL);
	window_main();
	return 0;
}


