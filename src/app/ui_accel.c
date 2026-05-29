#include "ui_accel.h"
#include "ui_theme.h"

static lv_obj_t          *s_lbl_filtered;
static lv_obj_t          *s_lbl_raw;

static lv_obj_t          *s_chart_filt;
static lv_chart_series_t *s_ser_fx, *s_ser_fy, *s_ser_fz;

static lv_obj_t          *s_chart_raw;
static lv_chart_series_t *s_ser_rx, *s_ser_ry, *s_ser_rz;

static lv_obj_t *make_chart(lv_obj_t *parent, const char *label_text)
{
    lv_obj_t *wrap = lv_obj_create(parent);
    lv_obj_set_size(wrap, LV_PCT(100), 0);
    lv_obj_set_flex_grow(wrap, 1);
    lv_obj_set_flex_flow(wrap, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(wrap, 2, 0);
    lv_obj_set_style_pad_row(wrap, 2, 0);
    lv_obj_clear_flag(wrap, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl = lv_label_create(wrap);
    lv_label_set_text(lbl, label_text);
    lv_obj_set_style_text_font(lbl, UI_FONT_SMALL, 0);
    lv_obj_set_style_text_color(lbl, lv_palette_lighten(LV_PALETTE_GREY, 1), 0);

    lv_obj_t *chart = lv_chart_create(wrap);
    lv_obj_set_size(chart, LV_PCT(100), 0);
    lv_obj_set_flex_grow(chart, 1);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, UI_CHART_POINTS);
    lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, -2000, 2000);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR);
    return chart;
}

void ui_accel_create(lv_obj_t *parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(parent, 0, 0);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    /* ---- Left panel: text values ---- */
    lv_obj_t *left = lv_obj_create(parent);
    lv_obj_set_size(left, UI_LEFT_PANEL_NARROW, LV_PCT(100));
    lv_obj_set_flex_flow(left, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(left, 8, 0);
    lv_obj_set_style_pad_row(left, 10, 0);
    lv_obj_clear_flag(left, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_filtered = lv_label_create(left);
    lv_label_set_text(s_lbl_filtered, "Filtered:\nX:--- Y:--- Z:---");
    lv_obj_set_style_text_font(s_lbl_filtered, UI_FONT_SMALL, 0);
    lv_obj_set_style_text_color(s_lbl_filtered, lv_palette_main(LV_PALETTE_CYAN), 0);
    lv_label_set_long_mode(s_lbl_filtered, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(s_lbl_filtered, LV_PCT(100));

    s_lbl_raw = lv_label_create(left);
    lv_label_set_text(s_lbl_raw, "Raw:\nX:--- Y:--- Z:---");
    lv_obj_set_style_text_font(s_lbl_raw, UI_FONT_SMALL, 0);
    lv_obj_set_style_text_color(s_lbl_raw, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_label_set_long_mode(s_lbl_raw, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(s_lbl_raw, LV_PCT(100));

    /* Legend */
    lv_obj_t *leg = lv_label_create(left);
    lv_label_set_text(leg, "X=R/Org  Y=G/Yel  Z=B/Cyn");
    lv_obj_set_style_text_font(leg, UI_FONT_SMALL, 0);
    lv_obj_set_style_text_color(leg, lv_palette_darken(LV_PALETTE_GREY, 1), 0);
    lv_label_set_long_mode(leg, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(leg, LV_PCT(100));

    /* ---- Right panel: dual charts (flex-grow:1) ---- */
    lv_obj_t *right = lv_obj_create(parent);
    lv_obj_set_height(right, LV_PCT(100));
    lv_obj_set_flex_grow(right, 1);
    lv_obj_set_flex_flow(right, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(right, 4, 0);
    lv_obj_set_style_pad_row(right, 4, 0);
    lv_obj_clear_flag(right, LV_OBJ_FLAG_SCROLLABLE);

    /* Filtered chart (top half) */
    s_chart_filt = make_chart(right, "Filtered (mg)");
    s_ser_fx = lv_chart_add_series(s_chart_filt, lv_palette_main(LV_PALETTE_RED),   LV_CHART_AXIS_PRIMARY_Y);
    s_ser_fy = lv_chart_add_series(s_chart_filt, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    s_ser_fz = lv_chart_add_series(s_chart_filt, lv_palette_main(LV_PALETTE_BLUE),  LV_CHART_AXIS_PRIMARY_Y);

    /* Raw chart (bottom half) */
    s_chart_raw = make_chart(right, "Raw (mg)");
    s_ser_rx = lv_chart_add_series(s_chart_raw, lv_palette_main(LV_PALETTE_ORANGE),     LV_CHART_AXIS_PRIMARY_Y);
    s_ser_ry = lv_chart_add_series(s_chart_raw, lv_palette_main(LV_PALETTE_YELLOW),     LV_CHART_AXIS_PRIMARY_Y);
    s_ser_rz = lv_chart_add_series(s_chart_raw, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_CHART_AXIS_PRIMARY_Y);
}

static void auto_scale_chart(lv_obj_t *chart, lv_chart_series_t *sers[], int n_sers,
                              int32_t min_margin)
{
    int32_t mn = INT32_MAX, mx = INT32_MIN;
    uint32_t count = lv_chart_get_point_count(chart);
    for (int s = 0; s < n_sers; s++) {
        const int32_t *arr = lv_chart_get_series_y_array(chart, sers[s]);
        for (uint32_t i = 0; i < count; i++) {
            if (arr[i] == LV_CHART_POINT_NONE) continue;
            if (arr[i] < mn) mn = arr[i];
            if (arr[i] > mx) mx = arr[i];
        }
    }
    if (mn == INT32_MAX) return;
    int32_t margin = (mx - mn) / 10;
    if (margin < min_margin) margin = min_margin;
    lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, mn - margin, mx + margin);
}

void ui_accel_update_chart(const AccelGnssPacket_t *pkt)
{
    lv_chart_set_next_value(s_chart_filt, s_ser_fx, (int32_t)pkt->acc[3]);
    lv_chart_set_next_value(s_chart_filt, s_ser_fy, (int32_t)pkt->acc[4]);
    lv_chart_set_next_value(s_chart_filt, s_ser_fz, (int32_t)pkt->acc[5]);
    lv_chart_set_next_value(s_chart_raw,  s_ser_rx, (int32_t)pkt->acc[0]);
    lv_chart_set_next_value(s_chart_raw,  s_ser_ry, (int32_t)pkt->acc[1]);
    lv_chart_set_next_value(s_chart_raw,  s_ser_rz, (int32_t)pkt->acc[2]);

    lv_chart_series_t *filt_sers[] = {s_ser_fx, s_ser_fy, s_ser_fz};
    auto_scale_chart(s_chart_filt, filt_sers, 3, 50);

    lv_chart_series_t *raw_sers[] = {s_ser_rx, s_ser_ry, s_ser_rz};
    auto_scale_chart(s_chart_raw, raw_sers, 3, 50);
}

void ui_accel_update_labels(const AccelGnssPacket_t *pkt)
{
    lv_label_set_text_fmt(s_lbl_filtered,
        "Filtered:\nX:%+.3f  Y:%+.3f  Z:%+.3f mg",
        (double)pkt->acc[3], (double)pkt->acc[4], (double)pkt->acc[5]);
    lv_label_set_text_fmt(s_lbl_raw,
        "Raw:\nX:%+.3f  Y:%+.3f  Z:%+.3f mg",
        (double)pkt->acc[0], (double)pkt->acc[1], (double)pkt->acc[2]);
}
