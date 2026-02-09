/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_random.h"
#include "esp_attr.h"
#include "test_mipi_dsi_board.h"
#include "esp_lcd_ek79007.h"
#include "driver/ppa.h"

TEST_CASE("MIPI DSI Pattern Generator (EK79007)", "[mipi_dsi]")
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    esp_lcd_panel_handle_t mipi_dpi_panel;

    test_bsp_enable_dsi_phy_power();

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    TEST_ESP_OK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,
        .in_color_format = LCD_COLOR_FMT_RGB888,
        .video_timing = {
            .h_size = MIPI_DSI_LCD_H_RES,
            .v_size = MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = MIPI_DSI_LCD_HFP,
            .vsync_back_porch = MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = MIPI_DSI_LCD_VFP,
        },
    };
    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 24,
        .vendor_config = &vendor_config,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ek79007(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_reset(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_init(mipi_dpi_panel));

    TEST_ESP_OK(esp_lcd_dpi_panel_set_pattern(mipi_dpi_panel, MIPI_DSI_PATTERN_BAR_HORIZONTAL));
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ESP_OK(esp_lcd_dpi_panel_set_pattern(mipi_dpi_panel, MIPI_DSI_PATTERN_BAR_VERTICAL));
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ESP_OK(esp_lcd_dpi_panel_set_pattern(mipi_dpi_panel, MIPI_DSI_PATTERN_BER_VERTICAL));
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ESP_OK(esp_lcd_dpi_panel_set_pattern(mipi_dpi_panel, MIPI_DSI_PATTERN_NONE));

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));

    test_bsp_disable_dsi_phy_power();
}

#define TEST_IMG_SIZE (200 * 200 * sizeof(uint16_t))

TEST_CASE("MIPI DSI draw RGB bitmap (EK79007)", "[mipi_dsi]")
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    esp_lcd_panel_handle_t mipi_dpi_panel;

    test_bsp_enable_dsi_phy_power();

    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    TEST_ESP_OK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .video_timing = {
            .h_size = MIPI_DSI_LCD_H_RES,
            .v_size = MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = MIPI_DSI_LCD_HFP,
            .vsync_back_porch = MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = MIPI_DSI_LCD_VFP,
        },
    };
    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ek79007(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_reset(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_init(mipi_dpi_panel));

    for (int i = 0; i < 100; i++) {
        uint8_t color_byte = rand() & 0xFF;
        int x_start = rand() % (MIPI_DSI_LCD_H_RES - 100);
        int y_start = rand() % (MIPI_DSI_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(mipi_dpi_panel, x_start, y_start, x_start + 100, y_start + 100, img);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));
    free(img);

    test_bsp_disable_dsi_phy_power();
}

#if SOC_HAS(DMA2D)
TEST_CASE("MIPI DSI use DMA2D (EK79007)", "[mipi_dsi]")
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    esp_lcd_panel_handle_t mipi_dpi_panel;

    test_bsp_enable_dsi_phy_power();

    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    TEST_ESP_OK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .video_timing = {
            .h_size = MIPI_DSI_LCD_H_RES,
            .v_size = MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = MIPI_DSI_LCD_HFP,
            .vsync_back_porch = MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = MIPI_DSI_LCD_VFP,
        },
    };
    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ek79007(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_reset(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_init(mipi_dpi_panel));

    printf("Draw bitmap 2D by CPU\r\n");
    for (int i = 0; i < 100; i++) {
        int x_start = rand() % (MIPI_DSI_LCD_H_RES - 100);
        int y_start = rand() % (MIPI_DSI_LCD_V_RES - 100);
        uint8_t color_byte = rand() & 0xFF;
        memset(img, color_byte, TEST_IMG_SIZE / 2);
        color_byte = rand() & 0xFF;
        memset(img + TEST_IMG_SIZE / 2, color_byte, TEST_IMG_SIZE / 2);
        esp_lcd_panel_draw_bitmap_2d(mipi_dpi_panel, x_start, y_start, x_start + 100, y_start + 100, img, 200, 200, 50, 50, 150, 150);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("Add Built-in DMA2D draw bitmap hook\r\n");
    TEST_ESP_OK(esp_lcd_dpi_panel_enable_dma2d(mipi_dpi_panel));
    for (int i = 0; i < 100; i++) {
        int x_start = rand() % (MIPI_DSI_LCD_H_RES - 100);
        int y_start = rand() % (MIPI_DSI_LCD_V_RES - 100);
        uint8_t color_byte = rand() & 0xFF;
        memset(img, color_byte, TEST_IMG_SIZE / 2);
        color_byte = rand() & 0xFF;
        memset(img + TEST_IMG_SIZE / 2, color_byte, TEST_IMG_SIZE / 2);
        esp_lcd_panel_draw_bitmap_2d(mipi_dpi_panel, x_start, y_start, x_start + 100, y_start + 100, img, 200, 200, 50, 50, 150, 150);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    TEST_ESP_OK(esp_lcd_dpi_panel_disable_dma2d(mipi_dpi_panel));
    vTaskDelay(pdMS_TO_TICKS(1000));

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));
    free(img);

    test_bsp_disable_dsi_phy_power();
}
#endif // SOC_HAS(DMA2D)

#if SOC_HAS(PPA)
typedef struct {
    ppa_client_handle_t ppa_srm_handle;
    esp_lcd_draw_bitmap_hook_data_t hook_data;
    SemaphoreHandle_t draw_sem;
    esp_lcd_panel_handle_t panel;
} test_dpi_panel_draw_bitmap_hook_ctx_t;

typedef struct {
    uint32_t count;
    SemaphoreHandle_t draw_sem;
} test_dpi_panel_color_trans_done_callback_ctx_t;

IRAM_ATTR static bool test_ppa_srm_trans_done_callback(ppa_client_handle_t ppa_client, ppa_event_data_t *edata, void *user_ctx)
{
    bool need_yield = false;
    test_dpi_panel_draw_bitmap_hook_ctx_t *hook_ctx = (test_dpi_panel_draw_bitmap_hook_ctx_t *)user_ctx;
    esp_lcd_draw_bitmap_hook_data_t *hook_data = &hook_ctx->hook_data;

    if (hook_data->on_hook_end) {
        if (hook_data->on_hook_end(hook_ctx->panel)) {
            need_yield = true;
        }
    }

    return need_yield;
}

static esp_err_t test_draw_bitmap_hook_ppa(esp_lcd_panel_handle_t panel, const esp_lcd_draw_bitmap_hook_data_t *hook_data, void *user_ctx)
{
    test_dpi_panel_draw_bitmap_hook_ctx_t *hook_ctx = (test_dpi_panel_draw_bitmap_hook_ctx_t *)user_ctx;
    ppa_client_handle_t ppa_srm_handle = hook_ctx->ppa_srm_handle;
    memcpy(&hook_ctx->hook_data, hook_data, sizeof(esp_lcd_draw_bitmap_hook_data_t));
    xSemaphoreTake(hook_ctx->draw_sem, portMAX_DELAY);
    ppa_srm_oper_config_t srm_config = {
        .in.buffer = hook_data->src_data,
        .in.pic_w = hook_data->src_x_size,
        .in.pic_h = hook_data->src_y_size,
        .in.block_w = hook_data->src_x_end - hook_data->src_x_start,
        .in.block_h = hook_data->src_y_end - hook_data->src_y_start,
        .in.block_offset_x = hook_data->src_x_start,
        .in.block_offset_y = hook_data->src_y_start,
        .in.srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        .out.buffer = hook_data->dst_data,
        .out.buffer_size = hook_data->dst_x_size * hook_data->dst_y_size * hook_data->bits_per_pixel / 8,
        .out.pic_w = hook_data->dst_x_size,
        .out.pic_h = hook_data->dst_y_size,
        .out.block_offset_x = hook_data->dst_x_start,
        .out.block_offset_y = hook_data->dst_y_start,
        .out.srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        .rotation_angle = PPA_SRM_ROTATION_ANGLE_90,
        .scale_x = 0.5,
        .scale_y = 0.5,
        .rgb_swap = 0,
        .byte_swap = 0,
        .mode = PPA_TRANS_MODE_NON_BLOCKING,
        .user_data = hook_ctx,
    };

    ppa_event_callbacks_t ppa_srm_event_callbacks = {
        .on_trans_done = test_ppa_srm_trans_done_callback,
    };
    TEST_ESP_OK(ppa_client_register_event_callbacks(ppa_srm_handle, &ppa_srm_event_callbacks));

    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config));

    return ESP_OK;
}

IRAM_ATTR static bool test_dpi_panel_color_trans_done_count_callback(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx)
{
    BaseType_t task_woken = pdFALSE;
    test_dpi_panel_color_trans_done_callback_ctx_t *color_trans_done_ctx = (test_dpi_panel_color_trans_done_callback_ctx_t *)user_ctx;
    color_trans_done_ctx->count++;
    xSemaphoreGiveFromISR(color_trans_done_ctx->draw_sem, &task_woken);
    return task_woken == pdTRUE;
}

TEST_CASE("MIPI DSI use PPA (EK79007)", "[mipi_dsi]")
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    esp_lcd_panel_handle_t mipi_dpi_panel;

    test_bsp_enable_dsi_phy_power();

    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    TEST_ESP_OK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .video_timing = {
            .h_size = MIPI_DSI_LCD_H_RES,
            .v_size = MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = MIPI_DSI_LCD_HFP,
            .vsync_back_porch = MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = MIPI_DSI_LCD_VFP,
        },
    };
    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ek79007(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_reset(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_init(mipi_dpi_panel));

    SemaphoreHandle_t draw_sem = xSemaphoreCreateBinaryWithCaps(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(draw_sem);
    xSemaphoreGive(draw_sem);
    // use PPA to scale and rotate the image in draw bitmap hook
    ppa_client_handle_t ppa_srm_handle = NULL;
    ppa_client_config_t ppa_srm_config = {
        .oper_type = PPA_OPERATION_SRM,
        .max_pending_trans_num = 1,
    };
    TEST_ESP_OK(ppa_register_client(&ppa_srm_config, &ppa_srm_handle));

    esp_lcd_dpi_panel_event_callbacks_t cbs = {
        .on_color_trans_done = test_dpi_panel_color_trans_done_count_callback,
    };

    test_dpi_panel_color_trans_done_callback_ctx_t color_trans_done_ctx = {
        .draw_sem = draw_sem,
        .count = 0,
    };
    TEST_ESP_OK(esp_lcd_dpi_panel_register_event_callbacks(mipi_dpi_panel, &cbs, &color_trans_done_ctx));

    printf("Add PPA draw bitmap hook\r\n");
    esp_lcd_panel_hooks_t hooks = {
        .draw_bitmap_hook = test_draw_bitmap_hook_ppa,
    };
    test_dpi_panel_draw_bitmap_hook_ctx_t hook_ctx = {
        .draw_sem = draw_sem,
        .ppa_srm_handle = ppa_srm_handle,
        .panel = mipi_dpi_panel,
    };
    TEST_ESP_OK(esp_lcd_dpi_panel_register_hooks(mipi_dpi_panel, &hooks, &hook_ctx));
    for (int i = 0; i < 100; i++) {
        int x_start = rand() % (MIPI_DSI_LCD_H_RES - 100);
        int y_start = rand() % (MIPI_DSI_LCD_V_RES - 100);
        uint8_t color_byte = rand() & 0xFF;
        memset(img, color_byte, TEST_IMG_SIZE / 2);
        color_byte = rand() & 0xFF;
        memset(img + TEST_IMG_SIZE / 2, color_byte, TEST_IMG_SIZE / 2);
        esp_lcd_panel_draw_bitmap_2d(mipi_dpi_panel, x_start, y_start, x_start + 50, y_start + 50,
                                     img, 200, 200, 0, 0, 200, 200);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    TEST_ASSERT_EQUAL_INT(100, color_trans_done_ctx.count);

    hooks.draw_bitmap_hook = NULL;
    TEST_ESP_OK(esp_lcd_dpi_panel_register_hooks(mipi_dpi_panel, &hooks, NULL));
    TEST_ESP_OK(ppa_unregister_client(ppa_srm_handle));

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));
    vSemaphoreDelete(draw_sem);
    free(img);

    test_bsp_disable_dsi_phy_power();
}
#endif // SOC_HAS(PPA)

TEST_CASE("MIPI DSI use multiple frame buffers (EK79007)", "[mipi_dsi]")
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    esp_lcd_panel_handle_t mipi_dpi_panel;

    test_bsp_enable_dsi_phy_power();

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    TEST_ESP_OK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .num_fbs = 3, // the driver will allocate and maintain 3 frame buffers
        .video_timing = {
            .h_size = MIPI_DSI_LCD_H_RES,
            .v_size = MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = MIPI_DSI_LCD_HFP,
            .vsync_back_porch = MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = MIPI_DSI_LCD_VFP,
        },
    };

    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ek79007(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_reset(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_init(mipi_dpi_panel));

    uint16_t *fbs[3];
    TEST_ESP_OK(esp_lcd_dpi_panel_get_frame_buffer(mipi_dpi_panel, 3, (void **)&fbs[0], (void **)&fbs[1], (void **)&fbs[2]));

    for (int i = 0; i < 9; i++) {
        uint16_t color_byte = rand() & 0xFFFF;
        int x_start = rand() % (MIPI_DSI_LCD_H_RES - 100);
        int y_start = rand() % (MIPI_DSI_LCD_V_RES - 100);
        for (int j = y_start; j < y_start + 100; j++) {
            for (int k = x_start; k < x_start + 100; k++) {
                fbs[i % 3][j * MIPI_DSI_LCD_H_RES + k] = color_byte;
            }
        }
        esp_lcd_panel_draw_bitmap(mipi_dpi_panel, x_start, y_start, x_start + 100, y_start + 100, fbs[i % 3]);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));

    test_bsp_disable_dsi_phy_power();
}

TEST_CASE("MIPI DSI draw YUV422 image (EK79007)", "[mipi_dsi]")
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    esp_lcd_panel_handle_t mipi_dpi_panel;

    test_bsp_enable_dsi_phy_power();

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    TEST_ESP_OK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,

        // YUV422 -> RGB888
        .in_color_format = LCD_COLOR_FMT_YUV422_YUYV,
        .out_color_format = LCD_COLOR_FMT_RGB888,

        .video_timing = {
            .h_size = MIPI_DSI_LCD_H_RES,
            .v_size = MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = MIPI_DSI_LCD_HFP,
            .vsync_back_porch = MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = MIPI_DSI_LCD_VFP,
        },
    };
    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 24,
        .vendor_config = &vendor_config,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ek79007(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));

    // Set color conversion configuration
    esp_lcd_color_conv_yuv_config_t convert_config = {
        .in_color_range = LCD_COLOR_RANGE_FULL,
        .out_color_range = LCD_COLOR_RANGE_FULL,
        .conv_std = LCD_YUV_CONV_STD_BT601,
    };
    TEST_ESP_OK(esp_lcd_dpi_panel_set_yuv_conversion(mipi_dpi_panel, &convert_config));

    TEST_ESP_OK(esp_lcd_panel_reset(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_init(mipi_dpi_panel));

    // YUV images are embedded in the firmware binary
    extern const uint8_t image_hello_yuv_start[] asm("_binary_hello_yuv_start");
    extern const uint8_t image_world_yuv_start[] asm("_binary_world_yuv_start");

    printf("Draw YUV images\r\n");
    for (int i = 0; i < 4; i++) {
        TEST_ESP_OK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, 320, 320, image_hello_yuv_start));
        vTaskDelay(pdMS_TO_TICKS(1000));
        TEST_ESP_OK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, 320, 320, image_world_yuv_start));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));

    test_bsp_disable_dsi_phy_power();
}

#if !CONFIG_ESP32P4_SELECTS_REV_LESS_V3

TEST_CASE("MIPI DSI draw Gray8 image (EK79007)", "[mipi_dsi]")
{
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    esp_lcd_panel_handle_t mipi_dpi_panel;

    test_bsp_enable_dsi_phy_power();

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .lane_bit_rate_mbps = 1000, // 1000 Mbps
    };
    TEST_ESP_OK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = MIPI_DSI_DPI_CLK_MHZ,
        .virtual_channel = 0,

        // Gray8 -> RGB888
        .in_color_format = LCD_COLOR_FMT_GRAY8,
        .out_color_format = LCD_COLOR_FMT_RGB888,

        .video_timing = {
            .h_size = MIPI_DSI_LCD_H_RES,
            .v_size = MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = MIPI_DSI_LCD_HFP,
            .vsync_back_porch = MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = MIPI_DSI_LCD_VFP,
        },
    };
    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 24,
        .vendor_config = &vendor_config,
    };
    TEST_ESP_OK(esp_lcd_new_panel_ek79007(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));

    TEST_ESP_OK(esp_lcd_panel_reset(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_init(mipi_dpi_panel));

    // YUV images are embedded in the firmware binary
    extern const uint8_t image_hello_gray_start[] asm("_binary_hello_gray_start");
    extern const uint8_t image_world_gray_start[] asm("_binary_world_gray_start");

    printf("Draw Gray8 images\r\n");
    for (int i = 0; i < 4; i++) {
        TEST_ESP_OK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, 320, 320, image_hello_gray_start));
        vTaskDelay(pdMS_TO_TICKS(1000));
        TEST_ESP_OK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, 320, 320, image_world_gray_start));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    TEST_ESP_OK(esp_lcd_panel_del(mipi_dpi_panel));
    TEST_ESP_OK(esp_lcd_panel_io_del(mipi_dbi_io));
    TEST_ESP_OK(esp_lcd_del_dsi_bus(mipi_dsi_bus));

    test_bsp_disable_dsi_phy_power();
}

#endif
