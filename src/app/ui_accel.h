#ifndef UI_ACCEL_H
#define UI_ACCEL_H

#include "lvgl/lvgl.h"
#include "packet_parser.h"

void ui_accel_create(lv_obj_t *parent);
void ui_accel_update_chart(const AccelGnssPacket_t *pkt);   /* ~62Hz — chart only */
void ui_accel_update_labels(const AccelGnssPacket_t *pkt);  /* ~2Hz  — text labels */

#endif
