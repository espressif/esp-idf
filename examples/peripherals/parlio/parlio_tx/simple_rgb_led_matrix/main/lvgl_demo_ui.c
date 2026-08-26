/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "lvgl.h"

void example_lvgl_demo_ui(lv_display_t *disp)
{
    lv_obj_t *scr = lv_display_get_screen_active(disp);
    static lv_style_t up_style;
    lv_style_init(&up_style);
    lv_obj_t *up_label = lv_label_create(scr);
    lv_style_set_text_color(&up_style, lv_palette_main(LV_PALETTE_RED));
    lv_obj_add_style(up_label, &up_style, 0);
    lv_label_set_text(up_label, "Hello World");
    lv_label_set_long_mode(up_label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    /* Size of the screen */
    lv_obj_set_width(up_label, lv_display_get_horizontal_resolution(disp));
    lv_obj_align_to(up_label, scr, LV_ALIGN_TOP_MID, 0, 0);

    static lv_style_t low_style;
    lv_style_init(&low_style);
    lv_obj_t *low_label = lv_label_create(scr);
    lv_style_set_text_color(&low_style, lv_palette_main(LV_PALETTE_GREEN));
    lv_obj_add_style(low_label, &low_style, 0);
    lv_label_set_text(low_label, LV_SYMBOL_WIFI LV_SYMBOL_GPS LV_SYMBOL_BATTERY_2 LV_SYMBOL_AUDIO);
    /* Size of the screen */
    lv_obj_set_width(low_label, lv_display_get_horizontal_resolution(disp));
    lv_obj_align_to(low_label, scr, LV_ALIGN_BOTTOM_MID, 0, 0);
}
