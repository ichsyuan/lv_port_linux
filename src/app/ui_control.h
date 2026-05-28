#ifndef UI_CONTROL_H
#define UI_CONTROL_H

#include "lvgl/lvgl.h"

/* Creates the fixed 56-px control bar inside parent.
 * Buttons: Start / Stop / Self-Test / Recalibrate */
void ui_control_create(lv_obj_t *parent);

#endif
