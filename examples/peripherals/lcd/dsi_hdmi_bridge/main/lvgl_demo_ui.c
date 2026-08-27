/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "lvgl.h"

#define EXAMPLE_COLOR_FORMAT   "RGB565"

static void strip_default_obj_style(lv_obj_t *obj)
{
    lv_obj_set_scrollable(obj, false);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_shadow_width(obj, 0, 0);
}

static void gauge_anim_cb(void *var, int32_t value)
{
    lv_obj_t *arc = var;
    lv_arc_set_value(arc, value);
    lv_label_set_text_fmt((lv_obj_t *)lv_obj_get_user_data(arc), "%" LV_PRId32 "%%", value);
}

static void create_header(lv_obj_t *parent, int32_t hor_res, int32_t ver_res, const lv_font_t *font)
{
    lv_obj_t *header = lv_obj_create(parent);
    strip_default_obj_style(header);
    lv_obj_set_size(header, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x151A22), 0);
    lv_obj_set_style_text_color(header, lv_color_white(), 0);
    lv_obj_set_style_pad_hor(header, ver_res / 24, 0);
    lv_obj_set_style_pad_ver(header, ver_res / 36, 0);

    static int32_t cols[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t rows[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(header, cols, rows);

    lv_obj_t *left = lv_label_create(header);
    lv_obj_set_style_text_font(left, font, 0);
    lv_obj_set_style_text_color(left, lv_color_white(), 0);
    lv_label_set_text(left, "MIPI DSI to HDMI");
    lv_obj_set_grid_cell(left, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_t *right = lv_label_create(header);
    lv_obj_set_style_text_font(right, font, 0);
    lv_obj_set_style_text_color(right, lv_color_white(), 0);
    lv_label_set_text_fmt(right, "%" LV_PRId32 "x%" LV_PRId32 "  %s", hor_res, ver_res, EXAMPLE_COLOR_FORMAT);
    lv_obj_set_grid_cell(right, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
}

static lv_obj_t *create_gauge_card(lv_obj_t *parent, const char *name, lv_color_t color,
                                   int32_t gauge_size, int32_t arc_width,
                                   const lv_font_t *font_value, const lv_font_t *font_caption)
{
    lv_obj_t *card = lv_obj_create(parent);
    strip_default_obj_style(card);
    lv_obj_set_flex_grow(card, 1);
    lv_obj_set_height(card, LV_PCT(100));
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x171E28), 0);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_set_style_pad_all(card, 16, 0);
    lv_obj_set_style_pad_row(card, 12, 0);

    lv_obj_t *arc = lv_arc_create(card);
    lv_obj_set_size(arc, gauge_size, gauge_size);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_value(arc, 0);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_clickable(arc, false);
    lv_obj_set_style_arc_width(arc, arc_width, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, arc_width, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x2A3140), LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_INDICATOR);

    lv_obj_t *value = lv_label_create(arc);
    lv_obj_set_style_text_font(value, font_value, 0);
    lv_obj_set_style_text_color(value, lv_color_white(), 0);
    lv_label_set_text(value, "0%");
    lv_obj_center(value);
    lv_obj_set_user_data(arc, value);

    lv_obj_t *caption = lv_label_create(card);
    lv_obj_set_style_text_font(caption, font_caption, 0);
    lv_obj_set_style_text_color(caption, lv_color_hex(0xA8B0C0), 0);
    lv_label_set_text(caption, name);

    return arc;
}

static void create_color_bars(lv_obj_t *parent, int32_t height)
{
    static const uint32_t colors[] = {
        0xFFFFFF, 0xFFFF00, 0x00FFFF, 0x00FF00,
        0xFF00FF, 0xFF0000, 0x0000FF, 0x3C3C3C,
    };

    lv_obj_t *row = lv_obj_create(parent);
    strip_default_obj_style(row);
    lv_obj_set_size(row, LV_PCT(100), height);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(row, 0, 0);

    for (size_t i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
        lv_obj_t *bar = lv_obj_create(row);
        strip_default_obj_style(bar);
        lv_obj_set_flex_grow(bar, 1);
        lv_obj_set_height(bar, LV_PCT(100));
        lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(bar, lv_color_hex(colors[i]), 0);
    }
}

void example_lvgl_demo_ui(lv_display_t *disp)
{
    const int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    const int32_t ver_res = lv_display_get_vertical_resolution(disp);
    const bool is_1080p = (ver_res >= 1000);
    const lv_font_t *font_header = is_1080p ? &lv_font_montserrat_48 : &lv_font_montserrat_28;
    const lv_font_t *font_value = &lv_font_montserrat_48;
    const lv_font_t *font_caption = is_1080p ? &lv_font_montserrat_28 : &lv_font_montserrat_16;
    const int32_t pad = is_1080p ? 32 : 20;
    const int32_t gauge_size = ver_res * 42 / 100;
    const int32_t arc_width = is_1080p ? 24 : 16;

    lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                          true, font_caption);

    lv_obj_t *scr = lv_display_get_screen_active(disp);
    lv_obj_set_scrollable(scr, false);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0B0E12), 0);
    lv_obj_set_style_text_color(scr, lv_color_white(), 0);
    lv_obj_set_style_border_width(scr, 0, 0);
    lv_obj_set_style_radius(scr, 0, 0);
    lv_obj_set_style_pad_all(scr, 0, 0);
    lv_obj_set_style_pad_row(scr, 0, 0);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);

    create_header(scr, hor_res, ver_res, font_header);

    lv_obj_t *gauges = lv_obj_create(scr);
    strip_default_obj_style(gauges);
    lv_obj_set_width(gauges, LV_PCT(100));
    lv_obj_set_flex_grow(gauges, 1);
    lv_obj_set_flex_flow(gauges, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(gauges, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(gauges, pad, 0);
    lv_obj_set_style_pad_column(gauges, pad, 0);

    static const char *names[] = {"Video", "Render", "Memory"};
    static const lv_palette_t palette[] = {LV_PALETTE_BLUE, LV_PALETTE_RED, LV_PALETTE_GREEN};
    static const uint32_t durations[] = {4100, 2600, 2800};
    static const uint32_t playbacks[] = {2700, 3200, 1800};

    lv_anim_t animation;
    lv_anim_init(&animation);
    lv_anim_set_values(&animation, 20, 100);
    lv_anim_set_repeat_count(&animation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&animation, gauge_anim_cb);

    for (int i = 0; i < 3; i++) {
        lv_obj_t *arc = create_gauge_card(gauges, names[i], lv_palette_main(palette[i]),
                                          gauge_size, arc_width, font_value, font_caption);
        lv_anim_set_var(&animation, arc);
        lv_anim_set_duration(&animation, durations[i]);
        lv_anim_set_playback_duration(&animation, playbacks[i]);
        lv_anim_start(&animation);
    }

    create_color_bars(scr, ver_res * 12 / 100);
}
