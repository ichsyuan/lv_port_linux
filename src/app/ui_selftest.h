#ifndef UI_SELFTEST_H
#define UI_SELFTEST_H

#include "lvgl/lvgl.h"
#include "packet_parser.h"

void ui_selftest_create(lv_obj_t *parent);
void ui_selftest_update(const SelfTestPacket_t *pkt);

#endif
