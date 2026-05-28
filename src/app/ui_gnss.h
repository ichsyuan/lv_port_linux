#ifndef UI_GNSS_H
#define UI_GNSS_H

#include "lvgl/lvgl.h"
#include "packet_parser.h"

void ui_gnss_create(lv_obj_t *parent);
void ui_gnss_update(const AccelGnssPacket_t *pkt);

#endif
