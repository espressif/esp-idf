/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ppa.h"
#include "esp_heap_caps.h"
#include "esp_err.h"
#include "ccomp_timer.h"
#include "hal/color_hal.h"
#include "esp_cache.h"

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))

TEST_CASE("ppa_client_do_ppa_operation", "[PPA]")
{
    const uint32_t w = 480;
    const uint32_t h = 480;
    const uint32_t buf_1_color_type_id = COLOR_TYPE_ID(COLOR_SPACE_ARGB, COLOR_PIXEL_ARGB8888);
    const uint32_t buf_2_color_type_id = COLOR_TYPE_ID(COLOR_SPACE_ARGB, COLOR_PIXEL_ARGB8888);

    color_space_pixel_format_t buf_1_cm = {
        .color_type_id = buf_1_color_type_id,
    };
    color_space_pixel_format_t buf_2_cm = {
        .color_type_id = buf_2_color_type_id,
    };

    uint32_t buf_1_size = ALIGN_UP(w * h * color_hal_pixel_format_get_bit_depth(buf_1_cm) / 8, 64);
    uint32_t buf_2_size = ALIGN_UP(w * h * color_hal_pixel_format_get_bit_depth(buf_2_cm) / 8, 64);
    uint8_t *buf_1 = heap_caps_aligned_calloc(4, buf_1_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA); // cache alignment is implicited by MALLOC_CAP_DMA
    TEST_ASSERT_NOT_NULL(buf_1);
    uint8_t *buf_2 = heap_caps_aligned_calloc(4, buf_2_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(buf_2);

    // Register different types of PPA clients
    ppa_client_handle_t ppa_client_srm_handle;
    ppa_client_handle_t ppa_client_blend_handle;
    ppa_client_handle_t ppa_client_fill_handle_a;
    ppa_client_handle_t ppa_client_fill_handle_b;
    ppa_client_config_t ppa_client_config = {
        .oper_type = PPA_OPERATION_SRM,
    };
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_srm_handle));
    ppa_client_config.oper_type = PPA_OPERATION_BLEND;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_blend_handle));
    ppa_client_config.oper_type = PPA_OPERATION_FILL;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_fill_handle_a));
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_fill_handle_b));

    ppa_srm_oper_config_t srm_oper_config = {
        .in.buffer = buf_1,
        .in.pic_w = w,
        .in.pic_h = h,
        .in.block_w = w,
        .in.block_h = h,
        .in.block_offset_x = 0,
        .in.block_offset_y = 0,
        .in.srm_cm = buf_1_color_type_id,

        .out.buffer = buf_2,
        .out.buffer_size = buf_2_size,
        .out.pic_w = w,
        .out.pic_h = h,
        .out.block_offset_x = 0,
        .out.block_offset_y = 0,
        .out.srm_cm = buf_2_color_type_id,

        .rotation_angle = PPA_SRM_ROTATION_ANGLE_0,
        .scale_x = 1.0,
        .scale_y = 1.0,

        .mode = PPA_TRANS_MODE_BLOCKING,
    };
    // A SRM client can request to do a SRM operation
    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_srm_handle, &srm_oper_config));
    // A non-SRM client can not request to do a SRM operation
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, ppa_do_scale_rotate_mirror(ppa_client_blend_handle, &srm_oper_config));

    ppa_blend_oper_config_t blend_oper_config = {
        .in_bg.buffer = buf_1,
        .in_bg.pic_w = w,
        .in_bg.pic_h = h,
        .in_bg.block_w = w,
        .in_bg.block_h = h,
        .in_bg.block_offset_x = 0,
        .in_bg.block_offset_y = 0,
        .in_bg.blend_cm = buf_1_color_type_id,

        .in_fg.buffer = buf_2,
        .in_fg.pic_w = w,
        .in_fg.pic_h = h,
        .in_fg.block_w = w,
        .in_fg.block_h = h,
        .in_fg.block_offset_x = 0,
        .in_fg.block_offset_y = 0,
        .in_fg.blend_cm = buf_2_color_type_id,

        .out.buffer = buf_1,
        .out.buffer_size = buf_1_size,
        .out.pic_w = w,
        .out.pic_h = h,
        .out.block_offset_x = 0,
        .out.block_offset_y = 0,
        .out.blend_cm = buf_1_color_type_id,

        .mode = PPA_TRANS_MODE_BLOCKING,
    };
    // A blend client can request to do a blend operation
    TEST_ESP_OK(ppa_do_blend(ppa_client_blend_handle, &blend_oper_config));
    // A non-blend client can not request to do a blend operation
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, ppa_do_blend(ppa_client_fill_handle_b, &blend_oper_config));

    ppa_fill_oper_config_t fill_oper_config = {
        .out.buffer = buf_1,
        .out.buffer_size = buf_1_size,
        .out.pic_w = w,
        .out.pic_h = h,
        .out.block_offset_x = 0,
        .out.block_offset_y = 0,
        .out.fill_cm = buf_1_color_type_id,

        .fill_block_w = w,
        .fill_block_h = h,
        .fill_argb_color = {
            .val = 0xFF00FF00,
        },

        .mode = PPA_TRANS_MODE_NON_BLOCKING,
    };
    // A fill client can request to do a fill operation
    TEST_ESP_OK(ppa_do_fill(ppa_client_fill_handle_a, &fill_oper_config));
    // Another fill client can also request another fill operation at the same time
    TEST_ESP_OK(ppa_do_fill(ppa_client_fill_handle_b, &fill_oper_config));

    vTaskDelay(pdMS_TO_TICKS(500));

    // Unregister all PPA clients
    TEST_ESP_OK(ppa_unregister_client(ppa_client_srm_handle));
    TEST_ESP_OK(ppa_unregister_client(ppa_client_blend_handle));
    TEST_ESP_OK(ppa_unregister_client(ppa_client_fill_handle_a));
    TEST_ESP_OK(ppa_unregister_client(ppa_client_fill_handle_b));

    free(buf_1);
    free(buf_2);
}

static bool ppa_trans_done_cb(ppa_client_handle_t ppa_client, ppa_event_data_t *event_data, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    SemaphoreHandle_t sem = (SemaphoreHandle_t)user_data;
    xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
    return (xHigherPriorityTaskWoken == pdTRUE);
}

TEST_CASE("ppa_pending_transactions_in_queue", "[PPA]")
{
    // A big picture block takes longer time to process, desired for this test case
    const uint32_t w = 1920;
    const uint32_t h = 1080;
    const uint32_t buf_1_color_type_id = COLOR_TYPE_ID(COLOR_SPACE_ARGB, COLOR_PIXEL_ARGB8888);
    const uint32_t buf_2_color_type_id = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420);

    color_space_pixel_format_t buf_1_cm = {
        .color_type_id = buf_1_color_type_id,
    };
    color_space_pixel_format_t buf_2_cm = {
        .color_type_id = buf_2_color_type_id,
    };

    uint32_t buf_1_size = w * h * color_hal_pixel_format_get_bit_depth(buf_1_cm) / 8;
    uint32_t buf_2_size = ALIGN_UP(w * h * color_hal_pixel_format_get_bit_depth(buf_2_cm) / 8, 64);
    uint8_t *buf_1 = heap_caps_aligned_calloc(4, buf_1_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(buf_1);
    uint8_t *buf_2 = heap_caps_aligned_calloc(4, buf_2_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(buf_2);

    // Register two PPA SRM clients with different max_pending_trans_num
    ppa_client_handle_t ppa_client_a_handle;
    ppa_client_handle_t ppa_client_b_handle;
    ppa_client_config_t ppa_client_config = {
        .oper_type = PPA_OPERATION_SRM,
    };
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_a_handle));
    ppa_client_config.max_pending_trans_num = 3;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_b_handle));

    ppa_event_callbacks_t cbs = {
        .on_trans_done = ppa_trans_done_cb,
    };
    ppa_client_register_event_callbacks(ppa_client_a_handle, &cbs);

    SemaphoreHandle_t sem = xSemaphoreCreateBinary();

    ppa_srm_oper_config_t oper_config = {
        .in.buffer = buf_1,
        .in.pic_w = w,
        .in.pic_h = h,
        .in.block_w = w,
        .in.block_h = h,
        .in.block_offset_x = 0,
        .in.block_offset_y = 0,
        .in.srm_cm = buf_1_color_type_id,

        .out.buffer = buf_2,
        .out.buffer_size = buf_2_size,
        .out.pic_w = w,
        .out.pic_h = h,
        .out.block_offset_x = 0,
        .out.block_offset_y = 0,
        .out.srm_cm = buf_2_color_type_id,

        .rotation_angle = PPA_SRM_ROTATION_ANGLE_0,
        .scale_x = 1.0,
        .scale_y = 1.0,

        .user_data = (void *)sem,
        .mode = PPA_TRANS_MODE_NON_BLOCKING,
    };
    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_a_handle, &oper_config));

    // Another transaction cannot be accept since client_a can only hold one transaction
    TEST_ESP_ERR(ESP_FAIL, ppa_do_scale_rotate_mirror(ppa_client_a_handle, &oper_config));

    // Wait for the last transaction finishes
    xSemaphoreTake(sem, portMAX_DELAY);
    // Then a new transaction can be accepted again
    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_a_handle, &oper_config));

    // Client can not be unregistered when there are unfinished transactions
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, ppa_unregister_client(ppa_client_a_handle));

    oper_config.mode = PPA_TRANS_MODE_BLOCKING;
    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_b_handle, &oper_config));
    // Every PPA engine can only process one operation at a time
    // Transactions are being processed with First-In-First-Out
    // So, at the moment, the new transaction requested by client_b has finished, the last transaction requested by client_a for sure has finished
    TEST_ASSERT(xSemaphoreTake(sem, 0) == pdTRUE);
    // client_b can accept more than one transactions
    oper_config.mode = PPA_TRANS_MODE_NON_BLOCKING;
    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_b_handle, &oper_config));
    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_b_handle, &oper_config));
    oper_config.mode = PPA_TRANS_MODE_BLOCKING;
    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_b_handle, &oper_config));
    // The last transaction requested is with BLOCKING mode, so the last call to ppa_do_scale_rotate_mirror returned means all transactions finished

    // Unregister all PPA clients
    TEST_ESP_OK(ppa_unregister_client(ppa_client_a_handle));
    TEST_ESP_OK(ppa_unregister_client(ppa_client_b_handle));

    vSemaphoreDelete(sem);
    free(buf_1);
    free(buf_2);
}

TEST_CASE("ppa_srm_basic_data_correctness_check", "[PPA]")
{
    const uint32_t w = 4;
    const uint32_t h = 4;
    const uint32_t block_w = 3;
    const uint32_t block_h = 3;
    const uint32_t in_block_offset_x = 1;
    const uint32_t in_block_offset_y = 1;
    const uint32_t out_block_offset_x = 1;
    const uint32_t out_block_offset_y = 0;
    const ppa_srm_color_mode_t cm = PPA_SRM_COLOR_MODE_RGB565;
    const ppa_srm_rotation_angle_t rotation = PPA_SRM_ROTATION_ANGLE_90; // CCW
    const float scale_x = 1.0;
    const float scale_y = 1.0;

    color_space_pixel_format_t buf_cm = {
        .color_type_id = cm,
    };
    const uint32_t buf_len = w * h * color_hal_pixel_format_get_bit_depth(buf_cm) / 8; // 32
    uint32_t out_buf_size = ALIGN_UP(buf_len, 64);
    uint8_t *out_buf = heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(out_buf);
    esp_cache_msync((void *)out_buf, out_buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    const uint16_t in_buf[16] = {
        0xFFFF,      0xFFFF, 0xFFFF, 0xFFFF,
        //      /*******************************/
        0xFFFF, /**/ 0x8080, 0x8080, 0x8080, /**/
        0xFFFF, /**/ 0x8F80, 0x8F80, 0x8F80, /**/
        0xFFFF, /**/ 0xFF80, 0xFF80, 0xFF80, /**/
        //      /*******************************/
    };
    // Expected SRM output
    const uint16_t out_buf_expected[16] = {
        //      /*******************************/
        0x0000, /**/ 0x8080, 0x8F80, 0xFF80, /**/
        0x0000, /**/ 0x8080, 0x8F80, 0xFF80, /**/
        0x0000, /**/ 0x8080, 0x8F80, 0xFF80, /**/
        //      /*******************************/
        0x0000,      0x0000, 0x0000, 0x0000
    };

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {
        .oper_type = PPA_OPERATION_SRM,
        .max_pending_trans_num = 1,
    };
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

    ppa_srm_oper_config_t oper_config = {
        .in.buffer = in_buf,
        .in.pic_w = w,
        .in.pic_h = h,
        .in.block_w = block_w,
        .in.block_h = block_h,
        .in.block_offset_x = in_block_offset_x,
        .in.block_offset_y = in_block_offset_y,
        .in.srm_cm = cm,

        .out.buffer = out_buf,
        .out.buffer_size = out_buf_size,
        .out.pic_w = w,
        .out.pic_h = h,
        .out.block_offset_x = out_block_offset_x,
        .out.block_offset_y = out_block_offset_y,
        .out.srm_cm = cm,

        .rotation_angle = rotation,
        .scale_x = scale_x,
        .scale_y = scale_y,

        .rgb_swap = 0,
        .byte_swap = 0,

        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_handle, &oper_config));

    // Check result
    for (int i = 0; i < buf_len; i++) {
        if (i % 8 == 0) {
            printf("\n");
        }
        printf("0x%02X ", out_buf[i]);
    }
    printf("\n");
    TEST_ASSERT_EQUAL_UINT8_ARRAY((void *)out_buf_expected, (void *)out_buf, buf_len);

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));

    free(out_buf);
}

TEST_CASE("ppa_blend_basic_data_correctness_check", "[PPA]")
{
    const uint32_t w = 2;
    const uint32_t h = 2;
    const uint32_t block_w = 1;
    const uint32_t block_h = 1;
    const uint32_t block_offset_x = 1;
    const uint32_t block_offset_y = 1;
    const ppa_blend_color_mode_t in_bg_cm = PPA_BLEND_COLOR_MODE_RGB888;
    const ppa_blend_color_mode_t in_fg_cm = PPA_BLEND_COLOR_MODE_ARGB8888;
    const ppa_blend_color_mode_t out_cm = PPA_BLEND_COLOR_MODE_ARGB8888;

    color_space_pixel_format_t bg_buf_cm = {
        .color_type_id = in_bg_cm,
    };
    const uint32_t bg_buf_len __attribute__((unused)) = w * h * color_hal_pixel_format_get_bit_depth(bg_buf_cm) / 8; // 12
    color_space_pixel_format_t fg_buf_cm = {
        .color_type_id = in_fg_cm,
    };
    const uint32_t fg_buf_len __attribute__((unused)) = w * h * color_hal_pixel_format_get_bit_depth(fg_buf_cm) / 8; // 16
    const uint32_t out_buf_len = fg_buf_len; // 16
    // We will make the output write to the in_fg_buffer, therefore, it has cache line alignment requirement
    const uint32_t out_buf_size = 64;

    // Alpha of BG will be scaled by 0.8
    // Alpha of FG will be inverted
    const float bg_alpha_scale_ratio = 0.8;
    const uint8_t in_bg_buf[12] = {
        0xFF, 0xFF, 0xFF,      0xFF, 0xFF, 0xFF,
        //                /*************************/
        0xFF, 0xFF, 0xFF, /**/ 0x80, 0x40, 0xA0, /**/
        //                /*************************/
    };
    uint8_t in_fg_buf[64] __attribute__((aligned(64))) = {
        0xFF, 0xFF, 0xFF, 0xFF,      0xFF, 0xFF, 0xFF, 0xFF,
        //                      /*******************************/
        0xFF, 0xFF, 0xFF, 0xFF, /**/ 0x00, 0x80, 0x80, 0xC0, /**/
        //                      /*    (B)   (G)   (R)   (A)    */
        //                      /*******************************/
        [16 ... 63] = 0,
    };
    uint8_t *out_buf = in_fg_buf;
    // Expected blend output
    // Alpha Blending calculation:
    // A_bg' = (255 * 0.8) / 255 = 0.8, A_fg' = (255 - 0xC0) / 255 = 0.247
    // A_out = 0.8 + 0.247 - 0.8 * 0.247 = 0.849 (216 -> 0xD8)
    // C_out_b = (0x80 * 0.8 * (1 - 0.247) + 0x00 * 0.247) / 0.849 = 91 -> 0x5B
    // C_out_g = (0x40 * 0.8 * (1 - 0.247) + 0x80 * 0.247) / 0.849 = 83 -> 0x53
    // C_out_g = (0xA0 * 0.8 * (1 - 0.247) + 0x80 * 0.247) / 0.849 = 150 -> 0x96
    const uint8_t out_buf_expected[16] = {
        0xFF, 0xFF, 0xFF, 0xFF,      0xFF, 0xFF, 0xFF, 0xFF,
        //                      /*******************************/
        0xFF, 0xFF, 0xFF, 0xFF, /**/ 0x5B, 0x53, 0x96, 0xD8, /**/
        //                      /*******************************/
    };

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {
        .oper_type = PPA_OPERATION_BLEND,
        .max_pending_trans_num = 1,
    };
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

    ppa_blend_oper_config_t oper_config = {
        .in_bg.buffer = in_bg_buf,
        .in_bg.pic_w = w,
        .in_bg.pic_h = h,
        .in_bg.block_w = block_w,
        .in_bg.block_h = block_h,
        .in_bg.block_offset_x = block_offset_x,
        .in_bg.block_offset_y = block_offset_y,
        .in_bg.blend_cm = in_bg_cm,

        .in_fg.buffer = in_fg_buf,
        .in_fg.pic_w = w,
        .in_fg.pic_h = h,
        .in_fg.block_w = block_w,
        .in_fg.block_h = block_h,
        .in_fg.block_offset_x = block_offset_x,
        .in_fg.block_offset_y = block_offset_y,
        .in_fg.blend_cm = in_fg_cm,

        .out.buffer = out_buf,
        .out.buffer_size = out_buf_size,
        .out.pic_w = w,
        .out.pic_h = h,
        .out.block_offset_x = block_offset_x,
        .out.block_offset_y = block_offset_y,
        .out.blend_cm = out_cm,

        .bg_alpha_update_mode = PPA_ALPHA_SCALE,
        .bg_alpha_scale_ratio = bg_alpha_scale_ratio,

        .fg_alpha_update_mode = PPA_ALPHA_INVERT,

        .bg_ck_en = false,
        .fg_ck_en = false,

        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    TEST_ESP_OK(ppa_do_blend(ppa_client_handle, &oper_config));

    // Check result
    for (int i = 0; i < out_buf_len; i++) {
        if (i % 8 == 0) {
            printf("\n");
        }
        printf("0x%02X ", out_buf[i]);
    }
    printf("\n");
    TEST_ASSERT_EQUAL_UINT8_ARRAY((void *)out_buf_expected, (void *)out_buf, out_buf_len);

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));
}

TEST_CASE("ppa_fill_basic_data_correctness_check", "[PPA]")
{
    const uint32_t w = 80;
    const uint32_t h = 120;
    const uint32_t block_w = 80;
    const uint32_t block_h = 80;
    const uint32_t block_offset_x = 0;
    const uint32_t block_offset_y = 40;
    const ppa_fill_color_mode_t out_cm = PPA_FILL_COLOR_MODE_RGB565;
    const color_pixel_argb8888_data_t fill_color = {.a = 0x80, .r = 0xFF, .g = 0x55, .b = 0xAA};

    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = out_cm,
    };

    uint32_t out_pixel_depth = color_hal_pixel_format_get_bit_depth(out_pixel_format); // bits
    uint32_t out_buf_len = w * h * out_pixel_depth / 8;
    uint32_t out_buf_size = ALIGN_UP(out_buf_len, 64);
    uint8_t *out_buf = heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(out_buf);

    memset(out_buf, 0xFF, out_buf_len);

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {
        .oper_type = PPA_OPERATION_FILL,
        .max_pending_trans_num = 1,
    };
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

    ppa_fill_oper_config_t oper_config = {
        .out.buffer = out_buf,
        .out.buffer_size = out_buf_size,
        .out.pic_w = w,
        .out.pic_h = h,
        .out.block_offset_x = block_offset_x,
        .out.block_offset_y = block_offset_y,
        .out.fill_cm = out_cm,

        .fill_block_w = block_w,
        .fill_block_h = block_h,
        .fill_argb_color = fill_color,

        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    TEST_ESP_OK(ppa_do_fill(ppa_client_handle, &oper_config));

    // Check result
    const color_pixel_rgb565_data_t fill_pixel_expected = {.r = fill_color.r >> 3,
                                                           .g = fill_color.g >> 2,
                                                           .b = fill_color.b >> 3,
                                                          };
    TEST_ASSERT_EACH_EQUAL_UINT16(fill_pixel_expected.val, (void *)((uint32_t)out_buf + w * block_offset_y * out_pixel_depth / 8), block_w * block_h);

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));

    free(out_buf);
}

/* All performance tests are tested under the following situations:
 * - Testing PPA speed where in_buffer(s) and out_buffer all located in PSRAM
 * - Only 2D-DMA is using PSRAM
 * - 2D-DMA burst length is at maximum 128B
 * - Input and output color mode is ARGB8888 (maximizing 2D-DMA data transafer amount)
 *
 * The time spend (T) to complete a PPA transaction is proportional to the amount of pixels (x) need to be processed.
 * T = k * x + b
 * k = (T - b) / x
 */

#define PPA_SRM_MIN_PERFORMANCE_PX_PER_SEC      (21000 * 1000)  // k_min
#define PPA_SRM_TIME_OFFSET                     (-26000)        // b_approx

#define PPA_BLEND_MIN_PERFORMANCE_PX_PER_SEC    (31500 * 1000)  // k_min
#define PPA_BLEND_TIME_OFFSET                   (-37150)        // b_approx

#define PPA_FILL_MIN_PERFORMANCE_PX_PER_SEC     (150000 * 1000)  // k_min
#define PPA_FILL_TIME_OFFSET                    (-106000)        // b_approx

TEST_CASE("ppa_srm_performance", "[PPA]")
{
    // Configurable parameters
    const uint32_t w = 1920; // 1920 / 1280 / 800 / 640
    const uint32_t h = 1080; // 1080 / 720 / 480
    const uint32_t block_w = w;
    const uint32_t block_h = h;
    const ppa_srm_color_mode_t in_cm = PPA_SRM_COLOR_MODE_ARGB8888;
    const ppa_srm_color_mode_t out_cm = PPA_SRM_COLOR_MODE_ARGB8888;
    const ppa_srm_rotation_angle_t rotation = PPA_SRM_ROTATION_ANGLE_0;
    const float scale_x = 1.0;
    const float scale_y = 1.0;

    color_space_pixel_format_t in_pixel_format = {
        .color_type_id = in_cm,
    };
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = out_cm,
    };

    uint32_t in_buf_size = w * h * color_hal_pixel_format_get_bit_depth(in_pixel_format) / 8;
    uint32_t out_buf_size = ALIGN_UP(w * h * color_hal_pixel_format_get_bit_depth(out_pixel_format) / 8, 64);
    uint8_t *out_buf = heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(out_buf);
    uint8_t *in_buf = heap_caps_aligned_calloc(4, in_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(in_buf);

    uint8_t *ptr = in_buf;
    for (int x = 0; x < in_buf_size; x++) {
        ptr[x] = x;
    }

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {
        .oper_type = PPA_OPERATION_SRM,
        .max_pending_trans_num = 1,
    };
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

    uint32_t out_pic_w = (rotation == PPA_SRM_ROTATION_ANGLE_0 || rotation == PPA_SRM_ROTATION_ANGLE_180) ? w : h;
    uint32_t out_pic_h = (rotation == PPA_SRM_ROTATION_ANGLE_0 || rotation == PPA_SRM_ROTATION_ANGLE_180) ? h : w;
    ppa_srm_oper_config_t oper_config = {
        .in.buffer = in_buf,
        .in.pic_w = w,
        .in.pic_h = h,
        .in.block_w = block_w,
        .in.block_h = block_h,
        .in.block_offset_x = 0,
        .in.block_offset_y = 0,
        .in.srm_cm = in_cm,

        .out.buffer = out_buf,
        .out.buffer_size = out_buf_size,
        .out.pic_w = out_pic_w,
        .out.pic_h = out_pic_h,
        .out.block_offset_x = 0,
        .out.block_offset_y = 0,
        .out.srm_cm = out_cm,

        .rotation_angle = rotation,
        .scale_x = scale_x,
        .scale_y = scale_y,

        .rgb_swap = 0,
        .byte_swap = 0,

        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    ccomp_timer_start();

    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_handle, &oper_config));

    int64_t oper_time = ccomp_timer_stop(); // us
    printf("PPA SRM - Process Time: %lld us\n", oper_time);

    // Check performance
    uint64_t num_pixels_processed = block_w * block_h;
    uint64_t px_per_second = (num_pixels_processed - PPA_SRM_TIME_OFFSET) * 1000 * 1000 / oper_time;
    printf("PPA SRM performance = %lld pixels/sec\n", px_per_second);
    TEST_ASSERT_GREATER_THAN(PPA_SRM_MIN_PERFORMANCE_PX_PER_SEC, px_per_second);

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));

    free(in_buf);
    free(out_buf);
}

TEST_CASE("ppa_blend_performance", "[PPA]")
{
    // Configurable parameters
    const uint32_t w = 1280;
    const uint32_t h = 720;
    const uint32_t block_w = w;
    const uint32_t block_h = h;
    const ppa_blend_color_mode_t in_bg_cm = PPA_BLEND_COLOR_MODE_ARGB8888;
    const ppa_blend_color_mode_t in_fg_cm = PPA_BLEND_COLOR_MODE_ARGB8888;
    const ppa_blend_color_mode_t out_cm = PPA_BLEND_COLOR_MODE_ARGB8888;

    color_space_pixel_format_t in_bg_pixel_format = {
        .color_type_id = in_bg_cm,
    };
    color_space_pixel_format_t in_fg_pixel_format = {
        .color_type_id = in_fg_cm,
    };
    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = out_cm,
    };

    uint32_t in_bg_buf_size = w * h * color_hal_pixel_format_get_bit_depth(in_bg_pixel_format) / 8;
    uint32_t in_fg_buf_size = w * h * color_hal_pixel_format_get_bit_depth(in_fg_pixel_format) / 8;
    uint32_t out_buf_size = ALIGN_UP(w * h * color_hal_pixel_format_get_bit_depth(out_pixel_format) / 8, 64);
    uint8_t *out_buf = heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(out_buf);
    uint8_t *in_bg_buf = heap_caps_aligned_calloc(4, in_bg_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(in_bg_buf);
    uint8_t *in_fg_buf = heap_caps_aligned_calloc(4, in_fg_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(in_fg_buf);

    uint8_t *ptr = in_bg_buf;
    for (int x = 0; x < in_bg_buf_size; x++) {
        ptr[x] = x & 0x55;
    }
    ptr = in_fg_buf;
    for (int x = 0; x < in_fg_buf_size; x++) {
        ptr[x] = x & 0xAA;
    }

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {
        .oper_type = PPA_OPERATION_BLEND,
        .max_pending_trans_num = 1,
    };
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

    ppa_blend_oper_config_t oper_config = {
        .in_bg.buffer = in_bg_buf,
        .in_bg.pic_w = w,
        .in_bg.pic_h = h,
        .in_bg.block_w = block_w,
        .in_bg.block_h = block_h,
        .in_bg.block_offset_x = 0,
        .in_bg.block_offset_y = 0,
        .in_bg.blend_cm = in_bg_cm,

        .in_fg.buffer = in_fg_buf,
        .in_fg.pic_w = w,
        .in_fg.pic_h = h,
        .in_fg.block_w = block_w,
        .in_fg.block_h = block_h,
        .in_fg.block_offset_x = 0,
        .in_fg.block_offset_y = 0,
        .in_fg.blend_cm = in_fg_cm,

        .out.buffer = out_buf,
        .out.buffer_size = out_buf_size,
        .out.pic_w = w,
        .out.pic_h = h,
        .out.block_offset_x = 0,
        .out.block_offset_y = 0,
        .out.blend_cm = out_cm,

        .bg_ck_en = false,
        .fg_ck_en = false,

        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    ccomp_timer_start();

    TEST_ESP_OK(ppa_do_blend(ppa_client_handle, &oper_config));

    int64_t oper_time = ccomp_timer_stop();
    printf("PPA Blend - Process Time: %lld us\n", oper_time);

    // Check performance
    uint64_t num_pixels_processed = block_w * block_h;
    uint64_t px_per_second = (num_pixels_processed - PPA_BLEND_TIME_OFFSET) * 1000 * 1000 / oper_time;
    printf("PPA Blend performance = %lld pixels/sec\n", px_per_second);
    TEST_ASSERT_GREATER_THAN(PPA_BLEND_MIN_PERFORMANCE_PX_PER_SEC, px_per_second);

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));

    free(in_bg_buf);
    free(in_fg_buf);
    free(out_buf);
}

TEST_CASE("ppa_fill_performance", "[PPA]")
{
    // Configurable parameters
    const uint32_t w = 1280;
    const uint32_t h = 720;
    const uint32_t block_w = 800;
    const uint32_t block_h = 480;
    const ppa_fill_color_mode_t out_cm = PPA_FILL_COLOR_MODE_RGB565;

    color_space_pixel_format_t out_pixel_format = {
        .color_type_id = out_cm,
    };

    uint32_t out_buf_size = ALIGN_UP(w * h * color_hal_pixel_format_get_bit_depth(out_pixel_format) / 8, 64);
    uint8_t *out_buf = heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(out_buf);

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {
        .oper_type = PPA_OPERATION_FILL,
        .max_pending_trans_num = 1,
    };
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

    ppa_fill_oper_config_t oper_config = {
        .out.buffer = out_buf,
        .out.buffer_size = out_buf_size,
        .out.pic_w = w,
        .out.pic_h = h,
        .out.block_offset_x = 0,
        .out.block_offset_y = 0,
        .out.fill_cm = out_cm,

        .fill_block_w = block_w,
        .fill_block_h = block_h,
        .fill_argb_color = {
            .val = 0xFF00FFFF,
        },

        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    ccomp_timer_start();

    TEST_ESP_OK(ppa_do_fill(ppa_client_handle, &oper_config));

    int64_t oper_time = ccomp_timer_stop();
    printf("PPA Fill - Process Time: %lld us\n", oper_time);

    // Check performance
    uint64_t num_pixels_processed = block_w * block_h;
    uint64_t px_per_second = (num_pixels_processed - PPA_FILL_TIME_OFFSET) * 1000 * 1000 / oper_time;
    printf("PPA Blend performance = %lld pixels/sec\n", px_per_second);
    TEST_ASSERT_GREATER_THAN(PPA_FILL_MIN_PERFORMANCE_PX_PER_SEC, px_per_second);

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));

    free(out_buf);
}
