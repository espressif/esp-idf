/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_ldo_regulator.h"
#include "driver/ppa.h"
#include "example_dsi_init.h"
#include "example_dsi_init_config.h"
#if CONFIG_EXAMPLE_SOURCE_IMAGE_FORMAT_JPEG
#include "jpeg_decoder.h"
#else
#include "image.h"
#endif

#define EXAMPLE_IMAGE_H 240
#define EXAMPLE_IMAGE_W 320
#define EXAMPLE_BUFFER_CNT_CYCLE(buffer_cnt)            (((buffer_cnt) + 1) % 2)

static const char *TAG = "ppa_dsi";

#if CONFIG_EXAMPLE_SOURCE_IMAGE_FORMAT_JPEG
//Reference the binary-included jpeg file
extern const uint8_t image_jpg_start[] asm("_binary_image_jpg_start");
extern const uint8_t image_jpg_end[] asm("_binary_image_jpg_end");

//Decode the embedded image into pixel lines that can be used with the rest of the logic.
esp_err_t s_get_rgb565_image(uint16_t **pixels)
{
    *pixels = NULL;
    esp_err_t ret = ESP_OK;

    //Allocate pixel memory. Each line is an array of EXAMPLE_IMAGE_W 16-bit pixels; the `*pixels` array itself contains pointers to these lines.
    *pixels = calloc(EXAMPLE_IMAGE_H * EXAMPLE_IMAGE_W, sizeof(uint16_t));
    ESP_GOTO_ON_FALSE((*pixels), ESP_ERR_NO_MEM, err, TAG, "Error allocating memory for lines");

    //JPEG decode config
    esp_jpeg_image_cfg_t jpeg_cfg = {
        .indata = (uint8_t *)image_jpg_start,
        .indata_size = image_jpg_end - image_jpg_start,
        .outbuf = (uint8_t*)(*pixels),
        .outbuf_size = EXAMPLE_IMAGE_W * EXAMPLE_IMAGE_H * sizeof(uint16_t),
        .out_format = JPEG_IMAGE_FORMAT_RGB565,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags = {
            .swap_color_bytes = 0,
        }
    };

    //JPEG decode
    esp_jpeg_image_output_t outimg;
    esp_jpeg_decode(&jpeg_cfg, &outimg);

    ESP_LOGI(TAG, "JPEG image decoded! Size of the decoded image is: %dpx x %dpx", outimg.width, outimg.height);

    return ret;
err:
    //Something went wrong! Exit cleanly, de-allocating everything we allocated.
    if (*pixels != NULL) {
        free(*pixels);
    }
    return ret;
}
#else
esp_err_t s_get_rgb565_image(uint16_t **pixels)
{
    *pixels = (uint16_t *)image_map;
    return ESP_OK;
}
#endif

void s_srm_ops(void *in_buf, void **out_buf, size_t buf_size, esp_lcd_panel_handle_t mipi_dpi_panel, ppa_client_handle_t ppa_srm_handle, void *clear_buf)
{
    ppa_srm_oper_config_t srm_config = {
        .in.buffer = in_buf,
        .in.pic_w = EXAMPLE_IMAGE_W,
        .in.pic_h = EXAMPLE_IMAGE_H,
        .in.block_w = EXAMPLE_IMAGE_W,
        .in.block_h = EXAMPLE_IMAGE_H,
        .in.block_offset_x = 0,
        .in.block_offset_y = 0,
        .in.srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        .out.buffer = out_buf[0],
        .out.buffer_size = buf_size,
        .out.pic_w = EXAMPLE_IMAGE_W,
        .out.pic_h = EXAMPLE_IMAGE_H,
        .out.block_offset_x = 0,
        .out.block_offset_y = 0,
        .out.srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        .rotation_angle = PPA_SRM_ROTATION_ANGLE_0,
        .scale_x = 1,
        .scale_y = 1,
        .rgb_swap = 0,
        .byte_swap = 0,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    //scale up from x0.5, x1, to x1.5
    const float size_mul[3] = {0.5, 1, 1.5};
    const float mul[3] = {0.5, 2, 1.5};
    int buffer_cnt = 0;
    for (int i = 0; i < 3; i++) {
        srm_config.out.pic_w = EXAMPLE_IMAGE_W * size_mul[i];
        srm_config.out.pic_h = EXAMPLE_IMAGE_H * size_mul[i];
        srm_config.scale_x = mul[i];
        srm_config.scale_y = mul[i];
        ESP_ERROR_CHECK(ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config));
        ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W * size_mul[i], EXAMPLE_IMAGE_H * size_mul[i], out_buf[buffer_cnt]));
        vTaskDelay(800 / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W * mul[i], EXAMPLE_IMAGE_H * mul[i], clear_buf));
        srm_config.in.buffer = out_buf[i % 2];
        srm_config.out.buffer = out_buf[1 - i % 2];
        srm_config.in.pic_w = EXAMPLE_IMAGE_W * size_mul[i];
        srm_config.in.pic_h = EXAMPLE_IMAGE_H * size_mul[i];
        srm_config.in.block_w = EXAMPLE_IMAGE_W * size_mul[i];
        srm_config.in.block_h = EXAMPLE_IMAGE_H * size_mul[i];
        buffer_cnt = EXAMPLE_BUFFER_CNT_CYCLE(buffer_cnt);
    }

    //rotate the x1.5 image in CCW direction
    srm_config.rotation_angle = PPA_SRM_ROTATION_ANGLE_90;
    srm_config.scale_x = 1;
    srm_config.scale_y = 1;
    srm_config.out.pic_w = EXAMPLE_IMAGE_H * 1.5;
    srm_config.out.pic_h = EXAMPLE_IMAGE_W * 1.5;
    ESP_ERROR_CHECK(ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_H * 1.5, EXAMPLE_IMAGE_W * 1.5, out_buf[buffer_cnt]));
    vTaskDelay(800 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_H * 1.5, EXAMPLE_IMAGE_W * 1.5, clear_buf));
    srm_config.in.buffer = out_buf[buffer_cnt];
    srm_config.in.pic_w = EXAMPLE_IMAGE_H * 1.5;
    srm_config.in.pic_h = EXAMPLE_IMAGE_W * 1.5;
    srm_config.in.block_w = EXAMPLE_IMAGE_H * 1.5;
    srm_config.in.block_h = EXAMPLE_IMAGE_W * 1.5;
    srm_config.out.buffer = out_buf[1 - buffer_cnt];
    buffer_cnt = EXAMPLE_BUFFER_CNT_CYCLE(buffer_cnt);

    //rotate the x1.5 image back in CW direction
    srm_config.rotation_angle = PPA_SRM_ROTATION_ANGLE_270;
    srm_config.out.pic_w = EXAMPLE_IMAGE_W * 1.5;
    srm_config.out.pic_h = EXAMPLE_IMAGE_H * 1.5;
    ESP_ERROR_CHECK(ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W * 1.5, EXAMPLE_IMAGE_H * 1.5, out_buf[buffer_cnt]));
    vTaskDelay(800 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W * 1.5, EXAMPLE_IMAGE_H * 1.5, clear_buf));
    srm_config.rotation_angle = PPA_SRM_ROTATION_ANGLE_0;
    srm_config.in.buffer = out_buf[buffer_cnt];
    srm_config.in.pic_w = EXAMPLE_IMAGE_W * 1.5;
    srm_config.in.pic_h = EXAMPLE_IMAGE_H * 1.5;
    srm_config.in.block_w = EXAMPLE_IMAGE_W * 1.5;
    srm_config.in.block_h = EXAMPLE_IMAGE_H * 1.5;
    srm_config.out.buffer = out_buf[1 - buffer_cnt];
    buffer_cnt = EXAMPLE_BUFFER_CNT_CYCLE(buffer_cnt);

    //mirror the x1.5 image along both x and y axis, then mirror it back
    for (int i = 0; i < 2; i++) {
        srm_config.mirror_x = 1;
        srm_config.mirror_y = 1;
        ESP_ERROR_CHECK(ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config));
        ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W * 1.5, EXAMPLE_IMAGE_H * 1.5, out_buf[buffer_cnt]));
        vTaskDelay(800 / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W * 1.5, EXAMPLE_IMAGE_H * 1.5, clear_buf));
        srm_config.in.buffer = out_buf[buffer_cnt];
        srm_config.out.buffer = out_buf[1 - buffer_cnt];
        buffer_cnt = EXAMPLE_BUFFER_CNT_CYCLE(buffer_cnt);
    }

    //back to 1x original image
    srm_config.in.buffer = in_buf;
    srm_config.in.pic_w = EXAMPLE_IMAGE_W;
    srm_config.in.pic_h = EXAMPLE_IMAGE_H;
    srm_config.in.block_w = EXAMPLE_IMAGE_W;
    srm_config.in.block_h = EXAMPLE_IMAGE_H;
    srm_config.out.buffer = out_buf[buffer_cnt];
    srm_config.rotation_angle = PPA_SRM_ROTATION_ANGLE_0;
    srm_config.out.pic_w = EXAMPLE_IMAGE_W;
    srm_config.out.pic_h = EXAMPLE_IMAGE_H;
    srm_config.scale_x = 1;
    srm_config.scale_y = 1;
    srm_config.mirror_x = 0;
    srm_config.mirror_y = 0;
    ESP_ERROR_CHECK(ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W, EXAMPLE_IMAGE_H, out_buf[buffer_cnt]));
    vTaskDelay(800 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W, EXAMPLE_IMAGE_H, clear_buf));
}

void s_blend_ops(void *bg_buf, void *fg_buf, void *out_buf, size_t buf_size, esp_lcd_panel_handle_t mipi_dpi_panel, ppa_client_handle_t ppa_blend_handle, void *clear_buf)
{
    //this operation will blend the bg_buf with the fg_buf
    ppa_blend_oper_config_t blend_config = {
        .in_bg.buffer = bg_buf,
        .in_bg.pic_w = EXAMPLE_IMAGE_W,
        .in_bg.pic_h = EXAMPLE_IMAGE_H,
        .in_bg.block_w = EXAMPLE_IMAGE_W,
        .in_bg.block_h = EXAMPLE_IMAGE_H,
        .in_bg.block_offset_x = 0,
        .in_bg.block_offset_y = 0,
        .in_bg.blend_cm = PPA_SRM_COLOR_MODE_RGB565,
        .in_fg.buffer = fg_buf,
        .in_fg.pic_w = EXAMPLE_IMAGE_W,
        .in_fg.pic_h = EXAMPLE_IMAGE_H,
        .in_fg.block_w = EXAMPLE_IMAGE_W,
        .in_fg.block_h = EXAMPLE_IMAGE_H,
        .in_fg.block_offset_x = 0,
        .in_fg.block_offset_y = 0,
        .in_fg.blend_cm = PPA_BLEND_COLOR_MODE_A8,
        .out.buffer = out_buf,
        .out.buffer_size = buf_size,
        .out.pic_w = EXAMPLE_IMAGE_W,
        .out.pic_h = EXAMPLE_IMAGE_H,
        .out.block_offset_x = 0,
        .out.block_offset_y = 0,
        .out.blend_cm = PPA_SRM_COLOR_MODE_RGB565,
        .bg_alpha_update_mode = PPA_ALPHA_SCALE,
        .bg_alpha_scale_ratio = 0.9,
        .fg_alpha_update_mode = PPA_ALPHA_SCALE,
        .fg_alpha_scale_ratio = 0.5,
        .fg_fix_rgb_val = {
            .b = 0xd3,
            .g = 0x03,
            .r = 0xff,
        },
        .bg_ck_en = false,
        .fg_ck_en = false,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };
    ESP_ERROR_CHECK(ppa_do_blend(ppa_blend_handle, &blend_config));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W, EXAMPLE_IMAGE_H, out_buf));
    vTaskDelay(800 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W, EXAMPLE_IMAGE_H, clear_buf));

    //the `ESP32` in the bg_buf will be color-keyed out
    memset(fg_buf, 0xff, buf_size);
    blend_config.fg_alpha_update_mode = PPA_ALPHA_NO_CHANGE;
    blend_config.in_bg.blend_cm = PPA_SRM_COLOR_MODE_RGB565;
    blend_config.fg_fix_rgb_val = (color_pixel_rgb888_data_t) {};
    blend_config.bg_ck_en = true;
    blend_config.bg_ck_rgb_low_thres = (color_pixel_rgb888_data_t) {
        .b = 0,
        .g = 0,
        .r = 0x80,
    };
    blend_config.bg_ck_rgb_high_thres = (color_pixel_rgb888_data_t) {
        .b = 0x40,
        .g = 0x40,
        .r = 0xfe,
    };
    ESP_ERROR_CHECK(ppa_do_blend(ppa_blend_handle, &blend_config));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W, EXAMPLE_IMAGE_H, out_buf));
    vTaskDelay(800 / portTICK_PERIOD_MS);
}

void s_fill_ops(void *out_buf, size_t buf_size, esp_lcd_panel_handle_t mipi_dpi_panel, ppa_client_handle_t ppa_fill_handle)
{
    int buffer_cnt = 0;
    ppa_fill_oper_config_t fill_config = {
        .out.buffer = out_buf,
        .out.buffer_size = buf_size,
        .out.pic_w = EXAMPLE_IMAGE_W,
        .out.pic_h = EXAMPLE_IMAGE_H,
        .out.block_offset_x = 0,
        .out.block_offset_y = 20,
        .out.fill_cm = PPA_FILL_COLOR_MODE_RGB565,
        .fill_block_w = 10,
        .fill_block_h = 200,
        .fill_argb_color = {
            .a = 0x80,
            .r = 0xe0,
            .g = 0x10,
            .b = 0x10,
        },
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    //below ops will add a frame to the `ESP32` characters
    ESP_ERROR_CHECK(ppa_do_fill(ppa_fill_handle, &fill_config));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W, EXAMPLE_IMAGE_H, out_buf));
    buffer_cnt = EXAMPLE_BUFFER_CNT_CYCLE(buffer_cnt);

    fill_config.out.block_offset_x = 0;
    fill_config.out.block_offset_y = 20;
    fill_config.fill_block_w = 320;
    fill_config.fill_block_h = 10;
    ESP_ERROR_CHECK(ppa_do_fill(ppa_fill_handle, &fill_config));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W, EXAMPLE_IMAGE_H, out_buf));
    buffer_cnt = EXAMPLE_BUFFER_CNT_CYCLE(buffer_cnt);

    fill_config.out.block_offset_x = 310;
    fill_config.out.block_offset_y = 20;
    fill_config.fill_block_w = 10;
    fill_config.fill_block_h = 200;
    ESP_ERROR_CHECK(ppa_do_fill(ppa_fill_handle, &fill_config));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W, EXAMPLE_IMAGE_H, out_buf));
    buffer_cnt = EXAMPLE_BUFFER_CNT_CYCLE(buffer_cnt);

    fill_config.out.block_offset_x = 0;
    fill_config.out.block_offset_y = 220;
    fill_config.fill_block_w = 320;
    fill_config.fill_block_h = 10;
    ESP_ERROR_CHECK(ppa_do_fill(ppa_fill_handle, &fill_config));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, EXAMPLE_IMAGE_W, EXAMPLE_IMAGE_H, out_buf));
    buffer_cnt = EXAMPLE_BUFFER_CNT_CYCLE(buffer_cnt);
    vTaskDelay(800 / portTICK_PERIOD_MS);
}

void app_main(void)
{
    uint16_t *pixels = NULL;
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
    esp_lcd_panel_io_handle_t mipi_dbi_io = NULL;
    esp_lcd_panel_handle_t mipi_dpi_panel = NULL;

    //---------------MIPI LDO Init------------------//
    esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
    esp_ldo_channel_config_t ldo_mipi_phy_config = {
        .chan_id = CONFIG_EXAMPLE_USED_LDO_CHAN_ID,
        .voltage_mv = CONFIG_EXAMPLE_USED_LDO_VOLTAGE_MV,
    };
    ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_mipi_phy_config, &ldo_mipi_phy));

    //---------------DSI Init------------------//
    example_dsi_resource_alloc(&mipi_dsi_bus, &mipi_dbi_io, &mipi_dpi_panel, NULL);
    example_dpi_panel_reset(mipi_dpi_panel);
    example_dpi_panel_init(mipi_dpi_panel);

    //---------------Get Source image------------------//
    ESP_ERROR_CHECK(s_get_rgb565_image(&pixels));
    size_t out_buf_size = EXAMPLE_IMAGE_H * EXAMPLE_IMAGE_W * 4 * 2;
    uint8_t *out_buf[2] = {};
    for (int i = 0; i < 2; i++) {
        out_buf[i] = heap_caps_calloc(out_buf_size, 1, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
        if (!out_buf[i]) {
            ESP_LOGE(TAG, "no mem for out_buf");
            return ;
        }
    }

    uint8_t *clear_buf = heap_caps_calloc(out_buf_size, 1, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
    if (!clear_buf) {
        ESP_LOGE(TAG, "no mem for clear_buf");
        return ;
    }

    //---------------SRM------------------//
    ppa_client_handle_t ppa_srm_handle = NULL;
    ppa_client_config_t ppa_srm_config = {
        .oper_type = PPA_OPERATION_SRM,
        .max_pending_trans_num = 1,
    };
    ESP_ERROR_CHECK(ppa_register_client(&ppa_srm_config, &ppa_srm_handle));
    printf("start srm operations\n");
    s_srm_ops(pixels, (void **)out_buf, out_buf_size, mipi_dpi_panel, ppa_srm_handle, clear_buf);

    uint8_t *fg_buf = heap_caps_calloc(out_buf_size, 1, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
    if (!fg_buf) {
        ESP_LOGE(TAG, "no mem for fg_buf");
        return ;
    }
    memset(fg_buf, 0xff, out_buf_size);

    //---------------BLEND------------------//
    ppa_client_handle_t ppa_blend_handle = NULL;
    ppa_client_config_t ppa_blend_config = {
        .oper_type = PPA_OPERATION_BLEND,
        .max_pending_trans_num = 1,
    };
    ESP_ERROR_CHECK(ppa_register_client(&ppa_blend_config, &ppa_blend_handle));
    printf("start blend operations\n");
    s_blend_ops(pixels, fg_buf, out_buf[0], out_buf_size, mipi_dpi_panel, ppa_blend_handle, clear_buf);

    //---------------FILL------------------//
    ppa_client_handle_t ppa_fill_handle = NULL;
    ppa_client_config_t ppa_fill_config = {
        .oper_type = PPA_OPERATION_FILL,
        .max_pending_trans_num = 1,
    };
    ESP_ERROR_CHECK(ppa_register_client(&ppa_fill_config, &ppa_fill_handle));
    printf("start fill operations\n");
    s_fill_ops(out_buf[0], out_buf_size, mipi_dpi_panel, ppa_fill_handle);

#if CONFIG_EXAMPLE_SOURCE_IMAGE_FORMAT_JPEG
    free(pixels);
#endif
    free(out_buf[0]);
    free(out_buf[1]);
    free(clear_buf);
    free(fg_buf);
    ESP_ERROR_CHECK(ppa_unregister_client(ppa_srm_handle));
    ESP_ERROR_CHECK(ppa_unregister_client(ppa_blend_handle));
    ESP_ERROR_CHECK(ppa_unregister_client(ppa_fill_handle));
    example_dsi_resource_destroy(mipi_dsi_bus, mipi_dbi_io, mipi_dpi_panel);
}
