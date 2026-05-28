#ifndef RPMSG_CLIENT_H
#define RPMSG_CLIENT_H

#include <stdint.h>

/* Open device (e.g. "/dev/ttyRPMSG0") and configure raw termios.
 * Returns 0 on success, -1 on error. */
int  rpmsg_client_open(const char *dev_path);

/* Send a single-byte command. Thread-safe (uses write() syscall).
 *   's' 0x73 — start streaming
 *   'e' 0x65 — stop streaming
 *   'r' 0x72 — recalibrate
 *   'c' 0x63 — self-test
 * Returns 0 on success, -1 on error. */
int  rpmsg_client_send_cmd(uint8_t cmd);

/* Reader thread function: blocking read loop; exits when fd is closed.
 * Signature matches pthread_create(). */
void *rpmsg_reader_thread_fn(void *arg);

/* Send 'e' then close the fd. Async-signal-safe. */
void rpmsg_client_stop(void);

#endif
