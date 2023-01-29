/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "lvgl.h"

void example_lvgl_demo_ui(lv_disp_t *disp)
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    lv_obj_t *up_label = lv_label_create(scr);
    lv_label_set_recolor(up_label, true);
    lv_label_set_text(up_label, "#FF0000 Hello# #0000FF World#");
    lv_label_set_long_mode(up_label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    /* Size of the screen */
    lv_obj_set_width(up_label, disp->driver->hor_res);
    lv_obj_align_to(up_label, scr, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *low_label = lv_label_create(scr);
    lv_label_set_text(low_label, LV_SYMBOL_WIFI LV_SYMBOL_GPS LV_SYMBOL_BATTERY_2 LV_SYMBOL_AUDIO);
    /* Size of the screen */
    lv_obj_set_width(low_label, disp->driver->hor_res);
    lv_obj_align_to(low_label, scr, LV_ALIGN_BOTTOM_MID, 0, 0);
}
