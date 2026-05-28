#include "ui_selftest.h"

static lv_obj_t *s_lbl_acc_status;
static lv_obj_t *s_lbl_acc_delta;
static lv_obj_t *s_lbl_offset;
static lv_obj_t *s_lbl_gnss_status;
static lv_obj_t *s_lbl_gnss_errcode;

static const char *const gnss_err_str[] = {
    "OK", "Gyro", "Acc", "Odo", "GNSS",
    "Pressure", "Mag", "Misalign", "Acc_KF", "Gyro_KF", "SW"
};

static lv_obj_t *make_panel(lv_obj_t *parent)
{
    lv_obj_t *p = lv_obj_create(parent);
    lv_obj_set_height(p, LV_PCT(100));
    lv_obj_set_flex_grow(p, 1);
    lv_obj_set_flex_flow(p, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(p, 8, 0);
    lv_obj_set_style_pad_row(p, 6, 0);
    lv_obj_clear_flag(p, LV_OBJ_FLAG_SCROLLABLE);
    return p;
}

void ui_selftest_create(lv_obj_t *parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(parent, 0, 0);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    /* ---- Left panel: ADXL355 ---- */
    lv_obj_t *left = make_panel(parent);

    lv_obj_t *t_acc = lv_label_create(left);
    lv_label_set_text(t_acc, "--- ADXL355 ---");
    lv_obj_set_style_text_font(t_acc, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(t_acc, lv_palette_main(LV_PALETTE_GREY), 0);

    s_lbl_acc_status = lv_label_create(left);
    lv_label_set_text(s_lbl_acc_status, "ACC: ---");
    lv_obj_set_style_text_font(s_lbl_acc_status, &lv_font_montserrat_24, 0);

    s_lbl_acc_delta = lv_label_create(left);
    lv_label_set_text(s_lbl_acc_delta, "Delta:\n(---, ---, ---) mg");
    lv_obj_set_style_text_font(s_lbl_acc_delta, &lv_font_montserrat_14, 0);

    s_lbl_offset = lv_label_create(left);
    lv_label_set_text(s_lbl_offset, "Offset:\n(---.---, ---.---, ---) mg");
    lv_obj_set_style_text_font(s_lbl_offset, &lv_font_montserrat_14, 0);

    /* ---- Right panel: GNSS ---- */
    lv_obj_t *right = make_panel(parent);

    lv_obj_t *t_gnss = lv_label_create(right);
    lv_label_set_text(t_gnss, "--- GNSS ---");
    lv_obj_set_style_text_font(t_gnss, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(t_gnss, lv_palette_main(LV_PALETTE_GREY), 0);

    s_lbl_gnss_status = lv_label_create(right);
    lv_label_set_text(s_lbl_gnss_status, "GNSS IMU: ---");
    lv_obj_set_style_text_font(s_lbl_gnss_status, &lv_font_montserrat_24, 0);

    s_lbl_gnss_errcode = lv_label_create(right);
    lv_label_set_text(s_lbl_gnss_errcode, "Error: ---");
    lv_obj_set_style_text_font(s_lbl_gnss_errcode, &lv_font_montserrat_14, 0);
}

void ui_selftest_update(const SelfTestPacket_t *pkt)
{
    if(pkt->isAccSelfTestFail == 0) {
        lv_label_set_text(s_lbl_acc_status, "ACC: PASS " LV_SYMBOL_OK);
        lv_obj_set_style_text_color(s_lbl_acc_status, lv_palette_main(LV_PALETTE_GREEN), 0);
    } else {
        lv_label_set_text(s_lbl_acc_status, "ACC: FAIL " LV_SYMBOL_CLOSE);
        lv_obj_set_style_text_color(s_lbl_acc_status, lv_palette_main(LV_PALETTE_RED), 0);
    }

    lv_label_set_text_fmt(s_lbl_acc_delta,
        "Delta:\n(%.1f, %.1f, %.1f) mg",
        (double)pkt->selfTestAcc[0],
        (double)pkt->selfTestAcc[1],
        (double)pkt->selfTestAcc[2]);

    lv_label_set_text_fmt(s_lbl_offset,
        "Offset:\n(%.3f, %.3f, %.3f) mg",
        (double)pkt->offsetFilterAcc[0],
        (double)pkt->offsetFilterAcc[1],
        (double)pkt->offsetFilterAcc[2]);

    bool gnss_pass = (pkt->gnssImuSelfTestResult == 0);
    if(gnss_pass) {
        lv_label_set_text(s_lbl_gnss_status, "GNSS: PASS " LV_SYMBOL_OK);
        lv_obj_set_style_text_color(s_lbl_gnss_status, lv_palette_main(LV_PALETTE_GREEN), 0);
    } else {
        lv_label_set_text(s_lbl_gnss_status, "GNSS: FAIL " LV_SYMBOL_CLOSE);
        lv_obj_set_style_text_color(s_lbl_gnss_status, lv_palette_main(LV_PALETTE_RED), 0);
    }

    uint8_t err = pkt->gnssErrCode;
    const char *err_str = (err <= 10) ? gnss_err_str[err] : "Unknown";
    lv_label_set_text_fmt(s_lbl_gnss_errcode, "Error: %s (%u)", err_str, (unsigned)err);
}
