#include <string.h>
#define  XK_MISCELLANY
#define  XK_LATIN1
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <pthread.h>
#include "key.h"

static Display *display;
static Window   rootwin;
static XEvent   xevent1, xevent2;

static pthread_t       event_thread;
static pthread_mutex_t nextevent_mutex;
static pthread_cond_t  nextevent_cond;
static uint8_t         nextevent_isready;
static key_in_event_t  nextevent = 0;

static void  key_next_X11(key_in_event_t *evt);
static void *key_thread(void *);

int key_init() {
	pthread_mutex_init(&nextevent_mutex, NULL);
	pthread_cond_init(&nextevent_cond, NULL);
	pthread_create(&event_thread, NULL, key_thread, NULL);
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
}

int key_next(key_in_event_t *evt, int timeout) {
	struct timespec start;
	struct timespec finish;
	uint64_t        msec;
	
	if (timeout >= 0) {
		clock_gettime(CLOCK_REALTIME, &start);
		msec  = start.sec * 1000 + finish.nsec / 1000000;
		msec += timeout;
		finish.sec  = msec / 1000;
		finish.nsec = msec * 1000000;
	}

	pthread_mutex_lock(&nextevent_mutex);
	if (!nextevent_isready) {
		if (timeout < 0) {
			pthread_cond_wait(&nextevent_cond);
		} else {
			pthread_cond_timedwait(&nextevent_cond, &finish);
		}
		if (!nextevent_isready) {
			pthread_mutex_unlock(&nextevent_mutex);
			return -1;
		}
	}
	nextevent_isready = 0;
	*evt              = nextevent;
	pthread_mutex_unlock(&nextevent_mutex);
	return 0;
}
