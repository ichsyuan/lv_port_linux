#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

/* Initialize — takes a baseline /proc/stat sample */
void  cpu_monitor_init(void);

/* Returns CPU usage percentage (0.0–100.0) since last call.
 * First call after init() returns 0. Safe to call from LVGL main thread. */
float cpu_monitor_get_pct(void);

#endif
