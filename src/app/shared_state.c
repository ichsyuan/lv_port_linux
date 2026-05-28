#include <string.h>
#include <pthread.h>
#include "shared_state.h"

static SharedSensorData_t s_data;
static pthread_mutex_t    s_mutex;

void shared_state_init(void)
{
    memset(&s_data, 0, sizeof(s_data));
    pthread_mutex_init(&s_mutex, NULL);
}

void shared_state_destroy(void)
{
    pthread_mutex_destroy(&s_mutex);
}

void shared_state_update_accel(const AccelGnssPacket_t *pkt)
{
    pthread_mutex_lock(&s_mutex);
    s_data.accel       = *pkt;
    s_data.accel_fresh = true;
    pthread_mutex_unlock(&s_mutex);
}

void shared_state_update_drsint(const DRSINTPacket_t *pkt)
{
    pthread_mutex_lock(&s_mutex);
    s_data.drsint       = *pkt;
    s_data.drsint_fresh = true;
    pthread_mutex_unlock(&s_mutex);
}

void shared_state_update_selftest(const SelfTestPacket_t *pkt)
{
    pthread_mutex_lock(&s_mutex);
    s_data.selftest       = *pkt;
    s_data.selftest_fresh = true;
    pthread_mutex_unlock(&s_mutex);
}

void shared_state_update_temp(const TemperaturePacket_t *pkt)
{
    pthread_mutex_lock(&s_mutex);
    s_data.temp       = *pkt;
    s_data.temp_fresh = true;
    pthread_mutex_unlock(&s_mutex);
}

bool shared_state_read(SharedSensorData_t *out)
{
    pthread_mutex_lock(&s_mutex);
    *out = s_data;
    s_data.accel_fresh    = false;
    s_data.drsint_fresh   = false;
    s_data.selftest_fresh = false;
    s_data.temp_fresh     = false;
    pthread_mutex_unlock(&s_mutex);

    return out->accel_fresh || out->drsint_fresh ||
           out->selftest_fresh || out->temp_fresh;
}
