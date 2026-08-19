/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_littlefs.h"
#include "esp_log.h"
#include "mbedtls/base64.h"
#include "driver/ppa.h"
#include "freetype/freetype.h"

#define EXAMPLE_IMAGE_WIDTH        320
#define EXAMPLE_IMAGE_HEIGHT       240
#define EXAMPLE_RGB565_PIXEL_SIZE  2
#define EXAMPLE_IMAGE_SIZE         (EXAMPLE_IMAGE_WIDTH * EXAMPLE_IMAGE_HEIGHT * EXAMPLE_RGB565_PIXEL_SIZE)
#define EXAMPLE_BASE64_CHUNK_LEN   96

#define EXAMPLE_FONT_PATH          "/fonts/fa-regular-400.otf"
#define EXAMPLE_FONT_PIXEL_SIZE    40

/* The final composited image shows Font Awesome icons arranged in two rows of
 * three, so six icons in total. Each row is rendered on its own baseline row so
 * they stack vertically. The code points below are the Font Awesome Free
 * regular glyphs (fa-regular-400): star, heart, sun / face-smile, bell, envelope. */
#define EXAMPLE_ICONS_PER_ROW      3
#define EXAMPLE_ICON_ROWS          2
#define EXAMPLE_ICON_ROW1          0xF005, 0xF004, 0xF185   /* star, heart, sun */
#define EXAMPLE_ICON_ROW2          0xF118, 0xF0F3, 0xF0E0   /* face-smile, bell, envelope */
#define EXAMPLE_ROW1_BASELINE_Y    120
#define EXAMPLE_ROW2_BASELINE_Y    180
#define EXAMPLE_FG_R               0xff
#define EXAMPLE_FG_G               0xff
#define EXAMPLE_FG_B               0xff

/* The Font Awesome icons to display, one row per line. Each row holds three
 * icons and is rendered on its own baseline so the two rows stack vertically. */
static const uint32_t example_icon_rows[EXAMPLE_ICON_ROWS][EXAMPLE_ICONS_PER_ROW] = {
    { EXAMPLE_ICON_ROW1 },
    { EXAMPLE_ICON_ROW2 },
};
static const int example_icon_baseline_y[EXAMPLE_ICON_ROWS] = {
    EXAMPLE_ROW1_BASELINE_Y,
    EXAMPLE_ROW2_BASELINE_Y,
};

static const char *TAG = "example";

static void print_base64_payload(const unsigned char *encoded, size_t encoded_len)
{
    printf("IMAGE_BASE64_BEGIN\n");
    size_t chunk_count = 0;
    for (size_t offset = 0; offset < encoded_len; offset += EXAMPLE_BASE64_CHUNK_LEN) {
        size_t chunk_len = encoded_len - offset;
        if (chunk_len > EXAMPLE_BASE64_CHUNK_LEN) {
            chunk_len = EXAMPLE_BASE64_CHUNK_LEN;
        }
        printf("IMAGE_BASE64 %.*s\n", (int)chunk_len, (const char *)&encoded[offset]);
        chunk_count++;
        if ((chunk_count % 16) == 0) {
            /* The complete payload is large. Yield periodically so the serial
             * monitor used by pytest can consume every line reliably. */
            vTaskDelay(1);
        }
    }
    printf("IMAGE_BASE64_END\n");
}

static void encode_and_print_image(const char *effect_name, const char *pixel_format,
                                   const uint8_t *image, size_t image_size)
{
    /* Binary image data is not safe to print directly on the serial console.
     * Base64 turns it into ASCII that pytest can capture and decode. */
    size_t encoded_len = 0;
    int ret = mbedtls_base64_encode(NULL, 0, &encoded_len, image, image_size);
    ESP_ERROR_CHECK((ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) ? ESP_OK : ESP_FAIL);

    unsigned char *encoded = calloc(encoded_len + 1, 1);
    ESP_ERROR_CHECK(encoded ? ESP_OK : ESP_ERR_NO_MEM);
    ESP_ERROR_CHECK(mbedtls_base64_encode(encoded, encoded_len + 1, &encoded_len, image, image_size) == 0 ? ESP_OK : ESP_FAIL);

    printf("IMAGE_META effect=%s width=%u height=%u format=%s encoding=base64\n",
           effect_name, EXAMPLE_IMAGE_WIDTH, EXAMPLE_IMAGE_HEIGHT, pixel_format);
    print_base64_payload(encoded, encoded_len);
    free(encoded);
}

/* Report one rendered icon row to the console so the serial log and pytest can
 * verify the exact set of Font Awesome icons being drawn. */
static void report_icon_row(const uint32_t *icons, int count, int baseline_y)
{
    printf("Rendering icon row at baseline Y=%d:", baseline_y);
    for (int i = 0; i < count; i++) {
        printf(" U+%04X", (unsigned int)icons[i]);
    }
    printf("\n");
}

/* Rasterize a single Font Awesome glyph and composite it over the output frame
 * with one PPA blend transaction. Instead of building a full-frame A8 mask in
 * software, this helper lets PPA blend one glyph at a time into its own target
 * region (the glyph's bounding box), so no large intermediate mask buffer is
 * needed and the example shows a multi-blend loop. */
static void compose_glyph(ppa_client_handle_t ppa_blend_handle, FT_Face face,
                          uint32_t icon, int pen_x, int baseline_y,
                          void *image_buf)
{
    ESP_ERROR_CHECK(FT_Load_Char(face, icon, FT_LOAD_RENDER) == 0 ? ESP_OK : ESP_FAIL);

    FT_GlyphSlot slot = face->glyph;
    uint32_t w = slot->bitmap.width;
    uint32_t h = slot->bitmap.rows;
    /* bitmap_left uses the same right-positive X axis as the framebuffer. */
    int dst_x = pen_x + slot->bitmap_left;
    /* bitmap_top is measured upward from the baseline, while framebuffer Y
     * increases downward, hence the subtraction. */
    int dst_y = baseline_y - slot->bitmap_top;

    /* A glyph may produce an empty bitmap (e.g. a space); nothing to blend. */
    if (w == 0 || h == 0 || dst_x < 0 || dst_y < 0 ||
            dst_x + (int)w > EXAMPLE_IMAGE_WIDTH || dst_y + (int)h > EXAMPLE_IMAGE_HEIGHT) {
        return;
    }

    /* PPA accepts any addressable input buffer, including FreeType's internal
     * SRAM bitmap. Its pitch is the row stride in bytes; use it as pic_w so
     * PPA advances to the correct row when FreeType adds row padding. Negative
     * pitch denotes a bottom-up bitmap, which PPA cannot consume directly. */
    if (slot->bitmap.pitch <= 0 || (uint32_t)slot->bitmap.pitch < w) {
        ESP_LOGE(TAG, "Unsupported glyph bitmap pitch %d for U+%04X",
                 slot->bitmap.pitch, (unsigned int)icon);
        abort();
    }
    uint32_t glyph_pitch = (uint32_t)slot->bitmap.pitch;

    /* Blend the single-glyph A8 mask over the current image, writing the result
     * back in place into the glyph's own bounding box. The target region
     * (block) is moved for every glyph by changing block_offset_x/y. */
    ppa_blend_oper_config_t blend_config = {
        .in_bg = {
            .buffer = image_buf,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_w = w,
            .block_h = h,
            .block_offset_x = dst_x,
            .block_offset_y = dst_y,
            .blend_cm = PPA_BLEND_COLOR_MODE_RGB565,
        },
        .in_fg = {
            .buffer = slot->bitmap.buffer,
            .pic_w = glyph_pitch,
            .pic_h = h,
            .block_w = w,
            .block_h = h,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .blend_cm = PPA_BLEND_COLOR_MODE_A8,
        },
        .out = {
            .buffer = image_buf,
            .buffer_size = EXAMPLE_IMAGE_SIZE,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = dst_x,
            .block_offset_y = dst_y,
            .blend_cm = PPA_BLEND_COLOR_MODE_RGB565,
        },
        .bg_alpha_update_mode = PPA_ALPHA_NO_CHANGE,
        .fg_alpha_update_mode = PPA_ALPHA_NO_CHANGE,
        .fg_fix_rgb_val = {
            .r = EXAMPLE_FG_R,
            .g = EXAMPLE_FG_G,
            .b = EXAMPLE_FG_B,
        },
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    ESP_ERROR_CHECK(ppa_do_blend(ppa_blend_handle, &blend_config));
}

/* Render one row of Font Awesome icons. The icons are spaced evenly (using each
 * glyph's advance) and the whole row is centered horizontally on the given
 * baseline row. Each icon is composited by a separate ppa_do_blend call whose
 * target region is updated to the corresponding glyph's bounding box. */
static void render_icon_row(ppa_client_handle_t ppa_blend_handle, FT_Face face,
                            const uint32_t *icons, int count, int baseline_y,
                            void *image_buf)
{
    ESP_ERROR_CHECK(FT_Set_Pixel_Sizes(face, 0, EXAMPLE_FONT_PIXEL_SIZE) == 0 ? ESP_OK : ESP_FAIL);

    /* FreeType stores glyph advances in 26.6 fixed-point format (pixels × 64).
     * Shift right by 6 to convert them to whole-pixel pen positions. */
    /* First pass: compute the total advance of the row so it can be centered. */
    int total_advance = 0;
    for (int i = 0; i < count; i++) {
        ESP_ERROR_CHECK(FT_Load_Char(face, icons[i], FT_LOAD_DEFAULT) == 0 ? ESP_OK : ESP_FAIL);
        total_advance += face->glyph->advance.x >> 6;
    }
    int pen_x = (EXAMPLE_IMAGE_WIDTH - total_advance) / 2;

    /* Second pass: blend each icon of the row into its own target region. */
    for (int i = 0; i < count; i++) {
        compose_glyph(ppa_blend_handle, face, icons[i], pen_x, baseline_y, image_buf);
        pen_x += face->glyph->advance.x >> 6;
    }
}

static void prepare_gradient_background(uint8_t *bg_buf)
{
    /* Generate a vertical RGB565 gradient in software as the background. This
     * demonstrates that PPA can composite a rendered glyph buffer over any
     * user-produced picture. The top color is brighter and the bottom darker. */
    const uint32_t top_rgb[3] = { 40, 80, 180 };
    const uint32_t bot_rgb[3] = { 8, 16, 60 };

    for (int y = 0; y < EXAMPLE_IMAGE_HEIGHT; y++) {
        uint32_t t = (uint32_t)y * 255 / (EXAMPLE_IMAGE_HEIGHT - 1);
        uint32_t r = (top_rgb[0] * (255 - t) + bot_rgb[0] * t) / 255;
        uint32_t g = (top_rgb[1] * (255 - t) + bot_rgb[1] * t) / 255;
        uint32_t b = (top_rgb[2] * (255 - t) + bot_rgb[2] * t) / 255;
        uint16_t v = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
        for (int x = 0; x < EXAMPLE_IMAGE_WIDTH; x++) {
            bg_buf[(y * EXAMPLE_IMAGE_WIDTH + x) * 2] = v & 0xff;
            bg_buf[(y * EXAMPLE_IMAGE_WIDTH + x) * 2 + 1] = (v >> 8) & 0xff;
        }
    }
}

void app_main(void)
{
    /* Mount the LittleFS partition that stores the font. */
    esp_vfs_littlefs_conf_t littlefs_conf = {
        .base_path = "/fonts",
        .partition_label = "fonts",
        .format_if_mount_failed = true,
    };
    ESP_ERROR_CHECK(esp_vfs_littlefs_register(&littlefs_conf));
    printf("LittleFS mounted\n");

    /* Initialize FreeType library and load the font. */
    FT_Library library = NULL;
    FT_Face face = NULL;
    ESP_ERROR_CHECK(FT_Init_FreeType(&library) == 0 ? ESP_OK : ESP_FAIL);
    printf("FreeType library initialized\n");
    ESP_ERROR_CHECK(FT_New_Face(library, EXAMPLE_FONT_PATH, 0, &face) == 0 ? ESP_OK : ESP_FAIL);
    printf("Font loaded\n");

    /* Create a software gradient picture, then update it in place for every
     * glyph. This preserves both the background outside glyph blocks and any
     * previously composited glyphs that overlap a later glyph block. */
    uint8_t *result_buf = heap_caps_aligned_calloc(64, 1, EXAMPLE_IMAGE_SIZE,
                                                   MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    ESP_ERROR_CHECK(result_buf ? ESP_OK : ESP_ERR_NO_MEM);
    prepare_gradient_background(result_buf);

    ppa_client_handle_t ppa_blend_handle = NULL;
    ppa_client_config_t ppa_blend_config = {
        .oper_type = PPA_OPERATION_BLEND,
        .max_pending_trans_num = 1,
    };
    ESP_ERROR_CHECK(ppa_register_client(&ppa_blend_config, &ppa_blend_handle));

    printf("Compositing text with PPA blend...\n");
    /* Composite each Font Awesome icon with its own PPA blend transaction. Every
     * glyph is rasterized into a small A8 mask and blended over the background
     * at its own target region, so no full-frame A8 mask is required. */
    for (int row = 0; row < EXAMPLE_ICON_ROWS; row++) {
        report_icon_row(example_icon_rows[row], EXAMPLE_ICONS_PER_ROW, example_icon_baseline_y[row]);
        render_icon_row(ppa_blend_handle, face, example_icon_rows[row], EXAMPLE_ICONS_PER_ROW,
                        example_icon_baseline_y[row], result_buf);
    }

    encode_and_print_image("text_overlay", "RGB565", result_buf, EXAMPLE_IMAGE_SIZE);
    printf("PPA FreeType demo done.\n");

    ESP_ERROR_CHECK(ppa_unregister_client(ppa_blend_handle));
    free(result_buf);
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    ESP_ERROR_CHECK(esp_vfs_littlefs_unregister("fonts"));
}
