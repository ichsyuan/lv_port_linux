#include "ui_temp.h"

static lv_obj_t          *s_lbl_temp_big;
static lv_obj_t          *s_lbl_raw;
static lv_obj_t          *s_chart;
static lv_chart_series_t *s_ser_temp;

void ui_temp_create(lv_obj_t *parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    /* ---- Left: big temperature reading (220px) ---- */
    lv_obj_t *left = lv_obj_create(parent);
    lv_obj_set_size(left, 220, LV_PCT(100));
    lv_obj_set_flex_flow(left, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(left, 8, 0);
    lv_obj_set_style_pad_row(left, 10, 0);
    lv_obj_clear_flag(left, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_temp_big = lv_label_create(left);
    lv_label_set_text(s_lbl_temp_big, "---.- °C");
    lv_obj_set_style_text_font(s_lbl_temp_big, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_align(s_lbl_temp_big, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(s_lbl_temp_big, LV_PCT(100));

    s_lbl_raw = lv_label_create(left);
    lv_label_set_text(s_lbl_raw, "Raw ADC: ---");
    lv_obj_set_style_text_font(s_lbl_raw, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(s_lbl_raw, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(s_lbl_raw, LV_PCT(100));

    /* ---- Right: 60-second history chart (flex-grow:1) ---- */
    lv_obj_t *right = lv_obj_create(parent);
    lv_obj_set_height(right, LV_PCT(100));
    lv_obj_set_flex_grow(right, 1);
    lv_obj_set_flex_flow(right, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(right, 4, 0);
    lv_obj_clear_flag(right, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *chart_lbl = lv_label_create(right);
    lv_label_set_text(chart_lbl, "Temperature History (60s)");
    lv_obj_set_style_text_font(chart_lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(chart_lbl, lv_palette_lighten(LV_PALETTE_GREY, 1), 0);

    s_chart = lv_chart_create(right);
    lv_obj_set_size(s_chart, LV_PCT(100), 0);
    lv_obj_set_flex_grow(s_chart, 1);
    lv_chart_set_type(s_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(s_chart, 60);
    lv_chart_set_axis_range(s_chart, LV_CHART_AXIS_PRIMARY_Y, 1500, 4000); /* °C × 100 */
    lv_chart_set_update_mode(s_chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_obj_set_style_size(s_chart, 0, 0, LV_PART_INDICATOR);

    s_ser_temp = lv_chart_add_series(s_chart, lv_palette_main(LV_PALETTE_ORANGE),
                                     LV_CHART_AXIS_PRIMARY_Y);
}

void ui_temp_update(const TemperaturePacket_t *pkt)
{
    lv_label_set_text_fmt(s_lbl_temp_big, "%.2f °C", (double)pkt->temp_celsius);
    lv_label_set_text_fmt(s_lbl_raw,      "Raw ADC: %u", (unsigned)pkt->temp_raw);
    lv_chart_set_next_value(s_chart, s_ser_temp, (int32_t)(pkt->temp_celsius * 100.0f));
}
