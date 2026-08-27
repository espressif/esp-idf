/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "lvgl.h"

// The images are stored as LZ4-compressed LVGL binary images on the LittleFS
// partition, which is mounted at /littlefs and exposed to LVGL through its POSIX
// file system interface using 'S' as the drive letter.
#define IMG_LOGO_PATH "S:/littlefs/esp_logo.bin"
#define IMG_TEXT_PATH "S:/littlefs/esp_text.bin"

#define ARC_COUNT 3

static lv_obj_t *img_logo;
static lv_obj_t *img_text;
static lv_obj_t *arc[ARC_COUNT];
static lv_color_t arc_color[] = {
    LV_COLOR_MAKE(232, 87, 116),
    LV_COLOR_MAKE(126, 87, 162),
    LV_COLOR_MAKE(90, 202, 228),
};

/*******************************************************************************
 * Animation exec callbacks
 *
 * LVGL's built-in animation engine (lv_anim) drives these callbacks from within
 * lv_timer_handler(), so no separate software timer is needed and the animation
 * runs at the natural LVGL refresh rate.
 ******************************************************************************/

static void anim_arc_rotation_cb(void *var, int32_t v)
{
    lv_arc_set_rotation((lv_obj_t *)var, v);
}

static void anim_arc_angles_cb(void *var, int32_t v)
{
    // Keep a fixed 100 degree opening while the arc sweeps around the circle
    lv_arc_set_bg_angles((lv_obj_t *)var, v, v + 100);
}

static void anim_logo_offset_cb(void *var, int32_t v)
{
    LV_UNUSED(var);
    lv_obj_align(img_logo, LV_ALIGN_CENTER, 0, v);
}

static void anim_text_offset_cb(void *var, int32_t v)
{
    LV_UNUSED(var);
    lv_obj_align(img_text, LV_ALIGN_CENTER, 0, v);
}

static void anim_text_opa_cb(void *var, int32_t v)
{
    LV_UNUSED(var);
    lv_obj_set_style_image_opa(img_text, v, 0);
}

/*******************************************************************************
 * Animation setup helpers
 ******************************************************************************/

static void start_arc_rotation_anim(lv_obj_t *arc_obj, int32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc_obj);
    lv_anim_set_exec_cb(&a, anim_arc_rotation_cb);
    lv_anim_set_values(&a, 0, 360);
    lv_anim_set_duration(&a, 2000);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);

    // Sweep the arc opening across the circle as well
    lv_anim_t a2;
    lv_anim_init(&a2);
    lv_anim_set_var(&a2, arc_obj);
    lv_anim_set_exec_cb(&a2, anim_arc_angles_cb);
    lv_anim_set_values(&a2, 0, 360);
    lv_anim_set_duration(&a2, 1500);
    lv_anim_set_delay(&a2, delay);
    lv_anim_set_repeat_count(&a2, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a2, lv_anim_path_linear);
    lv_anim_start(&a2);
}

static void stop_and_remove_arcs(void)
{
    for (int i = 0; i < ARC_COUNT; i++) {
        if (arc[i]) {
            lv_anim_del(arc[i], NULL);
            lv_obj_delete(arc[i]);
            arc[i] = NULL;
        }
    }
}

static void finish_arc_phase(lv_anim_t *a)
{
    LV_UNUSED(a);
    stop_and_remove_arcs();

    // Show the Espressif text image below the logo and fade it in while the
    // two images move apart.
    img_text = lv_image_create(lv_screen_active());
    lv_image_set_src(img_text, IMG_TEXT_PATH);
    lv_obj_set_style_image_opa(img_text, 0, 0);

    lv_anim_t off;
    lv_anim_init(&off);
    lv_anim_set_var(&off, NULL);
    lv_anim_set_duration(&off, 800);
    lv_anim_set_path_cb(&off, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&off, anim_logo_offset_cb);
    lv_anim_set_values(&off, 0, -40);
    lv_anim_start(&off);

    lv_anim_t off2;
    lv_anim_init(&off2);
    lv_anim_set_var(&off2, NULL);
    lv_anim_set_duration(&off2, 800);
    lv_anim_set_delay(&off2, 150);
    lv_anim_set_path_cb(&off2, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&off2, anim_text_offset_cb);
    lv_anim_set_values(&off2, 0, 55);
    lv_anim_start(&off2);

    lv_anim_t opa;
    lv_anim_init(&opa);
    lv_anim_set_var(&opa, NULL);
    lv_anim_set_duration(&opa, 800);
    lv_anim_set_delay(&opa, 150);
    lv_anim_set_path_cb(&opa, lv_anim_path_ease_in);
    lv_anim_set_exec_cb(&opa, anim_text_opa_cb);
    lv_anim_set_values(&opa, 0, 255);
    lv_anim_start(&opa);
}

static void start_animation(void)
{
    // Align the logo at the center
    lv_obj_center(img_logo);

    // Create the three spinning arcs around the logo
    for (int i = 0; i < ARC_COUNT; i++) {
        arc[i] = lv_arc_create(lv_screen_active());
        lv_obj_set_size(arc[i], 220 - 30 * i, 220 - 30 * i);
        lv_arc_set_bg_angles(arc[i], 120 * i, 120 * i + 100);
        lv_arc_set_value(arc[i], 0);
        lv_obj_remove_style(arc[i], NULL, LV_PART_KNOB);
        lv_obj_set_style_arc_width(arc[i], 10, 0);
        lv_obj_set_style_arc_color(arc[i], arc_color[i], 0);
        lv_obj_center(arc[i]);
        start_arc_rotation_anim(arc[i], i * 200);
    }

    // After the arcs have been spinning for a while, remove them and reveal the text
    lv_anim_t done;
    lv_anim_init(&done);
    lv_anim_set_var(&done, NULL);
    lv_anim_set_duration(&done, 1);
    lv_anim_set_delay(&done, 2200);
    lv_anim_set_exec_cb(&done, NULL);
    lv_anim_set_completed_cb(&done, finish_arc_phase);
    lv_anim_start(&done);
}

void example_lvgl_demo_ui(lv_display_t *disp)
{
    lv_obj_t *scr = lv_display_get_screen_active(disp);

    // Create the logo image, loaded from the LittleFS partition
    img_logo = lv_image_create(scr);
    lv_image_set_src(img_logo, IMG_LOGO_PATH);

    start_animation();
}
