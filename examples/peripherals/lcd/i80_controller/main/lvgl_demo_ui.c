/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <math.h>
#include "sdkconfig.h"
#include "lvgl.h"

#ifndef PI
#define PI  (3.14159f)
#endif

#if CONFIG_EXAMPLE_LCD_IMAGE_FROM_EMBEDDED_BINARY
// LVGL image declare
LV_IMG_DECLARE(esp_logo)
LV_IMG_DECLARE(esp_text)
#endif // CONFIG_EXAMPLE_LCD_IMAGE_FROM_EMBEDDED_BINARY

typedef struct {
    lv_obj_t *scr;
    int count_val;
} my_timer_context_t;

static my_timer_context_t my_tim_ctx;
static lv_obj_t *btn;
static lv_obj_t *arc[3];
static lv_obj_t *img_logo = NULL;
static lv_obj_t *img_text = NULL;
static lv_color_t arc_color[] = {
    LV_COLOR_MAKE(232, 87, 116),
    LV_COLOR_MAKE(126, 87, 162),
    LV_COLOR_MAKE(90, 202, 228),
};

static void anim_timer_cb(lv_timer_t *timer)
{
    my_timer_context_t *timer_ctx = (my_timer_context_t *) timer->user_data;
    int count = timer_ctx->count_val;
    lv_obj_t *scr = timer_ctx->scr;

    // Play arc animation
    if (count < 90) {
        lv_coord_t arc_start = count > 0 ? (1 - cosf(count / 180.0f * PI)) * 270 : 0;
        lv_coord_t arc_len = (sinf(count / 180.0f * PI) + 1) * 135;

        for (size_t i = 0; i < sizeof(arc) / sizeof(arc[0]); i++) {
            lv_arc_set_bg_angles(arc[i], arc_start, arc_len);
            lv_arc_set_rotation(arc[i], (count + 120 * (i + 1)) % 360);
        }
    }

    // Delete arcs when animation finished
    if (count == 90) {
        for (size_t i = 0; i < sizeof(arc) / sizeof(arc[0]); i++) {
            lv_obj_del(arc[i]);
        }

        // Create new image and make it transparent
        img_text = lv_img_create(scr);
#if CONFIG_EXAMPLE_LCD_IMAGE_FROM_FILE_SYSTEM
        lv_img_set_src(img_text, "S:/spiffs/esp_text.png");
#elif CONFIG_EXAMPLE_LCD_IMAGE_FROM_EMBEDDED_BINARY
        lv_img_set_src(img_text, &esp_text);
#endif
        lv_obj_set_style_img_opa(img_text, 0, 0);
    }

    // Move images when arc animation finished
    if ((count >= 100) && (count <= 180)) {
        lv_coord_t offset = (sinf((count - 140) * 2.25f / 90.0f) + 1) * 20.0f;
        lv_obj_align(img_logo, LV_ALIGN_CENTER, 0, -offset);
        lv_obj_align(img_text, LV_ALIGN_CENTER, 0, 2 * offset);
        lv_obj_set_style_img_opa(img_text, offset / 40.0f * 255, 0);
    }

    // Delete timer when all animation finished
    if ((count += 5) == 220) {
        lv_timer_del(timer);

        // Enable button
        lv_obj_clear_state(btn, LV_STATE_DISABLED);
    } else {
        timer_ctx->count_val = count;
    }
}

static void start_animation(lv_obj_t *scr)
{
    // Align image
    lv_obj_center(img_logo);

    // Create arcs
    for (size_t i = 0; i < sizeof(arc) / sizeof(arc[0]); i++) {
        arc[i] = lv_arc_create(scr);

        // Set arc caption
        lv_obj_set_size(arc[i], 220 - 30 * i, 220 - 30 * i);
        lv_arc_set_bg_angles(arc[i], 120 * i, 10 + 120 * i);
        lv_arc_set_value(arc[i], 0);

        // Set arc style
        lv_obj_remove_style(arc[i], NULL, LV_PART_KNOB);
        lv_obj_set_style_arc_width(arc[i], 10, 0);
        lv_obj_set_style_arc_color(arc[i], arc_color[i], 0);

        // Make arc center
        lv_obj_center(arc[i]);
    }

    if (img_text) {
        lv_obj_del(img_text);
        img_text = NULL;
    }

    // Create timer for animation
    my_tim_ctx.count_val = -90;
    my_tim_ctx.scr = scr;
    lv_timer_create(anim_timer_cb, 20, &my_tim_ctx);

    // Disable button
    lv_obj_add_state(btn, LV_STATE_DISABLED);
}

static void btn_cb(lv_event_t *e)
{
    lv_obj_t *scr = lv_event_get_user_data(e);
    start_animation(scr);
}

void example_lvgl_demo_ui(lv_disp_t *disp)
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);

    // Create image
    img_logo = lv_img_create(scr);
#if CONFIG_EXAMPLE_LCD_IMAGE_FROM_FILE_SYSTEM
    lv_img_set_src(img_logo, "S:/spiffs/esp_logo.png");
#elif CONFIG_EXAMPLE_LCD_IMAGE_FROM_EMBEDDED_BINARY
    lv_img_set_src(img_logo, &esp_logo);
#endif
    btn = lv_btn_create(scr);
    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text_static(lbl, LV_SYMBOL_REFRESH" SHOW AGAIN");
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_20, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 30, -30);
    // Button event
    lv_obj_add_event_cb(btn, btn_cb, LV_EVENT_CLICKED, scr);

    start_animation(scr);
}
