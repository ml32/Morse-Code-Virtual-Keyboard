#define _POSIX_C_SOURCE 200112L
#include <time.h>

#include "key.h"

#include <string.h>
#define  XK_MISCELLANY
#define  XK_LATIN1
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <pthread.h>
#include <errno.h>

static Display *display;
static Window   rootwin;
static XEvent   xevent1, xevent2;

static pthread_t       event_thread;
static pthread_mutex_t nextevent_mutex;
static pthread_condattr_t nextevent_condattr;
static pthread_cond_t  nextevent_cond;
static uint8_t         nextevent_isready;
static key_in_event_t  nextevent = {
  .state = KEY_UP,
  .time  = 0
};

static void  key_next_X11(key_in_event_t *evt);
static void *key_thread(void *);

int key_init() {
	if (pthread_mutex_init(&nextevent_mutex, NULL) != 0) return -1;
	if (pthread_condattr_init(&nextevent_condattr) != 0) return -1;
	if (pthread_condattr_setclock(&nextevent_condattr, CLOCK_MONOTONIC) != 0) return -1;
	if (pthread_cond_init(&nextevent_cond, &nextevent_condattr) != 0) return -1;
	if (pthread_create(&event_thread, NULL, key_thread, NULL) != 0) return -1;
	return 0;
}

static void key_next_X11(key_in_event_t *evt) {
	memset(evt, 0, sizeof(key_in_event_t));
	for (;;) {
		XNextEvent(display, &xevent1);
		if (xevent1.type == KeyPress) {
			evt->state = KEY_DOWN;
			evt->time  = xevent1.xkey.time;
			return;
		}
		if (xevent1.type == KeyRelease) {
			if (XPending(display) > 0) {
				XPeekEvent(display, &xevent2);
				if (xevent2.type == KeyPress &&
				    xevent2.xkey.time == xevent1.xkey.time)
				{
					XNextEvent(display, &xevent2);
					continue;
				}
			}
			evt->state = KEY_UP;
			evt->time  = xevent1.xkey.time;
			return;
		}
	}
}

static void *key_thread(void *arg) {
	KeyCode  keycode;

	display = XOpenDisplay(NULL);
	rootwin = DefaultRootWindow(display);
	keycode = XKeysymToKeycode(display, XK_f);
	XGrabKey(display, keycode, AnyModifier,
	         rootwin, True, GrabModeAsync, GrabModeAsync);

	key_in_event_t evt;
	for (;;) {
		key_next_X11(&evt);
		pthread_mutex_lock(&nextevent_mutex);
		while (nextevent_isready) {
			pthread_cond_wait(&nextevent_cond, &nextevent_mutex);
		}
		nextevent         = evt;
		nextevent_isready = 1;
		pthread_cond_broadcast(&nextevent_cond);
		pthread_mutex_unlock(&nextevent_mutex);
	}
	return NULL; /* not reached, prevents compiler warning */
}

int key_next(key_in_event_t *evt, int timeout) {
	struct timespec start;
	struct timespec finish;
	uint64_t        msec;
	
	if (timeout >= 0) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		msec  = start.tv_sec * 1000 + start.tv_nsec / 1000000;
		msec += timeout;
		finish.tv_sec  = msec / 1000;
		finish.tv_nsec = (msec % 1000) * 1000000;
	}

	pthread_mutex_lock(&nextevent_mutex);
	if (timeout < 0) {
		while (!nextevent_isready)
			pthread_cond_wait(&nextevent_cond, &nextevent_mutex);
	} else {
		while (!nextevent_isready) {
			if (pthread_cond_timedwait(&nextevent_cond, &nextevent_mutex, &finish) == ETIMEDOUT) {
				pthread_mutex_unlock(&nextevent_mutex);
				return -1;
			}
		}
	}
	nextevent_isready = 0;
	*evt              = nextevent;
	pthread_cond_broadcast(&nextevent_cond);
	pthread_mutex_unlock(&nextevent_mutex);
	return 0;
}
