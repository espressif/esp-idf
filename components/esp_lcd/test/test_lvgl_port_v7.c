#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_freertos_hooks.h"
#include "soc/soc_caps.h"
#if CONFIG_LV_USE_USER_DATA
#include "test_lvgl_port.h"

static void my_lvgl_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;

    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;

    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

#if CONFIG_LV_COLOR_DEPTH_1
static void my_lvgl_set_px_cb(lv_disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                              lv_color_t color, lv_opa_t opa)
{
    uint16_t byte_index = x + (( y >> 3 ) * buf_w);
    uint8_t  bit_index  = y & 0x7;

    if ((color.full == 0) && (LV_OPA_TRANSP != opa)) {
        buf[byte_index] |= (1 << bit_index);
    } else {
        buf[byte_index] &= ~(1 << bit_index);
    }
}

static void my_lvgl_rounder(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    area->y1 = (area->y1 & (~0x7));
    area->y2 = (area->y2 & (~0x7)) + 7;
}
#endif

static void increase_lvgl_tick(void)
{
    lv_tick_inc(portTICK_PERIOD_MS);
}

static void create_demo_application(lv_disp_t *disp)
{
    // Get the current screen
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    // Create a Label on the currently active screen
    lv_obj_t *label =  lv_label_create(scr, NULL);
    // Modify the Label's text
    lv_label_set_text(label, "Hello World");
    // Align the Label to the center
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

#if !CONFIG_LV_COLOR_DEPTH_1
    // new screen_spinner
    lv_obj_t *screen_spinner = lv_spinner_create(scr, NULL);
    lv_obj_align(screen_spinner, label, LV_ALIGN_OUT_BOTTOM_MID, 15, 20);
    lv_obj_set_size(screen_spinner, 100, 100);
    lv_spinner_set_arc_length(screen_spinner, 60);
    lv_spinner_set_spin_time(screen_spinner, 1000);
    lv_spinner_set_type(screen_spinner, LV_SPINNER_TYPE_SPINNING_ARC);
    lv_spinner_set_dir(screen_spinner, LV_SPINNER_DIR_FORWARD);

    lv_obj_t *bar = lv_bar_create(scr, NULL);
    lv_obj_set_size(bar, 100, 20);
    lv_obj_align(bar, screen_spinner, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_bar_set_anim_time(bar, 2000);
    lv_bar_set_value(bar, 100, LV_ANIM_ON);
#endif
}

void test_lvgl_task_loop(esp_lcd_panel_handle_t panel_handle, int h_res, int v_res, lv_disp_t **disp)
{
    static lv_disp_buf_t disp_buf;
    // alloc frame buffer used by LVGL
    lv_color_t *buf1 = heap_caps_malloc(h_res * 20 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(buf1);
    lv_color_t *buf2 = heap_caps_malloc(h_res * 20 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(buf2);
    lv_disp_buf_init(&disp_buf, buf1, buf2, h_res * 20);
    // register display driver
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = h_res;
    disp_drv.ver_res = v_res;
    disp_drv.flush_cb = my_lvgl_flush;
#if CONFIG_LV_COLOR_DEPTH_1
    disp_drv.rounder_cb = my_lvgl_rounder;
    disp_drv.set_px_cb = my_lvgl_set_px_cb;
#endif

    disp_drv.buffer = &disp_buf;
    disp_drv.user_data = panel_handle; // LV_USE_USER_DATA is disabled by default, need to enable it in menuconfig
    *disp = lv_disp_drv_register(&disp_drv);

    // Tick interface for LVGL
    esp_register_freertos_tick_hook(increase_lvgl_tick);

    // create a demo UI on that screen
    create_demo_application(*disp);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_task_handler(); // The task running lv_task_handler should have lower priority than that running `lv_tick_inc`
    }
}

#endif // CONFIG_LV_USE_USER_DATA
