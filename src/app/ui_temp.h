#ifndef UI_TEMP_H
#define UI_TEMP_H

#include "lvgl/lvgl.h"
#include "packet_parser.h"

void ui_temp_create(lv_obj_t *parent);
void ui_temp_update(const TemperaturePacket_t *pkt);

#endif
