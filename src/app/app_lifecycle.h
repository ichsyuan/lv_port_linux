#ifndef APP_LIFECYCLE_H
#define APP_LIFECYCLE_H

#include <signal.h>

/* Defined in main.c; used by drm.c run_loop and signal handler */
extern volatile sig_atomic_t g_running;

#endif
