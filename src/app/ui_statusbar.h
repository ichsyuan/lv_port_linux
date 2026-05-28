#ifndef UI_STATUSBAR_H
#define UI_STATUSBAR_H

#include "lvgl/lvgl.h"

void ui_statusbar_create(lv_obj_t *parent);
void ui_statusbar_update_cpu(float pct);

#endif
