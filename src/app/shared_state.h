#ifndef SHARED_STATE_H
#define SHARED_STATE_H

#include <stdbool.h>
#include "packet_parser.h"

typedef struct {
    AccelGnssPacket_t   accel;
    bool                accel_fresh;

    DRSINTPacket_t      drsint;
    bool                drsint_fresh;

    SelfTestPacket_t    selftest;
    bool                selftest_fresh;

    TemperaturePacket_t temp;
    bool                temp_fresh;
} SharedSensorData_t;

void shared_state_init(void);
void shared_state_destroy(void);

/* Writer API — called from reader thread */
void shared_state_update_accel(const AccelGnssPacket_t *pkt);
void shared_state_update_drsint(const DRSINTPacket_t *pkt);
void shared_state_update_selftest(const SelfTestPacket_t *pkt);
void shared_state_update_temp(const TemperaturePacket_t *pkt);

/* Reader API — called from LVGL lv_timer callback (main thread only).
 * Copies latest data into *out and clears fresh flags.
 * Returns true if any field was fresh. */
bool shared_state_read(SharedSensorData_t *out);

#endif
