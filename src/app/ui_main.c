#include "ui_main.h"
#include "shared_state.h"
#include "cpu_monitor.h"
#include "ui_statusbar.h"
#include "ui_accel.h"
#include "ui_gnss.h"
#include "ui_temp.h"
#include "ui_selftest.h"
#include "ui_control.h"
#include "lvgl/lvgl.h"

#define LABEL_UPDATE_EVERY  31   /* 16ms × 31 ≈ 496ms ≈ 2Hz  */
#define CPU_UPDATE_EVERY    62   /* 16ms × 62 ≈ 992ms ≈ 1Hz  */

static uint32_t s_label_tick = 0;
static uint32_t s_cpu_tick   = 0;

static void sensor_poll_timer_cb(lv_timer_t *timer)
{
    (void)timer;

    bool do_label = (++s_label_tick >= LABEL_UPDATE_EVERY);
    if(do_label) s_label_tick = 0;

    bool do_cpu = (++s_cpu_tick >= CPU_UPDATE_EVERY);
    if(do_cpu) {
        s_cpu_tick = 0;
        ui_statusbar_update_cpu(cpu_monitor_get_pct());
    }

    SharedSensorData_t snap;
    if(!shared_state_read(&snap)) return;

    if(snap.accel_fresh) {
        ui_accel_update_chart(&snap.accel);
        if(do_label) ui_accel_update_labels(&snap.accel);
        if(do_label) ui_gnss_update(&snap.accel);
    }
    if(snap.temp_fresh)     ui_temp_update(&snap.temp);
    if(snap.selftest_fresh) ui_selftest_update(&snap.selftest);
}

void ui_main_init(void)
{
    lv_group_set_default(lv_group_create());
    cpu_monitor_init();

#if LV_USE_EVDEV
    /* Touch coordinate transform for CCW 90° SW rotation (portrait 480×800 → landscape 800×480).
     * ABS_X [0,479]=physical px, ABS_Y [0,799]=physical py.
     * CCW: lx = 799 - py, ly = px.
     * swap_axes=true: swapped_x=raw_y, swapped_y=raw_x.
     * calibration inverts swapped_x range [799→0] to output [0,799], passes swapped_y [0→479]. */
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev != NULL) {
        if(lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER) {
            lv_evdev_set_swap_axes(indev, true);
            lv_evdev_set_calibration(indev, 799, 0, 0, 479);
            break;
        }
        indev = lv_indev_get_next(indev);
    }
#endif

    lv_obj_t *root = lv_screen_active();
    lv_obj_set_layout(root, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    /* Status bar — 28px at top */
    ui_statusbar_create(root);

    /* TabView — flex-grow:1, fills remaining space */
    lv_obj_t *tv = lv_tabview_create(root);
    lv_tabview_set_tab_bar_position(tv, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tv, 50);
    lv_obj_set_width(tv, LV_PCT(100));
    lv_obj_set_height(tv, 0);
    lv_obj_set_flex_grow(tv, 1);

    ui_accel_create(lv_tabview_add_tab(tv, "Accel"));
    ui_gnss_create(lv_tabview_add_tab(tv, "GNSS"));
    ui_temp_create(lv_tabview_add_tab(tv, "Temp"));
    ui_selftest_create(lv_tabview_add_tab(tv, "Self-Test"));

    /* Control bar — fixed 56px at bottom */
    ui_control_create(root);

    lv_timer_create(sensor_poll_timer_cb, 16, NULL);
}
