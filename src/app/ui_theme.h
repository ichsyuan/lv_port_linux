#ifndef UI_THEME_H
#define UI_THEME_H

#include "lvgl/lvgl.h"

#ifndef UI_HOR_RES
#define UI_HOR_RES 800
#endif

#if UI_HOR_RES <= 320
/* Ka-Ro TXMP-1570: 320x240 */
#define UI_STATUSBAR_HEIGHT     18
#define UI_TABBAR_HEIGHT        28
#define UI_CONTROLBAR_HEIGHT    36
#define UI_LEFT_PANEL_NARROW    110
#define UI_LEFT_PANEL_WIDE      140
#define UI_CHART_POINTS         64
#define UI_CHART_UPDATE_EVERY   4    /* 16ms × 4  ≈ 16Hz → 64pts/~4s fill  */
#define UI_LED_SIZE             10
#define UI_FONT_SMALL           (&lv_font_montserrat_10)
#define UI_FONT_NORMAL          (&lv_font_montserrat_12)
#define UI_FONT_MEDIUM          (&lv_font_montserrat_16)
#define UI_FONT_BIG             (&lv_font_montserrat_24)
#else
/* STM32MP157F-DK2: 800x400 */
#define UI_STATUSBAR_HEIGHT     28
#define UI_TABBAR_HEIGHT        50
#define UI_CONTROLBAR_HEIGHT    56
#define UI_LEFT_PANEL_NARROW    220
#define UI_LEFT_PANEL_WIDE      280
#define UI_CHART_POINTS         256
#define UI_CHART_UPDATE_EVERY   1    /* 16ms × 1  ≈ 62Hz → 256pts/~4s fill */
#define UI_LED_SIZE             16
#define UI_FONT_SMALL           (&lv_font_montserrat_20)
#define UI_FONT_NORMAL          (&lv_font_montserrat_24)
#define UI_FONT_MEDIUM          (&lv_font_montserrat_32)
#define UI_FONT_BIG             (&lv_font_montserrat_48)
#endif

#endif /* UI_THEME_H */
