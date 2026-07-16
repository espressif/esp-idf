/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
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
#include "ppa_performance.h"
#include "esp_random.h"
#include "esp_efuse.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_pm.h"
#include "esp_clk_tree.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))

TEST_CASE("ppa_client_do_ppa_operation", "[PPA]")
{
    const uint32_t w = 480;
    const uint32_t h = 480;
    const esp_color_fourcc_t buf_1_color_type_id = ESP_COLOR_FOURCC_BGRA32;
    const esp_color_fourcc_t buf_2_color_type_id = ESP_COLOR_FOURCC_BGRA32;

    uint32_t buf_1_size = ALIGN_UP(w * h * color_hal_pixel_format_fourcc_get_bit_depth(buf_1_color_type_id) / 8, 64);
    uint32_t buf_2_size = ALIGN_UP(w * h * color_hal_pixel_format_fourcc_get_bit_depth(buf_2_color_type_id) / 8, 64);
    uint8_t *buf_1 = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, buf_1_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA)); // cache alignment is implicited by MALLOC_CAP_DMA
    TEST_ASSERT_NOT_NULL(buf_1);
    uint8_t *buf_2 = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, buf_2_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(buf_2);

    // Register different types of PPA clients
    ppa_client_handle_t ppa_client_srm_handle;
    ppa_client_handle_t ppa_client_blend_handle;
    ppa_client_handle_t ppa_client_fill_handle_a;
    ppa_client_handle_t ppa_client_fill_handle_b;
    ppa_client_config_t ppa_client_config = {};
    ppa_client_config.oper_type = PPA_OPERATION_SRM;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_srm_handle));
    ppa_client_config.oper_type = PPA_OPERATION_BLEND;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_blend_handle));
    ppa_client_config.oper_type = PPA_OPERATION_FILL;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_fill_handle_a));
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_fill_handle_b));

    ppa_srm_oper_config_t srm_oper_config = {};
    srm_oper_config.in.buffer = buf_1;
    srm_oper_config.in.pic_w = w;
    srm_oper_config.in.pic_h = h;
    srm_oper_config.in.block_w = w;
    srm_oper_config.in.block_h = h;
    srm_oper_config.in.block_offset_x = 0;
    srm_oper_config.in.block_offset_y = 0;
    srm_oper_config.in.srm_cm = static_cast<ppa_srm_color_mode_t>(buf_1_color_type_id);
    srm_oper_config.out.buffer = buf_2;
    srm_oper_config.out.buffer_size = buf_2_size;
    srm_oper_config.out.pic_w = w;
    srm_oper_config.out.pic_h = h;
    srm_oper_config.out.block_offset_x = 0;
    srm_oper_config.out.block_offset_y = 0;
    srm_oper_config.out.srm_cm = static_cast<ppa_srm_color_mode_t>(buf_2_color_type_id);
    srm_oper_config.rotation_angle = PPA_SRM_ROTATION_ANGLE_0;
    srm_oper_config.scale_x = 1.0;
    srm_oper_config.scale_y = 1.0;
    srm_oper_config.mode = PPA_TRANS_MODE_BLOCKING;
    // A SRM client can request to do a SRM operation
    if (!esp_efuse_is_flash_encryption_enabled()) {
        TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_srm_handle, &srm_oper_config));
    }
    // A non-SRM client can not request to do a SRM operation
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, ppa_do_scale_rotate_mirror(ppa_client_blend_handle, &srm_oper_config));

    ppa_blend_oper_config_t blend_oper_config = {};
    blend_oper_config.in_bg.buffer = buf_1;
    blend_oper_config.in_bg.pic_w = w;
    blend_oper_config.in_bg.pic_h = h;
    blend_oper_config.in_bg.block_w = w;
    blend_oper_config.in_bg.block_h = h;
    blend_oper_config.in_bg.block_offset_x = 0;
    blend_oper_config.in_bg.block_offset_y = 0;
    blend_oper_config.in_bg.blend_cm = static_cast<ppa_blend_color_mode_t>(buf_1_color_type_id);
    blend_oper_config.in_fg.buffer = buf_2;
    blend_oper_config.in_fg.pic_w = w;
    blend_oper_config.in_fg.pic_h = h;
    blend_oper_config.in_fg.block_w = w;
    blend_oper_config.in_fg.block_h = h;
    blend_oper_config.in_fg.block_offset_x = 0;
    blend_oper_config.in_fg.block_offset_y = 0;
    blend_oper_config.in_fg.blend_cm = static_cast<ppa_blend_color_mode_t>(buf_2_color_type_id);
    blend_oper_config.out.buffer = buf_1;
    blend_oper_config.out.buffer_size = buf_1_size;
    blend_oper_config.out.pic_w = w;
    blend_oper_config.out.pic_h = h;
    blend_oper_config.out.block_offset_x = 0;
    blend_oper_config.out.block_offset_y = 0;
    blend_oper_config.out.blend_cm = static_cast<ppa_blend_color_mode_t>(buf_1_color_type_id);
    blend_oper_config.mode = PPA_TRANS_MODE_BLOCKING;
    // A blend client can request to do a blend operation
    TEST_ESP_OK(ppa_do_blend(ppa_client_blend_handle, &blend_oper_config));
    // A non-blend client can not request to do a blend operation
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, ppa_do_blend(ppa_client_fill_handle_b, &blend_oper_config));

    ppa_fill_oper_config_t fill_oper_config = {};
    fill_oper_config.out.buffer = buf_1;
    fill_oper_config.out.buffer_size = buf_1_size;
    fill_oper_config.out.pic_w = w;
    fill_oper_config.out.pic_h = h;
    fill_oper_config.out.block_offset_x = 0;
    fill_oper_config.out.block_offset_y = 0;
    fill_oper_config.out.fill_cm = static_cast<ppa_fill_color_mode_t>(buf_1_color_type_id);
    fill_oper_config.fill_block_w = w;
    fill_oper_config.fill_block_h = h;
    fill_oper_config.fill_argb_color.val = 0xFF00FF00;
    fill_oper_config.mode = PPA_TRANS_MODE_NON_BLOCKING;
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
    // this test tests software correctness, and SRM does not work with flash encrypted if buffer is in external memory, so skip
    if (esp_efuse_is_flash_encryption_enabled()) {
        TEST_PASS_MESSAGE("Flash encryption is enabled, skip this test");
    }

    // A big picture block takes longer time to process, desired for this test case
    const uint32_t w = 1920;
    const uint32_t h = 1080;
    const esp_color_fourcc_t buf_1_color_type_id = ESP_COLOR_FOURCC_BGRA32;
    const esp_color_fourcc_t buf_2_color_type_id = ESP_COLOR_FOURCC_OUYY_EVYY;

    uint32_t buf_1_size = w * h * color_hal_pixel_format_fourcc_get_bit_depth(buf_1_color_type_id) / 8;
    uint32_t buf_2_size = ALIGN_UP(w * h * color_hal_pixel_format_fourcc_get_bit_depth(buf_2_color_type_id) / 8, 64);
    uint8_t *buf_1 = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, buf_1_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(buf_1);
    uint8_t *buf_2 = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, buf_2_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(buf_2);

    // Register two PPA SRM clients with different max_pending_trans_num
    ppa_client_handle_t ppa_client_a_handle;
    ppa_client_handle_t ppa_client_b_handle;
    ppa_client_config_t ppa_client_config = {};
    ppa_client_config.oper_type = PPA_OPERATION_SRM;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_a_handle));
    ppa_client_config.max_pending_trans_num = 3;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_b_handle));

    ppa_event_callbacks_t cbs = {};
    cbs.on_trans_done = ppa_trans_done_cb;
    ppa_client_register_event_callbacks(ppa_client_a_handle, &cbs);

    SemaphoreHandle_t sem = xSemaphoreCreateBinary();

    ppa_srm_oper_config_t oper_config = {};
    oper_config.in.buffer = buf_1;
    oper_config.in.pic_w = w;
    oper_config.in.pic_h = h;
    oper_config.in.block_w = w;
    oper_config.in.block_h = h;
    oper_config.in.block_offset_x = 0;
    oper_config.in.block_offset_y = 0;
    oper_config.in.srm_cm = static_cast<ppa_srm_color_mode_t>(buf_1_color_type_id);
    oper_config.out.buffer = buf_2;
    oper_config.out.buffer_size = buf_2_size;
    oper_config.out.pic_w = w;
    oper_config.out.pic_h = h;
    oper_config.out.block_offset_x = 0;
    oper_config.out.block_offset_y = 0;
    oper_config.out.srm_cm = static_cast<ppa_srm_color_mode_t>(buf_2_color_type_id);
    oper_config.rotation_angle = PPA_SRM_ROTATION_ANGLE_0;
    oper_config.scale_x = 1.0;
    oper_config.scale_y = 1.0;
    oper_config.user_data = (void *)sem;
    oper_config.mode = PPA_TRANS_MODE_NON_BLOCKING;
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

static __attribute__((unused)) void enable_pm_strategy(bool auto_light_sleep)
{
    uint32_t xtal_hz = 0;
    esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_XTAL, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &xtal_hz);
    esp_pm_config_t pm_config = {};
    pm_config.max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ;
    pm_config.min_freq_mhz = static_cast<int>(xtal_hz / 1000000);
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
    pm_config.light_sleep_enable = auto_light_sleep;
#endif
    TEST_ESP_OK(esp_pm_configure(&pm_config));
}

static void ppa_srm_basic_data_correctness_check(bool auto_light_sleep)
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

    const uint32_t buf_len = w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)cm) / 8; // 32
    uint32_t out_buf_size = ALIGN_UP(buf_len, 64);
    uint8_t *out_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA)); // located in internal RAM so even w/ flash encrypted, it won't be affected
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

#if CONFIG_PM_ENABLE
    enable_pm_strategy(auto_light_sleep);
#endif

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {};
    ppa_client_config.oper_type = PPA_OPERATION_SRM;
    ppa_client_config.max_pending_trans_num = 1;
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    ppa_client_config.flags.allow_pd = (auto_light_sleep ? true : false);
#endif
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
#endif

    // loop 3 times if we are testing PPA sleep retention feature
    int cnt = (auto_light_sleep ? 3 : 1);
    for (int i = 0; i < cnt; i++) {
#if SOC_LIGHT_SLEEP_SUPPORTED
        sleep_ctx.sleep_request_result = ESP_FAIL; // set back to fail for new iteration
        sleep_ctx.sleep_flags = 0; // clear sleep flags for new iteration

        if (auto_light_sleep) {
            vTaskDelay(pdMS_TO_TICKS(1000)); // enter auto light sleep here if we are testing PPA sleep retention feature

            // Check if the sleep happened as expected
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
            TEST_ASSERT_EQUAL(PMU_SLEEP_PD_TOP, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
#endif
            TEST_ASSERT_EQUAL(ESP_OK, sleep_ctx.sleep_request_result);
        }
#endif

        ppa_srm_oper_config_t oper_config = {};
        oper_config.in.buffer = in_buf;
        oper_config.in.pic_w = w;
        oper_config.in.pic_h = h;
        oper_config.in.block_w = block_w;
        oper_config.in.block_h = block_h;
        oper_config.in.block_offset_x = in_block_offset_x;
        oper_config.in.block_offset_y = in_block_offset_y;
        oper_config.in.srm_cm = cm;
        oper_config.out.buffer = out_buf;
        oper_config.out.buffer_size = out_buf_size;
        oper_config.out.pic_w = w;
        oper_config.out.pic_h = h;
        oper_config.out.block_offset_x = out_block_offset_x;
        oper_config.out.block_offset_y = out_block_offset_y;
        oper_config.out.srm_cm = cm;
        oper_config.rotation_angle = rotation;
        oper_config.scale_x = scale_x;
        oper_config.scale_y = scale_y;
        oper_config.rgb_swap = 0;
        oper_config.byte_swap = 0;
        oper_config.mode = PPA_TRANS_MODE_BLOCKING;

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

#if !(CONFIG_IDF_TARGET_ESP32P4 && CONFIG_ESP32P4_SELECTS_REV_LESS_V3)
        // Test a rgb2gray color conversion
        memset(out_buf, 0, out_buf_size);
        esp_cache_msync((void *)out_buf, out_buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        const uint8_t r_weight = 100;
        const uint8_t g_weight = 56;
        const uint8_t b_weight = 100;
        TEST_ESP_OK(ppa_set_rgb2gray_formula(r_weight, g_weight, b_weight));
        oper_config.out.srm_cm = PPA_SRM_COLOR_MODE_GRAY8;
        oper_config.rotation_angle = PPA_SRM_ROTATION_ANGLE_0;
        uint8_t out_buf_expected_gray[16] = {};
        for (int i = 0; i < block_w * block_h; i++) {
            const uint16_t pix = in_buf[(i / block_w + in_block_offset_y) * w + (i % block_w + in_block_offset_x)];
            uint8_t _r = ((pix >> 8) & 0xF8);
            uint8_t _g = ((pix >> 3) & 0xFC);
            uint8_t _b = ((pix << 3) & 0xF8);
            out_buf_expected_gray[(i / block_w + out_block_offset_y) * w + (i % block_w + out_block_offset_x)] = (_r * r_weight + _g * g_weight + _b * b_weight) >> 8;
        }

        TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_handle, &oper_config));

        // Check result
        for (int i = 0; i < w * h; i++) {
            if (i % 4 == 0) {
                printf("\n");
            }
            printf("0x%02X ", out_buf[i]);
        }
        printf("\n");
        TEST_ASSERT_EQUAL_UINT8_ARRAY((void *)out_buf_expected_gray, (void *)out_buf, w * h);
#endif // !(CONFIG_IDF_TARGET_ESP32P4 && CONFIG_ESP32P4_SELECTS_REV_LESS_V3)

        memset(out_buf, 0, out_buf_size);
        esp_cache_msync((void *)out_buf, out_buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    }
#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_set_sleep_context(NULL);
#endif

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));

#if CONFIG_PM_ENABLE
    enable_pm_strategy(false);
#endif

    free(out_buf);
}

TEST_CASE("ppa_srm_basic_data_correctness_check", "[PPA]")
{
    ppa_srm_basic_data_correctness_check(false);
#if CONFIG_PM_ENABLE
#if !(CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP && !SOC_PM_FPU_RETENTION_BY_SW) // SRM uses FPU
    printf("\nTest again with auto light sleep...\n");
    ppa_srm_basic_data_correctness_check(true);
#endif
#endif
}

static void ppa_blend_basic_data_correctness_check(bool auto_light_sleep)
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

    const uint32_t bg_buf_len __attribute__((unused)) = w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)in_bg_cm) / 8; // 12
    const uint32_t fg_buf_len __attribute__((unused)) = w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)in_fg_cm) / 8; // 16
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
    const uint8_t in_fg_buf_template[64] __attribute__((aligned(64))) = {
        0xFF, 0xFF, 0xFF, 0xFF,      0xFF, 0xFF, 0xFF, 0xFF,
        //                      /*******************************/
        0xFF, 0xFF, 0xFF, 0xFF, /**/ 0x00, 0x80, 0x80, 0xC0, /**/
        //                      /*    (B)   (G)   (R)   (A)    */
        //                      /*******************************/
        // remaining bytes [16 ... 63] are value-initialized to 0
    };
    uint8_t *in_fg_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, 64, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(in_fg_buf);
    memcpy(in_fg_buf, in_fg_buf_template, 64);
    esp_cache_msync((void *)in_fg_buf, 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
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

#if CONFIG_PM_ENABLE
    enable_pm_strategy(auto_light_sleep);
#endif

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {};
    ppa_client_config.oper_type = PPA_OPERATION_BLEND;
    ppa_client_config.max_pending_trans_num = 1;
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    ppa_client_config.flags.allow_pd = (auto_light_sleep ? true : false);
#endif
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
#endif

    // loop 3 times if we are testing PPA sleep retention feature
    int cnt = (auto_light_sleep ? 3 : 1);
    for (int i = 0; i < cnt; i++) {
#if SOC_LIGHT_SLEEP_SUPPORTED
        sleep_ctx.sleep_request_result = ESP_FAIL; // set back to fail for new iteration
        sleep_ctx.sleep_flags = 0; // clear sleep flags for new iteration

        if (auto_light_sleep) {
            vTaskDelay(pdMS_TO_TICKS(1000)); // enter auto light sleep here if we are testing PPA sleep retention feature

            // Check if the sleep happened as expected
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
            TEST_ASSERT_EQUAL(PMU_SLEEP_PD_TOP, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
#endif
            TEST_ASSERT_EQUAL(ESP_OK, sleep_ctx.sleep_request_result);
        }
#endif

        ppa_blend_oper_config_t oper_config = {};
        oper_config.in_bg.buffer = in_bg_buf;
        oper_config.in_bg.pic_w = w;
        oper_config.in_bg.pic_h = h;
        oper_config.in_bg.block_w = block_w;
        oper_config.in_bg.block_h = block_h;
        oper_config.in_bg.block_offset_x = block_offset_x;
        oper_config.in_bg.block_offset_y = block_offset_y;
        oper_config.in_bg.blend_cm = in_bg_cm;
        oper_config.in_fg.buffer = in_fg_buf;
        oper_config.in_fg.pic_w = w;
        oper_config.in_fg.pic_h = h;
        oper_config.in_fg.block_w = block_w;
        oper_config.in_fg.block_h = block_h;
        oper_config.in_fg.block_offset_x = block_offset_x;
        oper_config.in_fg.block_offset_y = block_offset_y;
        oper_config.in_fg.blend_cm = in_fg_cm;
        oper_config.out.buffer = out_buf;
        oper_config.out.buffer_size = out_buf_size;
        oper_config.out.pic_w = w;
        oper_config.out.pic_h = h;
        oper_config.out.block_offset_x = block_offset_x;
        oper_config.out.block_offset_y = block_offset_y;
        oper_config.out.blend_cm = out_cm;
        oper_config.bg_alpha_update_mode = PPA_ALPHA_SCALE;
        oper_config.bg_alpha_scale_ratio = bg_alpha_scale_ratio;
        oper_config.fg_alpha_update_mode = PPA_ALPHA_INVERT;
        oper_config.bg_ck_en = false;
        oper_config.fg_ck_en = false;
        oper_config.mode = PPA_TRANS_MODE_BLOCKING;

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

        memcpy(in_fg_buf, in_fg_buf_template, 64);
        esp_cache_msync((void *)in_fg_buf, 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    }
#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_set_sleep_context(NULL);
#endif

#if !(CONFIG_IDF_TARGET_ESP32P4 && CONFIG_ESP32P4_SELECTS_REV_LESS_V3)
    // Test YUV422/YUV420 blend

    // A mid-grey image is achromatic (U = V = 128), so a correct RGB<->YUV path must round-trip
    // it back to grey with R == G == B. For each format we run RGB->YUV (exercises YUV as the
    // blend output) then YUV->RGB (exercises YUV as the blend background input); both must
    // complete without hanging and yield achromatic grey

    // 2 x 2 is the smallest valid YUV block: even w/h satisfies YUV422 (2x1) & YUV420 (2x2)
    const uint8_t grey = 0x80; // mid-grey: Y arbitrary, U = V = 128 -> R == G == B after decode

    // RGB888 mid-grey background (12B) and ARGB8888 foreground (16B). The foreground alpha is
    // inverted to 0, so it contributes nothing and the blend output equals the background color.
    uint8_t rgb_grey[12];
    memset(rgb_grey, grey, sizeof(rgb_grey));
    uint8_t fg_buf[16];
    memset(fg_buf, 0xFF, sizeof(fg_buf));
    // DMA outputs require cache-line alignment; yuv_mid is also reused as input for the YUV->RGB pass.
    uint8_t yuv_mid[64] __attribute__((aligned(64))) = {};
    uint8_t rgb_back[64] __attribute__((aligned(64)));
    memset(rgb_back, 0xCC, sizeof(rgb_back));
    const uint32_t yuv_buf_size = sizeof(yuv_mid);

    const ppa_blend_color_mode_t yuv_cms[] = {
        PPA_BLEND_COLOR_MODE_YUV422_UYVY,
        PPA_BLEND_COLOR_MODE_YUV420,
    };
    for (int c = 0; c < 2; c++) {
        const ppa_blend_color_mode_t yuv_cm = yuv_cms[c];

        // Foreground contributes nothing: alpha 0xFF inverted to 0, so output == background.
        ppa_blend_oper_config_t yuv_oper_config = {};
        yuv_oper_config.in_fg.buffer = fg_buf;
        yuv_oper_config.in_fg.pic_w = w;
        yuv_oper_config.in_fg.pic_h = h;
        yuv_oper_config.in_fg.block_w = w;
        yuv_oper_config.in_fg.block_h = h;
        yuv_oper_config.in_fg.blend_cm = PPA_BLEND_COLOR_MODE_ARGB8888;
        yuv_oper_config.bg_alpha_update_mode = PPA_ALPHA_NO_CHANGE;
        yuv_oper_config.fg_alpha_update_mode = PPA_ALPHA_INVERT;
        yuv_oper_config.mode = PPA_TRANS_MODE_BLOCKING;

        // 1) RGB888 grey -> YUV (exercises YUV as blend output; must not hang)
        yuv_oper_config.in_bg.buffer = rgb_grey;
        yuv_oper_config.in_bg.pic_w = w;
        yuv_oper_config.in_bg.pic_h = h;
        yuv_oper_config.in_bg.block_w = w;
        yuv_oper_config.in_bg.block_h = h;
        yuv_oper_config.in_bg.blend_cm = PPA_BLEND_COLOR_MODE_RGB888;
        yuv_oper_config.out.buffer = yuv_mid;
        yuv_oper_config.out.buffer_size = yuv_buf_size;
        yuv_oper_config.out.pic_w = w;
        yuv_oper_config.out.pic_h = h;
        yuv_oper_config.out.blend_cm = yuv_cm;
        TEST_ESP_OK(ppa_do_blend(ppa_client_handle, &yuv_oper_config));

        // 2) YUV grey -> RGB888 (exercises YUV as blend background input; must not hang)
        yuv_oper_config.in_bg.buffer = yuv_mid;
        yuv_oper_config.in_bg.blend_cm = yuv_cm;
        yuv_oper_config.out.buffer = rgb_back;
        yuv_oper_config.out.blend_cm = PPA_BLEND_COLOR_MODE_RGB888;
        TEST_ESP_OK(ppa_do_blend(ppa_client_handle, &yuv_oper_config));

        // Check result
        // Mid-grey is achromatic: every RGB888 pixel must stay near-grey, i.e. R ~= G ~= B
        // (chroma preserved) and the level must remain mid-grey (luma not driven to
        // black/white). A small tolerance covers the +-1 LSB rounding of two YUV conversions.
        for (int p = 0; p < w * h; p++) {
            const int b = rgb_back[p * 3 + 0];
            const int g = rgb_back[p * 3 + 1];
            const int r = rgb_back[p * 3 + 2];
            TEST_ASSERT_INT_WITHIN(2, b, g);    // achromatic: channels stay together
            TEST_ASSERT_INT_WITHIN(2, g, r);
            TEST_ASSERT_INT_WITHIN(16, grey, b); // round-trips back to ~mid-grey
        }
    }
#endif // !(CONFIG_IDF_TARGET_ESP32P4 && CONFIG_ESP32P4_SELECTS_REV_LESS_V3)

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));

#if CONFIG_PM_ENABLE
    enable_pm_strategy(false);
#endif

    free(in_fg_buf);
}

TEST_CASE("ppa_blend_basic_data_correctness_check", "[PPA]")
{
    ppa_blend_basic_data_correctness_check(false);
#if CONFIG_PM_ENABLE
#if !(CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP && !SOC_PM_FPU_RETENTION_BY_SW) // Blend uses FPU
    printf("\nTest again with auto light sleep...\n");
    ppa_blend_basic_data_correctness_check(true);
#endif
#endif
}

static void ppa_fill_basic_data_correctness_check(bool auto_light_sleep)
{
    const uint32_t w = 80;
    const uint32_t h = 120;
    const uint32_t block_w = 80;
    const uint32_t block_h = 80;
    const uint32_t block_offset_x = 0;
    const uint32_t block_offset_y = 40;
    const ppa_fill_color_mode_t out_cm = PPA_FILL_COLOR_MODE_RGB565;
    color_pixel_argb8888_data_t fill_color = {};
    fill_color.a = 0x80;
    fill_color.r = 0xFF;
    fill_color.g = 0x55;
    fill_color.b = 0xAA;

    uint32_t out_pixel_depth = color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)out_cm); // bits
    uint32_t out_buf_len = w * h * out_pixel_depth / 8;
    uint32_t out_buf_size = ALIGN_UP(out_buf_len, 64);
    uint8_t *out_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(out_buf);

    memset(out_buf, 0xFF, out_buf_len);

#if CONFIG_PM_ENABLE
    enable_pm_strategy(auto_light_sleep);
#endif

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {};
    ppa_client_config.oper_type = PPA_OPERATION_FILL;
    ppa_client_config.max_pending_trans_num = 1;
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    ppa_client_config.flags.allow_pd = (auto_light_sleep ? true : false);
#endif
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
#endif

    // loop 3 times if we are testing PPA sleep retention feature
    int cnt = (auto_light_sleep ? 3 : 1);
    for (int i = 0; i < cnt; i++) {
#if SOC_LIGHT_SLEEP_SUPPORTED
        sleep_ctx.sleep_request_result = ESP_FAIL; // set back to fail for new iteration
        sleep_ctx.sleep_flags = 0; // clear sleep flags for new iteration

        if (auto_light_sleep) {
            vTaskDelay(pdMS_TO_TICKS(1000)); // enter auto light sleep here if we are testing PPA sleep retention feature

            // Check if the sleep happened as expected
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
            TEST_ASSERT_EQUAL(PMU_SLEEP_PD_TOP, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
#endif
            TEST_ASSERT_EQUAL(ESP_OK, sleep_ctx.sleep_request_result);
        }
#endif

        ppa_fill_oper_config_t oper_config = {};
        oper_config.out.buffer = out_buf;
        oper_config.out.buffer_size = out_buf_size;
        oper_config.out.pic_w = w;
        oper_config.out.pic_h = h;
        oper_config.out.block_offset_x = block_offset_x;
        oper_config.out.block_offset_y = block_offset_y;
        oper_config.out.fill_cm = out_cm;
        oper_config.fill_block_w = block_w;
        oper_config.fill_block_h = block_h;
        oper_config.fill_argb_color = fill_color;
        oper_config.mode = PPA_TRANS_MODE_BLOCKING;

        TEST_ESP_OK(ppa_do_fill(ppa_client_handle, &oper_config));

        // Check result
        color_pixel_rgb565_data_t fill_pixel_expected = {};
        fill_pixel_expected.r = fill_color.r >> 3;
        fill_pixel_expected.g = fill_color.g >> 2;
        fill_pixel_expected.b = fill_color.b >> 3;
        TEST_ASSERT_EACH_EQUAL_UINT16(fill_pixel_expected.val, (void *)((uint32_t)out_buf + w * block_offset_y * out_pixel_depth / 8), block_w * block_h);

#if !(CONFIG_IDF_TARGET_ESP32P4 && CONFIG_ESP32P4_SELECTS_REV_LESS_V3)
        // Test a yuv color fill
        oper_config.out.fill_cm = PPA_FILL_COLOR_MODE_YUV422_UYVY; // output YUV422 is with UYVY packed order
        color_macroblock_yuv_data_t fill_yuv_color = {};
        fill_yuv_color.y = 0xFF;
        fill_yuv_color.u = 0x55;
        fill_yuv_color.v = 0xAA;
        oper_config.fill_yuv_color = fill_yuv_color;
        out_pixel_depth = color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)PPA_FILL_COLOR_MODE_YUV422_UYVY); // bits
        TEST_ESP_OK(ppa_do_fill(ppa_client_handle, &oper_config));

        // Check result (2 pixels per macro pixel)
        const uint32_t fill_pixel_expected_yuv422 = ((fill_yuv_color.y << 24) | (fill_yuv_color.v << 16) | (fill_yuv_color.y << 8) | (fill_yuv_color.u));
        TEST_ASSERT_EACH_EQUAL_UINT32(fill_pixel_expected_yuv422, (void *)((uint32_t)out_buf + w * block_offset_y * out_pixel_depth / 8), block_w * block_h / 2);
#endif // !(CONFIG_IDF_TARGET_ESP32P4 && CONFIG_ESP32P4_SELECTS_REV_LESS_V3)

        memset(out_buf, 0xFF, out_buf_size);
        esp_cache_msync((void *)out_buf, out_buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    }
#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_set_sleep_context(NULL);
#endif

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));

#if CONFIG_PM_ENABLE
    enable_pm_strategy(false);
#endif

    free(out_buf);
}

TEST_CASE("ppa_fill_basic_data_correctness_check", "[PPA]")
{
    ppa_fill_basic_data_correctness_check(false);
#if CONFIG_PM_ENABLE
    printf("\nTest again with auto light sleep...\n");
    ppa_fill_basic_data_correctness_check(true);
#endif
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

TEST_CASE("ppa_srm_performance", "[PPA]")
{
    // SRM does not work with flash encrypted if buffer is in external memory, so skip
    if (esp_efuse_is_flash_encryption_enabled()) {
        TEST_PASS_MESSAGE("Flash encryption is enabled, skip this test");
    }

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

    uint32_t in_buf_size = w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)in_cm) / 8;
    uint32_t out_buf_size = ALIGN_UP(w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)out_cm) / 8, 64);
    uint8_t *out_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(out_buf);
    uint8_t *in_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, in_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(in_buf);

    uint8_t *ptr = in_buf;
    for (int x = 0; x < in_buf_size; x++) {
        ptr[x] = x;
    }

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {};
    ppa_client_config.oper_type = PPA_OPERATION_SRM;
    ppa_client_config.max_pending_trans_num = 1;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

    uint32_t out_pic_w = (rotation == PPA_SRM_ROTATION_ANGLE_0 || rotation == PPA_SRM_ROTATION_ANGLE_180) ? w : h;
    uint32_t out_pic_h = (rotation == PPA_SRM_ROTATION_ANGLE_0 || rotation == PPA_SRM_ROTATION_ANGLE_180) ? h : w;
    ppa_srm_oper_config_t oper_config = {};
    oper_config.in.buffer = in_buf;
    oper_config.in.pic_w = w;
    oper_config.in.pic_h = h;
    oper_config.in.block_w = block_w;
    oper_config.in.block_h = block_h;
    oper_config.in.block_offset_x = 0;
    oper_config.in.block_offset_y = 0;
    oper_config.in.srm_cm = in_cm;
    oper_config.out.buffer = out_buf;
    oper_config.out.buffer_size = out_buf_size;
    oper_config.out.pic_w = out_pic_w;
    oper_config.out.pic_h = out_pic_h;
    oper_config.out.block_offset_x = 0;
    oper_config.out.block_offset_y = 0;
    oper_config.out.srm_cm = out_cm;
    oper_config.rotation_angle = rotation;
    oper_config.scale_x = scale_x;
    oper_config.scale_y = scale_y;
    oper_config.rgb_swap = 0;
    oper_config.byte_swap = 0;
    oper_config.mode = PPA_TRANS_MODE_BLOCKING;

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

    size_t in_buf_alignment = 4; // no special alignment requirement for input buffers
    if (esp_efuse_is_flash_encryption_enabled()) {
        in_buf_alignment = SOC_MEMSPI_ENCRYPTION_ALIGNMENT;
    }
    uint32_t in_bg_buf_size = ALIGN_UP(w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)in_bg_cm) / 8, in_buf_alignment);
    uint32_t in_fg_buf_size = ALIGN_UP(w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)in_fg_cm) / 8, in_buf_alignment);
    uint32_t out_buf_size = ALIGN_UP(w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)out_cm) / 8, 64);
    uint8_t *out_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(out_buf);
    uint8_t *in_bg_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, in_bg_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(in_bg_buf);
    uint8_t *in_fg_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, in_fg_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
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
    ppa_client_config_t ppa_client_config = {};
    ppa_client_config.oper_type = PPA_OPERATION_BLEND;
    ppa_client_config.max_pending_trans_num = 1;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

    ppa_blend_oper_config_t oper_config = {};
    oper_config.in_bg.buffer = in_bg_buf;
    oper_config.in_bg.pic_w = w;
    oper_config.in_bg.pic_h = h;
    oper_config.in_bg.block_w = block_w;
    oper_config.in_bg.block_h = block_h;
    oper_config.in_bg.block_offset_x = 0;
    oper_config.in_bg.block_offset_y = 0;
    oper_config.in_bg.blend_cm = in_bg_cm;
    oper_config.in_fg.buffer = in_fg_buf;
    oper_config.in_fg.pic_w = w;
    oper_config.in_fg.pic_h = h;
    oper_config.in_fg.block_w = block_w;
    oper_config.in_fg.block_h = block_h;
    oper_config.in_fg.block_offset_x = 0;
    oper_config.in_fg.block_offset_y = 0;
    oper_config.in_fg.blend_cm = in_fg_cm;
    oper_config.out.buffer = out_buf;
    oper_config.out.buffer_size = out_buf_size;
    oper_config.out.pic_w = w;
    oper_config.out.pic_h = h;
    oper_config.out.block_offset_x = 0;
    oper_config.out.block_offset_y = 0;
    oper_config.out.blend_cm = out_cm;
    oper_config.bg_ck_en = false;
    oper_config.fg_ck_en = false;
    oper_config.mode = PPA_TRANS_MODE_BLOCKING;

    ccomp_timer_start();

    TEST_ESP_OK(ppa_do_blend(ppa_client_handle, &oper_config));

    int64_t oper_time = ccomp_timer_stop();
    printf("PPA Blend - Process Time: %lld us\n", oper_time);

    // Check performance
    if (!esp_efuse_is_flash_encryption_enabled()) { // flash encryption slows down the performance, so skip the check
        uint64_t num_pixels_processed = block_w * block_h;
        uint64_t px_per_second = (num_pixels_processed - PPA_BLEND_TIME_OFFSET) * 1000 * 1000 / oper_time;
        printf("PPA Blend performance = %lld pixels/sec\n", px_per_second);
        TEST_ASSERT_GREATER_THAN(PPA_BLEND_MIN_PERFORMANCE_PX_PER_SEC, px_per_second);
    }

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

    uint32_t out_buf_size = ALIGN_UP(w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)out_cm) / 8, 64);
    uint8_t *out_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(out_buf);

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {};
    ppa_client_config.oper_type = PPA_OPERATION_FILL;
    ppa_client_config.max_pending_trans_num = 1;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

    ppa_fill_oper_config_t oper_config = {};
    oper_config.out.buffer = out_buf;
    oper_config.out.buffer_size = out_buf_size;
    oper_config.out.pic_w = w;
    oper_config.out.pic_h = h;
    oper_config.out.block_offset_x = 0;
    oper_config.out.block_offset_y = 0;
    oper_config.out.fill_cm = out_cm;
    oper_config.fill_block_w = block_w;
    oper_config.fill_block_h = block_h;
    oper_config.fill_argb_color.val = 0xFF00FFFF;
    oper_config.mode = PPA_TRANS_MODE_BLOCKING;

    ccomp_timer_start();

    TEST_ESP_OK(ppa_do_fill(ppa_client_handle, &oper_config));

    int64_t oper_time = ccomp_timer_stop();
    printf("PPA Fill - Process Time: %lld us\n", oper_time);

    // Check performance
    if (!esp_efuse_is_flash_encryption_enabled()) { // flash encryption slows down the performance, so skip the check
        uint64_t num_pixels_processed = block_w * block_h;
        uint64_t px_per_second = (num_pixels_processed - PPA_FILL_TIME_OFFSET) * 1000 * 1000 / oper_time;
        printf("PPA Blend performance = %lld pixels/sec\n", px_per_second);
        TEST_ASSERT_GREATER_THAN(PPA_FILL_MIN_PERFORMANCE_PX_PER_SEC, px_per_second);
    }

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));

    free(out_buf);
}

TEST_CASE("ppa_srm_stress_test", "[PPA]")
{
    // SRM does not work with flash encrypted if buffer is in external memory, so skip
    if (esp_efuse_is_flash_encryption_enabled()) {
        TEST_PASS_MESSAGE("Flash encryption is enabled, skip this test");
    }

    // Configurable parameters
    const uint32_t w = 200;
    const uint32_t h = 200;
    const ppa_srm_color_mode_t in_cm = PPA_SRM_COLOR_MODE_RGB565;
    const ppa_srm_color_mode_t out_cm = PPA_SRM_COLOR_MODE_RGB565;
    const ppa_srm_rotation_angle_t rotation = PPA_SRM_ROTATION_ANGLE_0;
    const float scale_x = 1.0;
    const float scale_y = 1.0;

    uint32_t in_buf_size = w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)in_cm) / 8;
    uint32_t out_buf_size = ALIGN_UP(w * h * color_hal_pixel_format_fourcc_get_bit_depth((esp_color_fourcc_t)out_cm) / 8, 64);
    uint8_t *out_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, out_buf_size, sizeof(uint8_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(out_buf);
    uint8_t *in_buf = static_cast<uint8_t *>(heap_caps_aligned_calloc(4, in_buf_size, sizeof(uint8_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA));
    TEST_ASSERT_NOT_NULL(in_buf);

    ppa_client_handle_t ppa_client_handle;
    ppa_client_config_t ppa_client_config = {};
    ppa_client_config.oper_type = PPA_OPERATION_SRM;
    ppa_client_config.max_pending_trans_num = 1;
    TEST_ESP_OK(ppa_register_client(&ppa_client_config, &ppa_client_handle));

    // Test on different sizes of the block
    int test_iterations = 50;
    while (test_iterations-- > 0) {
        uint32_t block_w_initial = esp_random() % (w - 100);
        uint32_t block_h_initial = esp_random() % (h - 100);
        block_w_initial = (block_w_initial == 0) ? 1 : block_w_initial;
        block_h_initial = (block_h_initial == 0) ? 1 : block_h_initial;
        uint32_t block_w = 0;
        uint32_t block_h = 0;
        for (int i = 0; i < 100; i++) {
            block_w = block_w_initial + i;
            block_h = block_h_initial + i;
            // printf("block_w = %ld, block_h = %ld\n", block_w, block_h);
            ppa_srm_oper_config_t oper_config = {};
            oper_config.in.buffer = in_buf;
            oper_config.in.pic_w = w;
            oper_config.in.pic_h = h;
            oper_config.in.block_w = block_w;
            oper_config.in.block_h = block_h;
            oper_config.in.block_offset_x = 0;
            oper_config.in.block_offset_y = 0;
            oper_config.in.srm_cm = in_cm;
            oper_config.out.buffer = out_buf;
            oper_config.out.buffer_size = out_buf_size;
            oper_config.out.pic_w = block_w;
            oper_config.out.pic_h = block_h;
            oper_config.out.block_offset_x = 0;
            oper_config.out.block_offset_y = 0;
            oper_config.out.srm_cm = out_cm;
            oper_config.rotation_angle = rotation;
            oper_config.scale_x = scale_x;
            oper_config.scale_y = scale_y;
            oper_config.rgb_swap = 0;
            oper_config.byte_swap = 0;
            oper_config.mode = PPA_TRANS_MODE_BLOCKING;

            TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_client_handle, &oper_config));
        }
    }

    TEST_ESP_OK(ppa_unregister_client(ppa_client_handle));

    free(in_buf);
    free(out_buf);
}
