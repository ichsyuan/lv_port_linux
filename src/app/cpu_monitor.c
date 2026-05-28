#include <stdio.h>
#include <stdint.h>
#include "cpu_monitor.h"

typedef struct { uint64_t total; uint64_t idle; } cpu_sample_t;

static cpu_sample_t s_prev;
static int          s_init = 0;

static cpu_sample_t read_sample(void)
{
    cpu_sample_t r = {0, 0};
    FILE *f = fopen("/proc/stat", "r");
    if(!f) return r;
    unsigned long long u, n, s, id, iw, irq, sirq;
    if(fscanf(f, "cpu %llu %llu %llu %llu %llu %llu %llu",
              &u, &n, &s, &id, &iw, &irq, &sirq) == 7) {
        r.total = (uint64_t)(u + n + s + id + iw + irq + sirq);
        r.idle  = (uint64_t)(id + iw);
    }
    fclose(f);
    return r;
}

void cpu_monitor_init(void)
{
    s_prev = read_sample();
    s_init = 1;
}

float cpu_monitor_get_pct(void)
{
    if(!s_init) { cpu_monitor_init(); return 0.0f; }
    cpu_sample_t cur = read_sample();
    uint64_t dt = cur.total - s_prev.total;
    uint64_t di = cur.idle  - s_prev.idle;
    s_prev = cur;
    if(dt == 0) return 0.0f;
    return (float)(dt - di) / (float)dt * 100.0f;
}
