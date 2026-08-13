/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "driver/isp_core.h"
#include "driver/isp_dma.h"
#include "driver/isp_color.h"
#include "driver/isp_dpc.h"
#include "hal/color_types.h"

#define TEST_DPC_IMAGE_WIDTH     240
#define TEST_DPC_IMAGE_HEIGHT    240
#define TEST_DPC_IMAGE_SIZE      (TEST_DPC_IMAGE_WIDTH * TEST_DPC_IMAGE_HEIGHT)
#define TEST_DPC_RGB888_SIZE     (TEST_DPC_IMAGE_SIZE * 3)
#define TEST_DPC_BASE_PIXEL      0x80
#define TEST_DPC_DARK_PIXEL      0x00
#define TEST_DPC_DIM_PIXEL       0x30
#define TEST_DPC_LIGHT_PIXEL     0xd0
#define TEST_DPC_BRIGHT_PIXEL    0xff
#define TEST_DPC_VERIFY_WINDOW_RADIUS 2
#define TEST_DPC_VERIFY_MIN_CORRECTED_PIXELS_PERCENT 70

#define TEST_DPC_DUMP_PPM_TO_CONSOLE 0

typedef struct {
    uint32_t x;
    uint32_t y;
    uint8_t value;
} test_dpc_bad_pixel_t;

static const test_dpc_bad_pixel_t s_test_dpc_bad_pixels[] = {
    // isolated dark/bright pixels on BGGR B sites (even row, even column)
    { 24,  24, TEST_DPC_DARK_PIXEL},
    { 56,  24, TEST_DPC_BRIGHT_PIXEL},
    { 96,  32, TEST_DPC_DIM_PIXEL},
    {136,  32, TEST_DPC_LIGHT_PIXEL},

    // isolated pixels on BGGR G sites (even row, odd column)
    { 25,  72, TEST_DPC_DARK_PIXEL},
    { 57,  72, TEST_DPC_BRIGHT_PIXEL},
    { 97,  80, TEST_DPC_DIM_PIXEL},
    {137,  80, TEST_DPC_LIGHT_PIXEL},

    // isolated pixels on BGGR G sites (odd row, even column)
    { 24, 121, TEST_DPC_BRIGHT_PIXEL},
    { 64, 121, TEST_DPC_DARK_PIXEL},
    {104, 129, TEST_DPC_LIGHT_PIXEL},
    {144, 129, TEST_DPC_DIM_PIXEL},

    // isolated pixels on BGGR R sites (odd row, odd column)
    { 25, 177, TEST_DPC_DARK_PIXEL},
    { 57, 177, TEST_DPC_BRIGHT_PIXEL},
    { 97, 185, TEST_DPC_DIM_PIXEL},
    {137, 185, TEST_DPC_LIGHT_PIXEL},

    // extra isolated pixels spread across the frame
    {184,  48, TEST_DPC_DARK_PIXEL},
    {217,  49, TEST_DPC_BRIGHT_PIXEL},
    {184, 160, TEST_DPC_LIGHT_PIXEL},
    {217, 217, TEST_DPC_DIM_PIXEL},
};

static void test_isp_dpc_skip_if_unsupported(void)
{
#if CONFIG_IDF_TARGET_ESP32P4 && CONFIG_ESP32P4_SELECTS_REV_LESS_V3
    TEST_IGNORE_MESSAGE("DPC is not supported on ESP32P4 chips prior than v3.0");
#endif
}

static void test_isp_dpc_generate_bad_pixel_image(uint8_t *buf)
{
    memset(buf, TEST_DPC_BASE_PIXEL, TEST_DPC_IMAGE_SIZE);
    for (size_t i = 0; i < sizeof(s_test_dpc_bad_pixels) / sizeof(s_test_dpc_bad_pixels[0]); i++) {
        const test_dpc_bad_pixel_t *bad_pixel = &s_test_dpc_bad_pixels[i];
        buf[bad_pixel->y * TEST_DPC_IMAGE_WIDTH + bad_pixel->x] = bad_pixel->value;
    }
}

static void test_isp_dpc_generate_calibration_image(uint8_t *buf, bool white_image)
{
    memset(buf, white_image ? 0xff : 0x00, TEST_DPC_IMAGE_SIZE);
    for (size_t i = 0; i < sizeof(s_test_dpc_bad_pixels) / sizeof(s_test_dpc_bad_pixels[0]); i++) {
        const test_dpc_bad_pixel_t *bad_pixel = &s_test_dpc_bad_pixels[i];
        bool dark_pixel = bad_pixel->value < TEST_DPC_BASE_PIXEL;
        if ((white_image && dark_pixel) || (!white_image && !dark_pixel)) {
            buf[bad_pixel->y * TEST_DPC_IMAGE_WIDTH + bad_pixel->x] = white_image ? 0x00 : 0xff;
        }
    }
}

#if TEST_DPC_DUMP_PPM_TO_CONSOLE
static void test_isp_dpc_dump_bgr888_ppm_to_console(const char *name, const uint8_t *buf)
{
    printf("\nBEGIN_PPM:%s\n", name);
    printf("P3\n%d %d\n255\n", TEST_DPC_IMAGE_WIDTH, TEST_DPC_IMAGE_HEIGHT);
    for (size_t i = 0; i < TEST_DPC_IMAGE_SIZE; i++) {
        const uint8_t *bgr = &buf[i * 3];
        printf("%u %u %u%c", bgr[2], bgr[1], bgr[0], (i + 1) % TEST_DPC_IMAGE_WIDTH == 0 ? '\n' : ' ');
    }
    printf("END_PPM:%s\n\n", name);
}
#endif

static void test_isp_dpc_write_bgr888_ppm(const char *path, const uint8_t *buf)
{
    FILE *fp = fopen(path, "wb");
    if (!fp) {
#if TEST_DPC_DUMP_PPM_TO_CONSOLE
        printf("Failed to open %s for writing, dump PPM to console instead\n", path);
        test_isp_dpc_dump_bgr888_ppm_to_console(path, buf);
#else
        printf("Failed to open %s for writing, skip PPM dump\n", path);
#endif
        return;
    }

    fprintf(fp, "P6\n%d %d\n255\n", TEST_DPC_IMAGE_WIDTH, TEST_DPC_IMAGE_HEIGHT);
    for (size_t i = 0; i < TEST_DPC_IMAGE_SIZE; i++) {
        const uint8_t *bgr = &buf[i * 3];
        uint8_t rgb[3] = {bgr[2], bgr[1], bgr[0]};
        TEST_ASSERT_EQUAL_size_t(sizeof(rgb), fwrite(rgb, 1, sizeof(rgb), fp));
    }
    TEST_ASSERT_EQUAL(0, fclose(fp));
}

static void *test_isp_dpc_alloc_dma_buffer(size_t size)
{
    void *buf = heap_caps_aligned_calloc(64, 1, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(buf);
    return buf;
}

static void test_isp_dpc_configure_neutral_color(isp_proc_handle_t isp_proc)
{
    esp_isp_color_config_t color_cfg = {
        .color_contrast = { .integer = 1, .decimal = 0 },
        .color_saturation = { .integer = 1, .decimal = 0 },
        .color_hue = 0,
        .color_brightness = 0,
    };

    TEST_ESP_OK(esp_isp_color_configure(isp_proc, &color_cfg));
    TEST_ESP_OK(esp_isp_color_enable(isp_proc));
}

static void test_isp_dpc_process_frame(isp_proc_handle_t isp_proc, uint8_t *output, uint8_t *input)
{
    TEST_ESP_OK(esp_isp_dma_process_frame(isp_proc, output, input, 1000));
}

static uint32_t test_isp_dpc_local_frame_error(const uint8_t *actual, const uint8_t *reference, uint32_t center_x, uint32_t center_y)
{
    uint32_t error = 0;
    int32_t min_y = (int32_t)center_y - TEST_DPC_VERIFY_WINDOW_RADIUS;
    int32_t max_y = (int32_t)center_y + TEST_DPC_VERIFY_WINDOW_RADIUS;
    int32_t min_x = (int32_t)center_x - TEST_DPC_VERIFY_WINDOW_RADIUS;
    int32_t max_x = (int32_t)center_x + TEST_DPC_VERIFY_WINDOW_RADIUS;

    for (int32_t y = min_y; y <= max_y; y++) {
        if (y < 0 || y >= TEST_DPC_IMAGE_HEIGHT) {
            continue;
        }
        for (int32_t x = min_x; x <= max_x; x++) {
            if (x < 0 || x >= TEST_DPC_IMAGE_WIDTH) {
                continue;
            }
            size_t pixel_offset = ((uint32_t)y * TEST_DPC_IMAGE_WIDTH + (uint32_t)x) * 3;
            error += abs((int)actual[pixel_offset] - reference[pixel_offset]);
            error += abs((int)actual[pixel_offset + 1] - reference[pixel_offset + 1]);
            error += abs((int)actual[pixel_offset + 2] - reference[pixel_offset + 2]);
        }
    }
    return error;
}

static void test_isp_dpc_verify_result(const char *name, const uint8_t *reference, const uint8_t *original,
                                       const uint8_t *corrected, bool allow_unimproved_pixels)
{
    uint32_t min_improvement = 100;
    uint32_t max_improvement = 0;
    uint32_t total_improvement = 0;
    uint32_t verified_count = 0;
    uint32_t corrected_count = 0;

    for (size_t i = 0; i < sizeof(s_test_dpc_bad_pixels) / sizeof(s_test_dpc_bad_pixels[0]); i++) {
        const test_dpc_bad_pixel_t *bad_pixel = &s_test_dpc_bad_pixels[i];
        uint32_t original_delta = test_isp_dpc_local_frame_error(original, reference, bad_pixel->x, bad_pixel->y);
        uint32_t corrected_delta = test_isp_dpc_local_frame_error(corrected, reference, bad_pixel->x, bad_pixel->y);
        uint32_t improvement = 100;
        if (original_delta) {
            improvement = corrected_delta >= original_delta ? 0 : (original_delta - corrected_delta) * 100 / original_delta;
        }
        bool improved = original_delta > 0 && corrected_delta < original_delta;
        bool pass = original_delta > 0 && corrected_delta <= original_delta && (allow_unimproved_pixels || improved);

        printf("%s verify %s: pixel[%u] (%" PRIu32 ", %" PRIu32 ") raw=0x%02x optimized %" PRIu32 "%%, original_delta=%" PRIu32 ", corrected_delta=%" PRIu32 "\n",
               name, pass ? "pass" : "failed", (unsigned)i, bad_pixel->x, bad_pixel->y, bad_pixel->value,
               improvement, original_delta, corrected_delta);

        TEST_ASSERT_TRUE(pass);
        if (improved) {
            corrected_count++;
        }

        if (improvement < min_improvement) {
            min_improvement = improvement;
        }
        if (improvement > max_improvement) {
            max_improvement = improvement;
        }
        total_improvement += improvement;
        verified_count++;
    }

    uint32_t corrected_percent = verified_count ? corrected_count * 100 / verified_count : 0;
    if (allow_unimproved_pixels && corrected_percent < TEST_DPC_VERIFY_MIN_CORRECTED_PIXELS_PERCENT) {
        printf("%s verify failed: only %" PRIu32 "%% of bad pixels were corrected, minimum is %d%%\n",
               name, corrected_percent, TEST_DPC_VERIFY_MIN_CORRECTED_PIXELS_PERCENT);
        TEST_FAIL();
    }

    printf("\n%s verify pass: %u bad pixels, corrected=%" PRIu32 "%%, optimized avg=%" PRIu32 "%% min=%" PRIu32 "%% max=%" PRIu32 "%%\n\n",
           name, (unsigned)verified_count, corrected_percent, total_improvement / verified_count, min_improvement, max_improvement);
}

static void test_isp_dpc_dump_with_config(const char *name, const esp_isp_dpc_dynamic_config_t *dpc_config,
                                          bool allow_unimproved_pixels)
{
    test_isp_dpc_skip_if_unsupported();

    isp_proc_handle_t isp_proc = NULL;
    uint8_t *input = test_isp_dpc_alloc_dma_buffer(TEST_DPC_IMAGE_SIZE);
    uint8_t *reference = test_isp_dpc_alloc_dma_buffer(TEST_DPC_RGB888_SIZE);
    uint8_t *original = test_isp_dpc_alloc_dma_buffer(TEST_DPC_RGB888_SIZE);
    uint8_t *output = test_isp_dpc_alloc_dma_buffer(TEST_DPC_RGB888_SIZE);
    char original_path[64] = {};
    char corrected_path[64] = {};

    memset(input, TEST_DPC_BASE_PIXEL, TEST_DPC_IMAGE_SIZE);
    snprintf(original_path, sizeof(original_path), "dpc_%s_original.ppm", name);
    snprintf(corrected_path, sizeof(corrected_path), "dpc_%s_corrected.ppm", name);

    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 240 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_DWGDMA,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB888,
        .bayer_order = COLOR_RAW_ELEMENT_ORDER_BGGR,
        .has_line_start_packet = false,
        .has_line_end_packet = false,
        .h_res = TEST_DPC_IMAGE_WIDTH,
        .v_res = TEST_DPC_IMAGE_HEIGHT,
        .dma_burst_size = 8,
    };
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));
    TEST_ESP_OK(esp_isp_enable(isp_proc));
    test_isp_dpc_configure_neutral_color(isp_proc);

    test_isp_dpc_process_frame(isp_proc, reference, input);

    test_isp_dpc_generate_bad_pixel_image(input);
    test_isp_dpc_process_frame(isp_proc, original, input);

    test_isp_dpc_write_bgr888_ppm(original_path, original);

    TEST_ESP_OK(esp_isp_dpc_dynamic_configure(isp_proc, dpc_config));
    esp_isp_dpc_config_t common_config = {
        .flags.update_once_configured = true,
    };
    TEST_ESP_OK(esp_isp_dpc_configure(isp_proc, &common_config));
    TEST_ESP_OK(esp_isp_dpc_enable(isp_proc));

    test_isp_dpc_process_frame(isp_proc, output, input);

    test_isp_dpc_write_bgr888_ppm(corrected_path, output);
    test_isp_dpc_verify_result(name, reference, original, output, allow_unimproved_pixels);

    TEST_ESP_OK(esp_isp_dpc_disable(isp_proc));
    TEST_ESP_OK(esp_isp_color_disable(isp_proc));
    TEST_ESP_OK(esp_isp_disable(isp_proc));
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
    heap_caps_free(output);
    heap_caps_free(original);
    heap_caps_free(reference);
    heap_caps_free(input);
}

static void test_isp_dpc_dump_dynamic1(void)
{
    esp_isp_dpc_dynamic_config_t dpc_config = {
        .method = ESP_ISP_DPC_DYNAMIC_METHOD_1,
        .method_1 = {
            .high_threshold = 8,
            .low_threshold = 8,
        },
    };

    test_isp_dpc_dump_with_config("dynamic1", &dpc_config, false);
}

static void test_isp_dpc_dump_static(const char *name, bool enable_dynamic)
{
    test_isp_dpc_skip_if_unsupported();

    isp_proc_handle_t isp_proc = NULL;
    static esp_isp_dpc_calibration_ref_t black_ref;
    static esp_isp_dpc_calibration_ref_t white_ref;
    static esp_isp_dpc_calibration_ref_t merged_ref;
    uint8_t *input = test_isp_dpc_alloc_dma_buffer(TEST_DPC_IMAGE_SIZE);
    uint8_t *reference = test_isp_dpc_alloc_dma_buffer(TEST_DPC_RGB888_SIZE);
    uint8_t *original = test_isp_dpc_alloc_dma_buffer(TEST_DPC_RGB888_SIZE);
    uint8_t *output = test_isp_dpc_alloc_dma_buffer(TEST_DPC_RGB888_SIZE);

    memset(input, TEST_DPC_BASE_PIXEL, TEST_DPC_IMAGE_SIZE);

    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 240 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_DWGDMA,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB888,
        .bayer_order = COLOR_RAW_ELEMENT_ORDER_BGGR,
        .has_line_start_packet = false,
        .has_line_end_packet = false,
        .h_res = TEST_DPC_IMAGE_WIDTH,
        .v_res = TEST_DPC_IMAGE_HEIGHT,
        .dma_burst_size = 8,
    };
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));
    TEST_ESP_OK(esp_isp_enable(isp_proc));
    test_isp_dpc_configure_neutral_color(isp_proc);

    test_isp_dpc_process_frame(isp_proc, reference, input);

    test_isp_dpc_generate_bad_pixel_image(input);
    test_isp_dpc_process_frame(isp_proc, original, input);
    test_isp_dpc_write_bgr888_ppm("dpc_static_original.ppm", original);

    esp_isp_dpc_calibration_config_t white_calibration_config = {
        .threshold = 0xf0,
        .enable_output = true,
    };
    esp_isp_dpc_calibration_config_t black_calibration_config = {
        .threshold = 0x0a,
        .enable_output = true,
    };

    // Use a white frame to calibrate dark pixels.
    test_isp_dpc_generate_calibration_image(input, true);
    TEST_ESP_OK(esp_isp_dpc_static_calibration_start_once(isp_proc, ESP_ISP_DPC_CALIBRATION_IMAGE_WHITE, &white_calibration_config));
    test_isp_dpc_process_frame(isp_proc, output, input);
    TEST_ESP_OK(esp_isp_dpc_calibration_read_result(isp_proc, 1000, &white_ref));

    // Use a black frame to calibrate bright pixels.
    test_isp_dpc_generate_calibration_image(input, false);
    TEST_ESP_OK(esp_isp_dpc_static_calibration_start_once(isp_proc, ESP_ISP_DPC_CALIBRATION_IMAGE_BLACK, &black_calibration_config));
    test_isp_dpc_process_frame(isp_proc, output, input);
    TEST_ESP_OK(esp_isp_dpc_calibration_read_result(isp_proc, 1000, &black_ref));

    // Merge the calibration references and configure the resulting coordinate list.
    const esp_isp_dpc_calibration_ref_t *calibration_refs[] = {
        &black_ref,
        &white_ref,
    };
    TEST_ESP_OK(esp_isp_dpc_calibration_merge_result(calibration_refs,
                                                     sizeof(calibration_refs) / sizeof(calibration_refs[0]),
                                                     &merged_ref));
    TEST_ASSERT_GREATER_THAN(0, merged_ref.dead_pixel_count);

    esp_isp_dpc_static_config_t static_config = {
        .dead_pixel_coords = merged_ref.dead_pixel_coords,
        .dead_pixel_count = merged_ref.dead_pixel_count,
    };
    TEST_ESP_OK(esp_isp_dpc_static_configure(isp_proc, &static_config));

    if (enable_dynamic) {
        esp_isp_dpc_dynamic_config_t dynamic_config = {
            .method = ESP_ISP_DPC_DYNAMIC_METHOD_1,
            .method_1 = {
                .high_threshold = 8,
                .low_threshold = 8,
            },
        };
        TEST_ESP_OK(esp_isp_dpc_dynamic_configure(isp_proc, &dynamic_config));
    }
    esp_isp_dpc_config_t common_config = {
        .flags.update_once_configured = true,
    };
    TEST_ESP_OK(esp_isp_dpc_configure(isp_proc, &common_config));
    TEST_ESP_OK(esp_isp_dpc_enable(isp_proc));

    test_isp_dpc_generate_bad_pixel_image(input);
    test_isp_dpc_process_frame(isp_proc, output, input);
    test_isp_dpc_write_bgr888_ppm("dpc_static_corrected.ppm", output);
    test_isp_dpc_verify_result(name, reference, original, output, false);

    TEST_ESP_OK(esp_isp_dpc_disable(isp_proc));
    TEST_ESP_OK(esp_isp_color_disable(isp_proc));
    TEST_ESP_OK(esp_isp_disable(isp_proc));
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
    heap_caps_free(output);
    heap_caps_free(original);
    heap_caps_free(reference);
    heap_caps_free(input);
}

TEST_CASE("ISP DPC merges multiple calibration references without hardware", "[isp][dpc]")
{
    static const esp_isp_dpc_calibration_ref_t input_refs[] = {
        {
            .dead_pixel_coords = {
                { .x = 5, .y = 2 },
                { .x = 1, .y = 1 },
            },
            .dead_pixel_count = 2,
        },
        {
            .dead_pixel_coords = {
                { .x = 3, .y = 1 },
                { .x = 5, .y = 2 },
            },
            .dead_pixel_count = 2,
        },
        {
            .dead_pixel_coords = {
                { .x = 2, .y = 3 },
            },
            .dead_pixel_count = 1,
        },
    };
    const esp_isp_dpc_calibration_ref_t *refs[] = {
        &input_refs[0],
        &input_refs[1],
        &input_refs[2],
    };
    const esp_isp_dpc_pixel_coord_t expected[] = {
        { .x = 1, .y = 1 },
        { .x = 3, .y = 1 },
        { .x = 5, .y = 2 },
        { .x = 2, .y = 3 },
    };
    static esp_isp_dpc_calibration_ref_t merged_ref;

    TEST_ESP_OK(esp_isp_dpc_calibration_merge_result(refs,
                                                     sizeof(refs) / sizeof(refs[0]),
                                                     &merged_ref));
    TEST_ASSERT_EQUAL_UINT32(sizeof(expected) / sizeof(expected[0]), merged_ref.dead_pixel_count);
    for (size_t i = 0; i < merged_ref.dead_pixel_count; i++) {
        TEST_ASSERT_EQUAL_UINT16(expected[i].x, merged_ref.dead_pixel_coords[i].x);
        TEST_ASSERT_EQUAL_UINT16(expected[i].y, merged_ref.dead_pixel_coords[i].y);
    }
}

static void test_isp_dpc_dump_dynamic2(void)
{
    esp_isp_dpc_dynamic_config_t dpc_config = {
        .method = ESP_ISP_DPC_DYNAMIC_METHOD_2,
        .method_2 = {
            .first_stage_upper_ratio = {
                .integer = 1,
                .decimal = 0,
            },
            .first_stage_lower_ratio = {
                .integer = 0,
                .decimal = 8,
            },
            .bright_deviation_factor = {
                .integer = 0,
                .decimal = 16,
            },
            .dark_deviation_factor = {
                .integer = 0,
                .decimal = 16,
            },
        },
    };

    test_isp_dpc_dump_with_config("dynamic2", &dpc_config, true);
}

TEST_CASE("ISP DPC dynamic1 dump bad pixel image", "[isp][dpc]")
{
    test_isp_dpc_dump_dynamic1();
}

TEST_CASE("ISP DPC dynamic2 dump bad pixel image", "[isp][dpc]")
{
    test_isp_dpc_dump_dynamic2();
}

TEST_CASE("ISP DPC static dump bad pixel image", "[isp][dpc]")
{
    test_isp_dpc_dump_static("static", false);
}

TEST_CASE("ISP DPC static and dynamic dump bad pixel image", "[isp][dpc]")
{
    test_isp_dpc_dump_static("static_dynamic", true);
}
