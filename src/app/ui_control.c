#include "ui_control.h"
#include "rpmsg_client.h"

static void btn_cb(lv_event_t *e)
{
    uint8_t cmd = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    rpmsg_client_send_cmd(cmd);
}

void ui_control_create(lv_obj_t *parent)
{
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_set_size(bar, LV_PCT(100), 56);
    lv_obj_set_style_pad_all(bar, 4, 0);
    lv_obj_set_style_pad_column(bar, 4, 0);
    lv_obj_set_style_radius(bar, 0, 0);
    lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    static const struct { const char *label; uint8_t cmd; } btns[] = {
        { LV_SYMBOL_PLAY     " Start",     0x73 },
        { LV_SYMBOL_STOP     " Stop",      0x65 },
        { LV_SYMBOL_SETTINGS " Self-Test", 0x63 },
        { LV_SYMBOL_REFRESH  " Recal",     0x72 },
    };

    for(int i = 0; i < 4; i++) {
        lv_obj_t *btn = lv_button_create(bar);
        lv_obj_set_flex_grow(btn, 1);
        lv_obj_set_height(btn, LV_PCT(100));
        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, btns[i].label);
        lv_obj_center(lbl);
        lv_obj_add_event_cb(btn, btn_cb, LV_EVENT_CLICKED,
                            (void *)(uintptr_t)btns[i].cmd);
    }
}
