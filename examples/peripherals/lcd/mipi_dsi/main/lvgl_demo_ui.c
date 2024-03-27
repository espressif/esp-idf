/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "lvgl.h"

static lv_style_t style_bullet;
static lv_obj_t *scale1;
static const lv_font_t *font_normal = &lv_font_montserrat_16;

static lv_obj_t *create_scale_box(lv_obj_t *parent, const char *text1, const char *text2, const char *text3)
{
    lv_obj_t *scale = lv_scale_create(parent);
    lv_obj_center(scale);
    lv_obj_set_size(scale, 600, 600);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_OUTER);
    lv_scale_set_label_show(scale, false);
    lv_scale_set_post_draw(scale, true);
    lv_obj_set_width(scale, LV_PCT(100));
    lv_obj_set_style_pad_all(scale, 30, 0);

    lv_obj_t *bullet1 = lv_obj_create(parent);
    lv_obj_set_size(bullet1, 13, 13);
    lv_obj_remove_style(bullet1, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet1, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet1, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_t *label1 = lv_label_create(parent);
    lv_label_set_text(label1, text1);

    lv_obj_t *bullet2 = lv_obj_create(parent);
    lv_obj_set_size(bullet2, 13, 13);
    lv_obj_remove_style(bullet2, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet2, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet2, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_t *label2 = lv_label_create(parent);
    lv_label_set_text(label2, text2);

    lv_obj_t *bullet3 = lv_obj_create(parent);
    lv_obj_set_size(bullet3, 13, 13);
    lv_obj_remove_style(bullet3,  NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet3, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet3, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_t *label3 = lv_label_create(parent);
    lv_label_set_text(label3, text3);

    static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_col_dsc, grid_row_dsc);
    lv_obj_set_grid_cell(scale, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 1, 1);
    lv_obj_set_grid_cell(bullet1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
    lv_obj_set_grid_cell(bullet2, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);
    lv_obj_set_grid_cell(bullet3, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);
    lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 2, 1);
    lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 3, 1);
    lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 4, 1);
    return scale;
}

static void scale1_indic1_anim_cb(void *var, int32_t v)
{
    lv_arc_set_value(var, v);

    lv_obj_t *card = lv_obj_get_parent(scale1);
    lv_obj_t *label = lv_obj_get_child(card, -5);
    lv_label_set_text_fmt(label, "Revenue: %"LV_PRId32" %%", v);
}

static void scale1_indic2_anim_cb(void *var, int32_t v)
{
    lv_arc_set_value(var, v);

    lv_obj_t *card = lv_obj_get_parent(scale1);
    lv_obj_t *label = lv_obj_get_child(card, -3);
    lv_label_set_text_fmt(label, "Sales: %"LV_PRId32" %%", v);
}

static void scale1_indic3_anim_cb(void *var, int32_t v)
{
    lv_arc_set_value(var, v);

    lv_obj_t *card = lv_obj_get_parent(scale1);
    lv_obj_t *label = lv_obj_get_child(card, -1);
    lv_label_set_text_fmt(label, "Costs: %"LV_PRId32" %%", v);
}

void example_lvgl_demo_ui(lv_display_t *disp)
{
    // init default theme
    lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                          font_normal);
    // bullet style
    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

    lv_obj_t *parent = lv_display_get_screen_active(disp);

    // create scale widget
    scale1 = create_scale_box(parent, "Revenue", "Sales", "Costs");

    // create arc indicators
    lv_obj_t *arc;
    arc = lv_arc_create(scale1);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_remove_style(arc, NULL, LV_PART_MAIN);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);

    // animation
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_values(&a, 20, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, scale1_indic1_anim_cb);
    lv_anim_set_var(&a, arc);
    lv_anim_set_duration(&a, 4100);
    lv_anim_set_playback_duration(&a, 2700);
    lv_anim_start(&a);

    arc = lv_arc_create(scale1);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_margin_all(arc, 20, 0);
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    lv_anim_set_exec_cb(&a, scale1_indic2_anim_cb);
    lv_anim_set_var(&a, arc);
    lv_anim_set_duration(&a, 2600);
    lv_anim_set_playback_duration(&a, 3200);
    lv_anim_start(&a);

    arc = lv_arc_create(scale1);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_margin_all(arc, 40, 0);
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    lv_anim_set_exec_cb(&a, scale1_indic3_anim_cb);
    lv_anim_set_var(&a, arc);
    lv_anim_set_duration(&a, 2800);
    lv_anim_set_playback_duration(&a, 1800);
    lv_anim_start(&a);
}
