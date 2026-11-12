/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "driver/ppa.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mbedtls/base64.h"

#define EXAMPLE_IMAGE_WIDTH        320
#define EXAMPLE_IMAGE_HEIGHT       240
#define EXAMPLE_RGB565_PIXEL_SIZE  2
#define EXAMPLE_RGB565_IMAGE_SIZE  (EXAMPLE_IMAGE_WIDTH * EXAMPLE_IMAGE_HEIGHT * EXAMPLE_RGB565_PIXEL_SIZE)
#define EXAMPLE_L8_IMAGE_SIZE      (EXAMPLE_IMAGE_WIDTH * EXAMPLE_IMAGE_HEIGHT)

#define EXAMPLE_BASE64_CHUNK_LEN   96

#define EXAMPLE_TOP_BAR_H          26
#define EXAMPLE_CARD_X             14
#define EXAMPLE_CARD_Y             40
#define EXAMPLE_CARD_W             292
#define EXAMPLE_CARD_H             124
#define EXAMPLE_CHART_BAR_NUM      6
#define EXAMPLE_CHART_BAR_W        32
#define EXAMPLE_CHART_BAR_GAP      13
#define EXAMPLE_CHART_LEFT         30
#define EXAMPLE_CHART_BASELINE     152
#define EXAMPLE_PROGRESS_Y         180
#define EXAMPLE_PROGRESS_H         12
#define EXAMPLE_PROGRESS_FILL_W    190
#define EXAMPLE_BUTTON_Y           202
#define EXAMPLE_BUTTON_H           26
#define EXAMPLE_BUTTON_W           140

/**
 * @brief The roles that make up the user interface
 *
 * These are what the pixels of the index picture store. A pixel says which part
 * of the interface it belongs to, not what color it is, so a theme is nothing
 * more than a table that assigns one color to each role.
 */
typedef enum {
    EXAMPLE_ROLE_SCREEN,          /*!< The screen behind everything else */
    EXAMPLE_ROLE_TOP_BAR,
    EXAMPLE_ROLE_CARD,            /*!< The surface of the card, also used as the label color on top of an accent */
    EXAMPLE_ROLE_CARD_BORDER,
    EXAMPLE_ROLE_TEXT_PRIMARY,
    EXAMPLE_ROLE_TEXT_SECONDARY,
    EXAMPLE_ROLE_CHART_BAR,
    EXAMPLE_ROLE_CHART_BAR_PEAK,
    EXAMPLE_ROLE_TRACK,           /*!< The unfilled part of a progress bar, and the secondary button */
    EXAMPLE_ROLE_ACCENT,
    EXAMPLE_ROLE_MAX,
} example_role_t;

typedef struct {
    const char *name;
    struct {
        uint32_t rgb;   /*!< 0xRRGGBB */
        uint8_t alpha;  /*!< An alpha below 0xFF lets the wallpaper behind the interface show through */
    } roles[EXAMPLE_ROLE_MAX];
} example_theme_t;

static const example_theme_t s_themes[] = {
    {
        .name = "day",
        .roles = {
            [EXAMPLE_ROLE_SCREEN]         = { 0xF2F4F8, 230 },
            [EXAMPLE_ROLE_TOP_BAR]        = { 0xFFFFFF, 0xFF },
            [EXAMPLE_ROLE_CARD]           = { 0xFFFFFF, 195 },
            [EXAMPLE_ROLE_CARD_BORDER]    = { 0xD8DEE8, 0xFF },
            [EXAMPLE_ROLE_TEXT_PRIMARY]   = { 0x1B2430, 0xFF },
            [EXAMPLE_ROLE_TEXT_SECONDARY] = { 0x8A94A6, 0xFF },
            [EXAMPLE_ROLE_CHART_BAR]      = { 0xA8C4F0, 0xFF },
            [EXAMPLE_ROLE_CHART_BAR_PEAK] = { 0x2F6BE0, 0xFF },
            [EXAMPLE_ROLE_TRACK]          = { 0xE3E8F0, 0xFF },
            [EXAMPLE_ROLE_ACCENT]         = { 0x2F6BE0, 0xFF },
        },
    },
    {
        .name = "night",
        .roles = {
            [EXAMPLE_ROLE_SCREEN]         = { 0x101620, 230 },
            [EXAMPLE_ROLE_TOP_BAR]        = { 0x161E2A, 0xFF },
            [EXAMPLE_ROLE_CARD]           = { 0x1C2634, 195 },
            [EXAMPLE_ROLE_CARD_BORDER]    = { 0x2C3A4E, 0xFF },
            [EXAMPLE_ROLE_TEXT_PRIMARY]   = { 0xEAF0F8, 0xFF },
            [EXAMPLE_ROLE_TEXT_SECONDARY] = { 0x76839A, 0xFF },
            [EXAMPLE_ROLE_CHART_BAR]      = { 0x2A4A6E, 0xFF },
            [EXAMPLE_ROLE_CHART_BAR_PEAK] = { 0x46C8F0, 0xFF },
            [EXAMPLE_ROLE_TRACK]          = { 0x232E3E, 0xFF },
            [EXAMPLE_ROLE_ACCENT]         = { 0x46C8F0, 0xFF },
        },
    },
    {
        /* An accessibility theme drops the translucency on purpose, so that
         * nothing dilutes the contrast between the interface and its text */
        .name = "high_contrast",
        .roles = {
            [EXAMPLE_ROLE_SCREEN]         = { 0x000000, 0xFF },
            [EXAMPLE_ROLE_TOP_BAR]        = { 0x000000, 0xFF },
            [EXAMPLE_ROLE_CARD]           = { 0x000000, 0xFF },
            [EXAMPLE_ROLE_CARD_BORDER]    = { 0xFFFFFF, 0xFF },
            [EXAMPLE_ROLE_TEXT_PRIMARY]   = { 0xFFFFFF, 0xFF },
            [EXAMPLE_ROLE_TEXT_SECONDARY] = { 0xFFE000, 0xFF },
            [EXAMPLE_ROLE_CHART_BAR]      = { 0x00C0FF, 0xFF },
            [EXAMPLE_ROLE_CHART_BAR_PEAK] = { 0xFFE000, 0xFF },
            [EXAMPLE_ROLE_TRACK]          = { 0x303030, 0xFF },
            [EXAMPLE_ROLE_ACCENT]         = { 0xFFE000, 0xFF },
        },
    },
};

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
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    printf("IMAGE_BASE64_END\n");
}

static void encode_and_print_image(const char *effect_name, const uint8_t *image, size_t image_size)
{
    /* Binary image data is not safe to print directly on the serial console.
     * Base64 turns it into ASCII that pytest can capture and decode. */
    size_t encoded_len = 0;
    int ret = mbedtls_base64_encode(NULL, 0, &encoded_len, image, image_size);
    ESP_ERROR_CHECK((ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) ? ESP_OK : ESP_FAIL);

    unsigned char *encoded = calloc(encoded_len + 1, 1);
    assert(encoded);
    ESP_ERROR_CHECK(mbedtls_base64_encode(encoded, encoded_len + 1, &encoded_len, image, image_size) == 0 ? ESP_OK : ESP_FAIL);

    printf("IMAGE_META effect=%s width=%u height=%u format=RGB565 encoding=base64\n",
           effect_name, EXAMPLE_IMAGE_WIDTH, EXAMPLE_IMAGE_HEIGHT);
    print_base64_payload(encoded, encoded_len);
    free(encoded);
}

static uint8_t *alloc_ppa_buffer(size_t size)
{
    /* PPA uses DMA underneath, so input and output buffers must be DMA-capable
     * and cache-line aligned. PSRAM keeps the example friendly to boards with
     * limited internal RAM. */
    uint8_t *buffer = heap_caps_aligned_calloc(64, 1, size, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(buffer);
    return buffer;
}

static void build_theme_clut(const example_theme_t *theme, color_pixel_argb8888_data_t *clut)
{
    for (int role = 0; role < EXAMPLE_ROLE_MAX; role++) {
        clut[role].r = (uint8_t)(theme->roles[role].rgb >> 16);
        clut[role].g = (uint8_t)(theme->roles[role].rgb >> 8);
        clut[role].b = (uint8_t)(theme->roles[role].rgb);
        clut[role].a = theme->roles[role].alpha;
    }
}

static void fill_rect(uint8_t *index_buf, int x, int y, int w, int h, example_role_t role)
{
    for (int row = y; row < y + h; row++) {
        if (row < 0 || row >= EXAMPLE_IMAGE_HEIGHT) {
            continue;
        }
        for (int col = x; col < x + w; col++) {
            if (col < 0 || col >= EXAMPLE_IMAGE_WIDTH) {
                continue;
            }
            index_buf[row * EXAMPLE_IMAGE_WIDTH + col] = (uint8_t)role;
        }
    }
}

static void prepare_index_picture(uint8_t *index_buf)
{
    /* A dashboard is drawn out of plain rectangles. What matters is that every
     * pixel is written as a role and not as a color, which is what lets the
     * whole interface be restyled later by the CLUT alone. */
    static const int bar_height[EXAMPLE_CHART_BAR_NUM] = { 28, 44, 36, 58, 46, 64 };

    fill_rect(index_buf, 0, 0, EXAMPLE_IMAGE_WIDTH, EXAMPLE_IMAGE_HEIGHT, EXAMPLE_ROLE_SCREEN);

    fill_rect(index_buf, 0, 0, EXAMPLE_IMAGE_WIDTH, EXAMPLE_TOP_BAR_H, EXAMPLE_ROLE_TOP_BAR);
    fill_rect(index_buf, 14, 10, 86, 7, EXAMPLE_ROLE_TEXT_PRIMARY);   /* the screen title */
    for (int icon = 0; icon < 3; icon++) {                            /* the status icons */
        fill_rect(index_buf, 250 + icon * 16, 9, 10, 9, EXAMPLE_ROLE_TEXT_SECONDARY);
    }

    fill_rect(index_buf, EXAMPLE_CARD_X, EXAMPLE_CARD_Y, EXAMPLE_CARD_W, EXAMPLE_CARD_H, EXAMPLE_ROLE_CARD_BORDER);
    fill_rect(index_buf, EXAMPLE_CARD_X + 1, EXAMPLE_CARD_Y + 1, EXAMPLE_CARD_W - 2, EXAMPLE_CARD_H - 2, EXAMPLE_ROLE_CARD);
    fill_rect(index_buf, 30, 54, 96, 8, EXAMPLE_ROLE_TEXT_PRIMARY);   /* the card heading */
    fill_rect(index_buf, 30, 68, 64, 6, EXAMPLE_ROLE_TEXT_SECONDARY); /* the card subheading */

    int peak_bar = 0;
    for (int bar = 1; bar < EXAMPLE_CHART_BAR_NUM; bar++) {
        if (bar_height[bar] > bar_height[peak_bar]) {
            peak_bar = bar;
        }
    }
    for (int bar = 0; bar < EXAMPLE_CHART_BAR_NUM; bar++) {
        const example_role_t role = (bar == peak_bar) ? EXAMPLE_ROLE_CHART_BAR_PEAK : EXAMPLE_ROLE_CHART_BAR;
        fill_rect(index_buf, EXAMPLE_CHART_LEFT + bar * (EXAMPLE_CHART_BAR_W + EXAMPLE_CHART_BAR_GAP),
                  EXAMPLE_CHART_BASELINE - bar_height[bar], EXAMPLE_CHART_BAR_W, bar_height[bar], role);
    }

    fill_rect(index_buf, EXAMPLE_CARD_X, EXAMPLE_PROGRESS_Y, EXAMPLE_CARD_W, EXAMPLE_PROGRESS_H, EXAMPLE_ROLE_TRACK);
    fill_rect(index_buf, EXAMPLE_CARD_X, EXAMPLE_PROGRESS_Y, EXAMPLE_PROGRESS_FILL_W, EXAMPLE_PROGRESS_H, EXAMPLE_ROLE_ACCENT);

    fill_rect(index_buf, EXAMPLE_CARD_X, EXAMPLE_BUTTON_Y, EXAMPLE_BUTTON_W, EXAMPLE_BUTTON_H, EXAMPLE_ROLE_ACCENT);
    fill_rect(index_buf, EXAMPLE_CARD_X + 30, EXAMPLE_BUTTON_Y + 10, 80, 6, EXAMPLE_ROLE_CARD);
    fill_rect(index_buf, 166, EXAMPLE_BUTTON_Y, EXAMPLE_BUTTON_W, EXAMPLE_BUTTON_H, EXAMPLE_ROLE_TRACK);
    fill_rect(index_buf, 196, EXAMPLE_BUTTON_Y + 10, 80, 6, EXAMPLE_ROLE_TEXT_PRIMARY);
}

static void prepare_wallpaper(uint8_t *bg_buf)
{
    /* A colorful gradient stands in for a wallpaper. The translucent roles of a
     * theme pick it up, which is how a frosted surface is built. */
    for (int y = 0; y < EXAMPLE_IMAGE_HEIGHT; y++) {
        for (int x = 0; x < EXAMPLE_IMAGE_WIDTH; x++) {
            const int red = 0x1F - (x * 0x1F) / (EXAMPLE_IMAGE_WIDTH - 1);
            const int green = (y * 0x3F) / (EXAMPLE_IMAGE_HEIGHT - 1);
            const int blue = (x * 0x1F) / (EXAMPLE_IMAGE_WIDTH - 1);
            const uint16_t pixel = (uint16_t)((red << 11) | (green << 5) | blue);

            const size_t pixel_index = (y * EXAMPLE_IMAGE_WIDTH + x) * EXAMPLE_RGB565_PIXEL_SIZE;
            bg_buf[pixel_index] = (uint8_t)(pixel & 0xFF);
            bg_buf[pixel_index + 1] = (uint8_t)(pixel >> 8);
        }
    }
}

static void compose_screen(ppa_client_handle_t ppa_blend_handle, const void *bg_buf,
                           const void *index_buf, void *out_buf)
{
    /* The interface is the foreground. The blending engine expands every index
     * into the color that sits at that entry of the foreground CLUT before the
     * pixel reaches the blending process. */
    ppa_blend_oper_config_t blend_config = {
        .in_bg = {
            .buffer = bg_buf,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_w = EXAMPLE_IMAGE_WIDTH,
            .block_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .blend_cm = PPA_BLEND_COLOR_MODE_RGB565,
        },
        .in_fg = {
            .buffer = index_buf,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_w = EXAMPLE_IMAGE_WIDTH,
            .block_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .blend_cm = PPA_BLEND_COLOR_MODE_L8,
        },
        .out = {
            .buffer = out_buf,
            .buffer_size = EXAMPLE_RGB565_IMAGE_SIZE,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .blend_cm = PPA_BLEND_COLOR_MODE_RGB565,
        },
        .bg_alpha_update_mode = PPA_ALPHA_NO_CHANGE,
        /* Keep the alpha that the CLUT entry carries, instead of overriding it */
        .fg_alpha_update_mode = PPA_ALPHA_NO_CHANGE,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    ESP_ERROR_CHECK(ppa_do_blend(ppa_blend_handle, &blend_config));
}

void app_main(void)
{
    static color_pixel_argb8888_data_t theme_clut[EXAMPLE_ROLE_MAX];

    uint8_t *wallpaper_buf = alloc_ppa_buffer(EXAMPLE_RGB565_IMAGE_SIZE);
    uint8_t *index_buf = alloc_ppa_buffer(EXAMPLE_L8_IMAGE_SIZE);
    uint8_t *result_buf = alloc_ppa_buffer(EXAMPLE_RGB565_IMAGE_SIZE);

    printf("Generating wallpaper...\n");
    prepare_wallpaper(wallpaper_buf);
    printf("Drawing the L8 interface...\n");
    prepare_index_picture(index_buf);
    printf("Interface size: %d bytes, the same interface in RGB565 would take %d bytes\n",
           EXAMPLE_L8_IMAGE_SIZE, EXAMPLE_RGB565_IMAGE_SIZE);

    ppa_client_handle_t ppa_blend_handle = NULL;
    ppa_client_config_t ppa_blend_config = {
        .oper_type = PPA_OPERATION_BLEND,
        .max_pending_trans_num = 1,
    };
    ESP_ERROR_CHECK(ppa_register_client(&ppa_blend_config, &ppa_blend_handle));

    for (size_t i = 0; i < sizeof(s_themes) / sizeof(s_themes[0]); i++) {
        /* Only the CLUT content differs between the passes below. The interface
         * is drawn once and is never touched again, so switching the theme costs
         * a table of a few entries instead of a redraw. */
        printf("Applying the %s theme...\n", s_themes[i].name);
        build_theme_clut(&s_themes[i], theme_clut);
        ESP_ERROR_CHECK(ppa_set_color_lookup_table(PPA_CLUT_BLEND_FG, theme_clut, EXAMPLE_ROLE_MAX));
        compose_screen(ppa_blend_handle, wallpaper_buf, index_buf, result_buf);
        encode_and_print_image(s_themes[i].name, result_buf, EXAMPLE_RGB565_IMAGE_SIZE);
    }

    /* An empty table releases the CLUT memory, which is not required but saves
     * power once no picture uses the indexed color modes anymore */
    printf("Releasing the CLUT...\n");
    ESP_ERROR_CHECK(ppa_set_color_lookup_table(PPA_CLUT_BLEND_FG, NULL, 0));
    printf("PPA palette demo done.\n");

    ESP_ERROR_CHECK(ppa_unregister_client(ppa_blend_handle));
    free(wallpaper_buf);
    free(index_buf);
    free(result_buf);
}
