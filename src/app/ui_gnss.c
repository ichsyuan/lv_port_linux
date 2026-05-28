#include "ui_gnss.h"

static lv_obj_t *s_led_fix;
static lv_obj_t *s_lbl_lat;
static lv_obj_t *s_lbl_lon;
static lv_obj_t *s_lbl_speed;
static lv_obj_t *s_lbl_sats;
static lv_obj_t *s_lbl_time;
static lv_obj_t *s_lbl_date;

void ui_gnss_create(lv_obj_t *parent)
{
    /* Landscape: info on left (280px), right side reserved for future map */
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *left = lv_obj_create(parent);
    lv_obj_set_size(left, 280, LV_PCT(100));
    lv_obj_set_flex_flow(left, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(left, 8, 0);
    lv_obj_set_style_pad_row(left, 6, 0);
    lv_obj_clear_flag(left, LV_OBJ_FLAG_SCROLLABLE);

    /* Fix status row */
    lv_obj_t *fix_row = lv_obj_create(left);
    lv_obj_set_size(fix_row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(fix_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(fix_row, 2, 0);
    lv_obj_set_style_pad_column(fix_row, 6, 0);
    lv_obj_clear_flag(fix_row, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_fix = lv_label_create(fix_row);
    lv_label_set_text(lbl_fix, "Fix:");
    lv_obj_set_style_text_font(lbl_fix, &lv_font_montserrat_16, 0);
    s_led_fix = lv_led_create(fix_row);
    lv_led_set_color(s_led_fix, lv_palette_main(LV_PALETTE_RED));
    lv_led_set_brightness(s_led_fix, 200);
    lv_obj_set_size(s_led_fix, 16, 16);

    /* Data labels */
    s_lbl_lat   = lv_label_create(left);
    s_lbl_lon   = lv_label_create(left);
    s_lbl_speed = lv_label_create(left);
    s_lbl_sats  = lv_label_create(left);
    s_lbl_time  = lv_label_create(left);
    s_lbl_date  = lv_label_create(left);

    lv_label_set_text(s_lbl_lat,   "Lat:   ---");
    lv_label_set_text(s_lbl_lon,   "Lon:   ---");
    lv_label_set_text(s_lbl_speed, "Speed: --- kn");
    lv_label_set_text(s_lbl_sats,  "Sats:  -/-");
    lv_label_set_text(s_lbl_time,  "Time:  --:--:--.---");
    lv_label_set_text(s_lbl_date,  "Date:  ------");

    lv_obj_t *lbls[] = { s_lbl_lat, s_lbl_lon, s_lbl_speed,
                         s_lbl_sats, s_lbl_time, s_lbl_date };
    for(int i = 0; i < 6; i++) {
        lv_obj_set_style_text_font(lbls[i], &lv_font_montserrat_16, 0);
    }
}

void ui_gnss_update(const AccelGnssPacket_t *pkt)
{
    bool fix = (pkt->status == 'A');
    lv_led_set_color(s_led_fix,
                     fix ? lv_palette_main(LV_PALETTE_GREEN)
                         : lv_palette_main(LV_PALETTE_RED));

    lv_label_set_text_fmt(s_lbl_lat,   "Lat:   %.6f", pkt->lat);
    lv_label_set_text_fmt(s_lbl_lon,   "Lon:   %.6f", pkt->lon);
    lv_label_set_text_fmt(s_lbl_speed, "Speed: %.1f kn", (double)pkt->speed);
    lv_label_set_text_fmt(s_lbl_sats,  "Sats:  %d/%d",
                          (int)pkt->sats_used, (int)pkt->tot_sats);
    lv_label_set_text_fmt(s_lbl_time,  "Time:  %02d:%02d:%02d.%03d",
                          (int)pkt->hh, (int)pkt->mm,
                          (int)pkt->ss, (int)pkt->msec);
    lv_label_set_text_fmt(s_lbl_date,  "Date:  %06d", (int)pkt->date);
}
