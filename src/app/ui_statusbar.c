#include "ui_statusbar.h"

static lv_obj_t *s_lbl_cpu;

void ui_statusbar_create(lv_obj_t *parent)
{
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_set_size(bar, LV_PCT(100), 28);
    lv_obj_set_style_radius(bar, 0, 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_pad_all(bar, 4, 0);
    lv_obj_set_style_pad_column(bar, 0, 0);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x1A1A2E), 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_cpu = lv_label_create(bar);
    lv_label_set_text(s_lbl_cpu, "CPU: --.-%");
    lv_obj_set_style_text_font(s_lbl_cpu, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(s_lbl_cpu, lv_palette_main(LV_PALETTE_CYAN), 0);

    lv_obj_t *title = lv_label_create(bar);
    lv_label_set_text(title, "VIC3DA UI");
    lv_obj_set_flex_grow(title, 1);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title, lv_palette_lighten(LV_PALETTE_GREY, 2), 0);
}

void ui_statusbar_update_cpu(float pct)
{
    lv_label_set_text_fmt(s_lbl_cpu, "CPU: %.1f%%", (double)pct);

    lv_color_t c;
    if(pct > 80.0f)      c = lv_palette_main(LV_PALETTE_RED);
    else if(pct > 50.0f) c = lv_palette_main(LV_PALETTE_YELLOW);
    else                 c = lv_palette_main(LV_PALETTE_CYAN);

    lv_obj_set_style_text_color(s_lbl_cpu, c, 0);
}
