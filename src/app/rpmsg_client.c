#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdatomic.h>
#include "rpmsg_client.h"
#include "packet_parser.h"

static atomic_int g_fd = -1;

int rpmsg_client_open(const char *dev_path)
{
    int fd = open(dev_path, O_RDWR | O_NOCTTY);
    if(fd < 0) {
        perror("rpmsg_client_open");
        return -1;
    }

    struct termios tty;
    if(tcgetattr(fd, &tty) != 0) {
        /* ttyRPMSG may not support tcgetattr; treat as non-fatal */
        perror("tcgetattr (non-fatal)");
    } else {
        cfmakeraw(&tty);
        tty.c_cc[VMIN]  = 1;  /* block until at least 1 byte */
        tty.c_cc[VTIME] = 0;  /* no read timeout */
        tcsetattr(fd, TCSANOW, &tty);
    }

    atomic_store(&g_fd, fd);
    return 0;
}

int rpmsg_client_send_cmd(uint8_t cmd)
{
    int fd = atomic_load(&g_fd);
    if(fd < 0) return -1;
    ssize_t n = write(fd, &cmd, 1);
    return (n == 1) ? 0 : -1;
}

void *rpmsg_reader_thread_fn(void *arg)
{
    (void)arg;
    packet_parser_reset();

    uint8_t byte;
    while(1) {
        ssize_t n = read(atomic_load(&g_fd), &byte, 1);
        if(n <= 0) break;
        packet_parser_feed(byte);
    }
    return NULL;
}

void rpmsg_client_stop(void)
{
    int fd = atomic_load(&g_fd);
    if(fd < 0) return;
    uint8_t e = 'e';
    write(fd, &e, 1);   /* async-signal-safe */
    close(fd);
    atomic_store(&g_fd, -1);
}
