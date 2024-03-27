/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_private/dma2d.h"
#include "dma2d_test_utils.h"
#include "hal/dma2d_types.h"
#include "hal/color_types.h"
#include "esp_heap_caps.h"
#include "esp_cache.h"

// All test will perform `M2M_TRANS_TIMES` times memcpy transactions, utilizing all available 2D-DMA channels.
// This tests the hardware capability of multiple 2D-DMA transactions running together, and the driver capbility of
// transactions being send to a queue, and waiting for free channels becoming available, and being picked to start the
// real hardware operation.
#define M2M_TRANS_TIMES (8)

// Descriptor and buffer address and size should aligned to 64 bytes (the cacheline size alignment restriction) to be used by CPU

static dma2d_descriptor_t *tx_dsc[M2M_TRANS_TIMES];
static dma2d_descriptor_t *rx_dsc[M2M_TRANS_TIMES];

static dma2d_m2m_trans_config_t m2m_trans_config[M2M_TRANS_TIMES];

static void dma2d_link_dscr_init(uint32_t *head, uint32_t *next, void *buf_ptr,
                                 uint32_t ha, uint32_t va, uint32_t hb, uint32_t vb,
                                 uint32_t eof, uint32_t en_2d, uint32_t pbyte, uint32_t mod,
                                 uint32_t bias_x, uint32_t bias_y)
{
    dma2d_descriptor_t *dma2d = (dma2d_descriptor_t *)head;
    memset(dma2d, 0, sizeof(dma2d_descriptor_t));
    dma2d->owner = DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA;
    dma2d->suc_eof = eof;
    dma2d->dma2d_en = en_2d;
    dma2d->err_eof = 0;
    dma2d->hb_length = hb;
    dma2d->vb_size = vb;
    dma2d->pbyte = pbyte;
    dma2d->ha_length = ha;
    dma2d->va_size = va;
    dma2d->mode = mod;
    dma2d->y = bias_y;
    dma2d->x = bias_x;
    dma2d->buffer = buf_ptr;
    dma2d->next = (dma2d_descriptor_t *)next;
}

static bool IRAM_ATTR dma2d_m2m_suc_eof_event_cb(void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    SemaphoreHandle_t sem = (SemaphoreHandle_t)user_data;
    xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
    return (xHigherPriorityTaskWoken == pdTRUE);
}

TEST_CASE("DMA2D_M2M_1D_basic", "[DMA2D]")
{
    // Test a 16KB data block pure memcopy
    const uint32_t data_size = 16 * 1024; // unit: byte

    memset(m2m_trans_config, 0, M2M_TRANS_TIMES * sizeof(dma2d_m2m_trans_config_t));
    TEST_ESP_OK(dma2d_m2m_init());

    dma2d_descriptor_t *tx_link_buffer = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, M2M_TRANS_TIMES, 64, MALLOC_CAP_DEFAULT);
    dma2d_descriptor_t *rx_link_buffer = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, M2M_TRANS_TIMES, 64, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(tx_link_buffer);
    TEST_ASSERT_NOT_NULL(rx_link_buffer);
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        tx_dsc[i] = (dma2d_descriptor_t *)((uint32_t)tx_link_buffer + 64 * i);
        rx_dsc[i] = (dma2d_descriptor_t *)((uint32_t)rx_link_buffer + 64 * i);
    }

    uint8_t *prtx;
    uint8_t *prrx;
    uint8_t *tx_buf = heap_caps_aligned_calloc(64, data_size * M2M_TRANS_TIMES, sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    uint8_t *rx_buf = heap_caps_aligned_calloc(64, data_size * M2M_TRANS_TIMES, sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(tx_buf);
    TEST_ASSERT_NOT_NULL(rx_buf);

    dma2d_transfer_ability_t transfer_ability_config = {
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_64,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };

    SemaphoreHandle_t counting_sem = xSemaphoreCreateCounting(M2M_TRANS_TIMES, 0);

    // Preparation
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        // Buffer data preparation
        prtx = tx_buf + i * data_size;
        prrx = rx_buf + i * data_size;
        for (int idx = 0; idx < data_size; idx++) {
            prtx[idx] = (i + idx + 0x45) & 0xFF;
            prrx[idx] = 0;
        }
        // Writeback TX and RX buffers
        esp_cache_msync((void *)prtx, data_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        esp_cache_msync((void *)prrx, data_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        // DMA description preparation
        dma2d_link_dscr_init((uint32_t *)tx_dsc[i], NULL, (void *)prtx,
                             data_size >> 14, data_size >> 14,
                             data_size & 0x3FFF, data_size & 0x3FFF,
                             1, 0, DMA2D_DESCRIPTOR_PBYTE_1B0_PER_PIXEL,
                             DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, 0, 0);
        dma2d_link_dscr_init((uint32_t *)rx_dsc[i], NULL, (void *)prrx,
                             0, data_size >> 14,
                             0, data_size & 0x3FFF,
                             0, 0, DMA2D_DESCRIPTOR_PBYTE_1B0_PER_PIXEL,
                             DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, 0, 0);
        // Writeback the DMA descriptors
        esp_cache_msync((void *)tx_dsc[i], 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        esp_cache_msync((void *)rx_dsc[i], 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        // Construct dma2d_m2m_trans_config_t structure
        m2m_trans_config[i].tx_desc_base_addr = (intptr_t)tx_dsc[i];
        m2m_trans_config[i].rx_desc_base_addr = (intptr_t)rx_dsc[i];
        m2m_trans_config[i].trans_eof_cb = dma2d_m2m_suc_eof_event_cb;
        m2m_trans_config[i].user_data = (void *)counting_sem;
        m2m_trans_config[i].transfer_ability_config = &transfer_ability_config;
    }

    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        printf("trans %d\n", i);
        TEST_ESP_OK(dma2d_m2m(&m2m_trans_config[i]));
    }

    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        xSemaphoreTake(counting_sem, portMAX_DELAY);
        printf("trans %d done\n", i);
    }
    printf("All transactions done!\n");

    // Check result
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        prtx = tx_buf + i * data_size;
        prrx = rx_buf + i * data_size;

        // Invalidate TX and RX buffers
        esp_cache_msync((void *)prtx, data_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        esp_cache_msync((void *)prrx, data_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

        for (int idx = 0; idx < data_size; idx++) {
            TEST_ASSERT_EQUAL(prtx[idx], prrx[idx]);
            TEST_ASSERT_EQUAL(prtx[idx], (i + idx + 0x45) & 0xFF);
        }
    }

    free(tx_link_buffer);
    free(rx_link_buffer);
    free(tx_buf);
    free(rx_buf);
    vSemaphoreDelete(counting_sem);

    TEST_ESP_OK(dma2d_m2m_deinit());
}

static void rgb565_to_rgb888(uint16_t rgb565, void *__r, void *__g, void *__b)
{
    uint8_t *r = (uint8_t *)__r;
    uint8_t *g = (uint8_t *)__g;
    uint8_t *b = (uint8_t *)__b;

    uint32_t _rgb565 = rgb565;
    uint8_t _b = (_rgb565>>8) & 0xF8;
    uint8_t _g = (_rgb565>>3) & 0xFC;
    uint8_t _r = (_rgb565<<3) & 0xF8;
    // *r = (_r & 0x08) ? (_r | 0x1) : (_r);
    // *g = (_g & 0x04) ? (_g | 0x1) : (_g);
    // *b = (_b & 0x08) ? (_b | 0x1) : (_b);

    *r = _r | ( (_r >>3) & 0x7);
    *g = _g | ( (_g >>2) & 0x3);
    *b = _b | ( (_b >>3) & 0x7);
}

static int rgb565_to_rgb888_and_cmp(void *_rgb565, void *__rgb888, int pix)
{
    uint16_t *rgb565 = (uint16_t *)_rgb565;
    uint8_t *_rgb888 = (uint8_t *)__rgb888;
    uint8_t _r,_g,_b;
    for (int i = 0; i < pix; i++) {
        rgb565_to_rgb888(rgb565[i], &_r, &_g, &_b);
        if (_r != _rgb888[0] || _g !=  _rgb888[1] || _b !=  _rgb888[2]) {
            printf("idx %d - conv fail, %x:%x:%x, rgb565:%x, _rgb888:%x:%x:%x\r\n",
                   i, _r, _g, _b, rgb565[i], _rgb888[0], _rgb888[1] ,_rgb888[2]);
            return -1;
        }
        _rgb888 += 3;
    }
    return 0;
}

TEST_CASE("DMA2D_M2M_1D_RGB565_to_RGB888", "[DMA2D]")
{
    // Test a 4K pixel 1D buffer (original pixel in RGB565 format, convert to RGB888 format)
    const uint32_t item_size = 1024 * 4;

    memset(m2m_trans_config, 0, M2M_TRANS_TIMES * sizeof(dma2d_m2m_trans_config_t));
    TEST_ESP_OK(dma2d_m2m_init());

    dma2d_descriptor_t *tx_link_buffer = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, M2M_TRANS_TIMES, 64, MALLOC_CAP_DEFAULT);
    dma2d_descriptor_t *rx_link_buffer = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, M2M_TRANS_TIMES, 64, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(tx_link_buffer);
    TEST_ASSERT_NOT_NULL(rx_link_buffer);
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        tx_dsc[i] = (dma2d_descriptor_t *)((uint32_t)tx_link_buffer + 64 * i);
        rx_dsc[i] = (dma2d_descriptor_t *)((uint32_t)rx_link_buffer + 64 * i);
    }

    uint8_t *prtx;
    uint8_t *prrx;
    uint8_t *tx_buf = heap_caps_aligned_calloc(64, item_size * 2 * M2M_TRANS_TIMES, sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    uint8_t *rx_buf = heap_caps_aligned_calloc(64, item_size * 3 * M2M_TRANS_TIMES, sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(tx_buf);
    TEST_ASSERT_NOT_NULL(rx_buf);

    SemaphoreHandle_t counting_sem = xSemaphoreCreateCounting(M2M_TRANS_TIMES, 0);

    dma2d_transfer_ability_t transfer_ability_config = {
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_128,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };

    dma2d_csc_config_t m2m_dma2d_tx_csc = {
        .tx_csc_option = DMA2D_CSC_TX_RGB565_TO_RGB888,
        .pre_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0,
    };

    // Preparation
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        // Buffer data preparation
        prtx = tx_buf + i * item_size * 2;
        prrx = rx_buf + i * item_size * 3;
        for (int idx = 0; idx < item_size; idx++) {
            uint32_t r = (idx * 32 / item_size) & 0x1F;
            uint32_t g = (idx * 64 / item_size) & 0x3F;
            uint32_t b = (idx * 32 / item_size) & 0x1F;
            prtx[idx * 2] = (r << 3) || (g >> 3);
            prtx[idx * 2 + 1] = (g << 5) || b;
            prrx[idx * 3] = 0;
            prrx[idx * 3 + 1] = 0;
            prrx[idx * 3 + 2] = 0;
        }
        // Writeback TX and RX buffers
        esp_cache_msync((void *)prtx, item_size * 2, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        esp_cache_msync((void *)prrx, item_size * 3, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        // DMA description preparation
        dma2d_link_dscr_init((uint32_t *)tx_dsc[i], NULL, (void *)prtx,
                             (item_size * 2) >> 14, (item_size * 2) >> 14,
                             (item_size * 2) & 0x3FFF, (item_size * 2) & 0x3FFF,
                             1, 0, DMA2D_DESCRIPTOR_PBYTE_1B0_PER_PIXEL,
                             DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, 0, 0);
        dma2d_link_dscr_init((uint32_t *)rx_dsc[i], NULL, (void *)prrx,
                             0, (item_size * 3) >> 14,
                             0, (item_size * 3) & 0x3FFF,
                             0, 0, DMA2D_DESCRIPTOR_PBYTE_1B0_PER_PIXEL,
                             DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, 0, 0);
        // Writeback the DMA descriptors
        esp_cache_msync((void *)tx_dsc[i], 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        esp_cache_msync((void *)rx_dsc[i], 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        // Construct dma2d_m2m_trans_config_t structure
        m2m_trans_config[i].tx_desc_base_addr = (intptr_t)tx_dsc[i];
        m2m_trans_config[i].rx_desc_base_addr = (intptr_t)rx_dsc[i];
        m2m_trans_config[i].trans_eof_cb = dma2d_m2m_suc_eof_event_cb;
        m2m_trans_config[i].user_data = (void *)counting_sem;
        m2m_trans_config[i].transfer_ability_config = &transfer_ability_config;
        m2m_trans_config[i].tx_csc_config = &m2m_dma2d_tx_csc;
    }

    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        printf("trans %d\n", i);
        TEST_ESP_OK(dma2d_m2m(&m2m_trans_config[i]));
    }

    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        xSemaphoreTake(counting_sem, portMAX_DELAY);
        printf("trans %d done\n", i);
    }
    printf("All transactions done!\n");

    // Check result
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        prtx = tx_buf + i * item_size * 2;
        prrx = rx_buf + i * item_size * 3;

        // Invalidate TX and RX buffers
        esp_cache_msync((void *)prtx, item_size * 2, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        esp_cache_msync((void *)prrx, item_size * 3, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

        TEST_ASSERT_EQUAL(0, rgb565_to_rgb888_and_cmp(prtx, prrx, item_size));
    }

    free(tx_link_buffer);
    free(rx_link_buffer);
    free(tx_buf);
    free(rx_buf);
    vSemaphoreDelete(counting_sem);

    TEST_ESP_OK(dma2d_m2m_deinit());
}

TEST_CASE("DMA2D_M2M_2D_basic", "[DMA2D]")
{
    // Test a 128 x 128 pixel data block (one byte per pixel - assume A8)
    const color_space_pixel_format_t pixel_format = {
        .color_space = COLOR_SPACE_ALPHA,
        .pixel_format = COLOR_PIXEL_A8,
    };
    const uint32_t stripe_size = 128; // unit: bytes

    memset(m2m_trans_config, 0, M2M_TRANS_TIMES * sizeof(dma2d_m2m_trans_config_t));
    TEST_ESP_OK(dma2d_m2m_init());

    dma2d_descriptor_t *tx_link_buffer = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, M2M_TRANS_TIMES, 64, MALLOC_CAP_DEFAULT);
    dma2d_descriptor_t *rx_link_buffer = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, M2M_TRANS_TIMES, 64, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(tx_link_buffer);
    TEST_ASSERT_NOT_NULL(rx_link_buffer);
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        tx_dsc[i] = (dma2d_descriptor_t *)((uint32_t)tx_link_buffer + 64 * i);
        rx_dsc[i] = (dma2d_descriptor_t *)((uint32_t)rx_link_buffer + 64 * i);
    }

    uint8_t *prtx;
    uint8_t *prrx;
    uint8_t *tx_buf = heap_caps_aligned_calloc(64, stripe_size * stripe_size * M2M_TRANS_TIMES, sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    uint8_t *rx_buf = heap_caps_aligned_calloc(64, stripe_size * stripe_size * M2M_TRANS_TIMES, sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(tx_buf);
    TEST_ASSERT_NOT_NULL(rx_buf);

    SemaphoreHandle_t counting_sem = xSemaphoreCreateCounting(M2M_TRANS_TIMES, 0);

    dma2d_transfer_ability_t transfer_ability_config = {
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_128,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };

    // Preparation
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        // Buffer data preparation
        prtx = tx_buf + i * stripe_size * stripe_size;
        prrx = rx_buf + i * stripe_size * stripe_size;
        for (int idx = 0; idx < stripe_size * stripe_size; idx++) {
            prtx[idx] = (i + idx + 0x45) & 0xFF;
            prrx[idx] = 0;
        }
        // Writeback TX and RX buffers
        esp_cache_msync((void *)prtx, stripe_size * stripe_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        esp_cache_msync((void *)prrx, stripe_size * stripe_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        // DMA description preparation
        dma2d_link_dscr_init((uint32_t *)tx_dsc[i], NULL, (void *)prtx,
                             stripe_size, stripe_size,
                             stripe_size, stripe_size,
                             1, 1, dma2d_desc_pixel_format_to_pbyte_value(pixel_format), // 1 bytes/pixel
                             DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, 0, 0);
        dma2d_link_dscr_init((uint32_t *)rx_dsc[i], NULL, (void *)prrx,
                             stripe_size, stripe_size,
                             stripe_size, stripe_size,
                             0, 1, dma2d_desc_pixel_format_to_pbyte_value(pixel_format), // 1 bytes/pixel
                             DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, 0, 0);
        // Writeback the DMA descriptors
        esp_cache_msync((void *)tx_dsc[i], 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        esp_cache_msync((void *)rx_dsc[i], 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        // Construct dma2d_m2m_trans_config_t structure
        m2m_trans_config[i].tx_desc_base_addr = (intptr_t)tx_dsc[i];
        m2m_trans_config[i].rx_desc_base_addr = (intptr_t)rx_dsc[i];
        m2m_trans_config[i].trans_eof_cb = dma2d_m2m_suc_eof_event_cb;
        m2m_trans_config[i].user_data = (void *)counting_sem;
        m2m_trans_config[i].transfer_ability_config = &transfer_ability_config;
    }

    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        printf("trans %d\n", i);
        TEST_ESP_OK(dma2d_m2m(&m2m_trans_config[i]));
    }

    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        xSemaphoreTake(counting_sem, portMAX_DELAY);
        printf("trans %d done\n", i);
    }
    printf("All transactions done!\n");

    // Check result
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        prtx = tx_buf + i * stripe_size * stripe_size;
        prrx = rx_buf + i * stripe_size * stripe_size;

        // Invalidate TX and RX buffers
        esp_cache_msync((void *)prtx, stripe_size * stripe_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        esp_cache_msync((void *)prrx, stripe_size * stripe_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

        for (int idx = 0; idx < stripe_size * stripe_size; idx++) {
            TEST_ASSERT_EQUAL(prtx[idx], prrx[idx]);
            TEST_ASSERT_EQUAL(prtx[idx], (i + idx + 0x45) & 0xFF);
        }
    }

    free(tx_link_buffer);
    free(rx_link_buffer);
    free(tx_buf);
    free(rx_buf);
    vSemaphoreDelete(counting_sem);

    TEST_ESP_OK(dma2d_m2m_deinit());
}

static void rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b, uint16_t *rgb565)
{
    uint16_t _rgb565 = (b >> 3);
    _rgb565 = (_rgb565 << 6) | (g >>2);
    _rgb565 = (_rgb565 << 5) | (r >>3);
    *rgb565 = _rgb565;
}

static int rgb888_to_rgb565_and_cmp(void *__rgb888, void *__rgb565, int pix)
{
    uint8_t *_rgb888 = (uint8_t *)__rgb888;
    uint16_t *rgb565 = (uint16_t *)__rgb565;

    uint16_t _rgb565 = 0;
    uint8_t *rgb888 = _rgb888;
    for (int i = 0; i < pix; i++) {
        rgb888_to_rgb565(rgb888[0], rgb888[1], rgb888[2], &_rgb565);
        if (_rgb565 != rgb565[0]) {
            printf("conv fail, r:%x, g:%x, b:%x, rgb565:%x, _rgb565:%x\r\n",
                   rgb888[0], rgb888[1], rgb888[2], rgb565[0], _rgb565);
            return -1;
        }
        rgb888 += 3;
        rgb565++;
    }
    return 0;
}

TEST_CASE("DMA2D_M2M_2D_RGB888_to_RGB565", "[DMA2D]")
{
    // Test a 64 x 64 pixel data block (original pixel in RGB888 format, convert to RGB565 format)
    const color_space_pixel_format_t in_pixel_format = {
        .color_space = COLOR_SPACE_RGB,
        .pixel_format = COLOR_PIXEL_RGB888,
    };
    const color_space_pixel_format_t out_pixel_format = {
        .color_space = COLOR_SPACE_RGB,
        .pixel_format = COLOR_PIXEL_RGB565,
    };
    const uint32_t stripe_pixel_size = 64; // unit: pixel

    memset(m2m_trans_config, 0, M2M_TRANS_TIMES * sizeof(dma2d_m2m_trans_config_t));
    TEST_ESP_OK(dma2d_m2m_init());

    dma2d_descriptor_t *tx_link_buffer = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, M2M_TRANS_TIMES, 64, MALLOC_CAP_DEFAULT);
    dma2d_descriptor_t *rx_link_buffer = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, M2M_TRANS_TIMES, 64, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(tx_link_buffer);
    TEST_ASSERT_NOT_NULL(rx_link_buffer);
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        tx_dsc[i] = (dma2d_descriptor_t *)((uint32_t)tx_link_buffer + 64 * i);
        rx_dsc[i] = (dma2d_descriptor_t *)((uint32_t)rx_link_buffer + 64 * i);
    }

    uint8_t *prtx;
    uint8_t *prrx;
    uint8_t *tx_buf = heap_caps_aligned_calloc(64, stripe_pixel_size * stripe_pixel_size * 3 * M2M_TRANS_TIMES, sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    uint8_t *rx_buf = heap_caps_aligned_calloc(64, stripe_pixel_size * stripe_pixel_size * 2 * M2M_TRANS_TIMES, sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(tx_buf);
    TEST_ASSERT_NOT_NULL(rx_buf);

    SemaphoreHandle_t counting_sem = xSemaphoreCreateCounting(M2M_TRANS_TIMES, 0);

    dma2d_transfer_ability_t transfer_ability_config = {
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_16,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };

    dma2d_csc_config_t m2m_dma2d_tx_csc = {
        .tx_csc_option = DMA2D_CSC_TX_RGB888_TO_RGB565,
        .pre_scramble = DMA2D_SCRAMBLE_ORDER_BYTE2_1_0,
    };

    // Preparation
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        // Buffer data preparation
        prtx = tx_buf + i * stripe_pixel_size * stripe_pixel_size * 3;
        prrx = rx_buf + i * stripe_pixel_size * stripe_pixel_size * 2;
        for (int idx = 0; idx < stripe_pixel_size * stripe_pixel_size; idx++) {
            uint32_t r = (i + idx + 0x5A) & 0xFF;
            uint32_t g = (i + idx + 0x4C) & 0xFF;
            uint32_t b = (i + idx + 0x9E) & 0xFF;
            prtx[idx * 3] = r;
            prtx[idx * 3 + 1] = g;
            prtx[idx * 3 + 2] = b;
            prrx[idx * 2] = 0;
            prrx[idx * 2 + 1] = 0;
        }
        // Writeback TX and RX buffers
        esp_cache_msync((void *)prtx, stripe_pixel_size * stripe_pixel_size * 3, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        esp_cache_msync((void *)prrx, stripe_pixel_size * stripe_pixel_size * 2, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        // DMA description preparation
        dma2d_link_dscr_init((uint32_t *)tx_dsc[i], NULL, (void *)prtx,
                             stripe_pixel_size, stripe_pixel_size,
                             stripe_pixel_size, stripe_pixel_size,
                             1, 1, dma2d_desc_pixel_format_to_pbyte_value(in_pixel_format),
                             DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, 0, 0);
        dma2d_link_dscr_init((uint32_t *)rx_dsc[i], NULL, (void *)prrx,
                             stripe_pixel_size, stripe_pixel_size,
                             stripe_pixel_size, stripe_pixel_size,
                             0, 1, dma2d_desc_pixel_format_to_pbyte_value(out_pixel_format),
                             DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, 0, 0);
        // Writeback the DMA descriptors
        esp_cache_msync((void *)tx_dsc[i], 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        esp_cache_msync((void *)rx_dsc[i], 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        // Construct dma2d_m2m_trans_config_t structure
        m2m_trans_config[i].tx_desc_base_addr = (intptr_t)tx_dsc[i];
        m2m_trans_config[i].rx_desc_base_addr = (intptr_t)rx_dsc[i];
        m2m_trans_config[i].trans_eof_cb = dma2d_m2m_suc_eof_event_cb;
        m2m_trans_config[i].user_data = (void *)counting_sem;
        m2m_trans_config[i].transfer_ability_config = &transfer_ability_config;
        m2m_trans_config[i].tx_csc_config = &m2m_dma2d_tx_csc;
    }

    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        printf("trans %d\n", i);
        TEST_ESP_OK(dma2d_m2m(&m2m_trans_config[i]));
    }

    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        xSemaphoreTake(counting_sem, portMAX_DELAY);
        printf("trans %d done\n", i);
    }
    printf("All transactions done!\n");

    // Check result
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        prtx = tx_buf + i * stripe_pixel_size * stripe_pixel_size * 3;
        prrx = rx_buf + i * stripe_pixel_size * stripe_pixel_size * 2;

        // Invalidate TX and RX buffers
        esp_cache_msync((void *)prtx, stripe_pixel_size * stripe_pixel_size * 3, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        esp_cache_msync((void *)prrx, stripe_pixel_size * stripe_pixel_size * 2, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

        TEST_ASSERT_EQUAL(0, rgb888_to_rgb565_and_cmp(prtx, prrx, stripe_pixel_size * stripe_pixel_size));
    }

    free(tx_link_buffer);
    free(rx_link_buffer);
    free(tx_buf);
    free(rx_buf);
    vSemaphoreDelete(counting_sem);

    TEST_ESP_OK(dma2d_m2m_deinit());
}

TEST_CASE("DMA2D_M2M_2D_window", "[DMA2D]")
{
    // Test 2D memcpy to a 2 x 2 block at (2, 4) in a 8 x 8 picture (pixel in RGB565 format)
    const color_space_pixel_format_t pixel_format = {
        .color_space = COLOR_SPACE_RGB,
        .pixel_format = COLOR_PIXEL_RGB565,
    };
    const uint32_t va = 8, ha = 8; // Define picture height and width (unit: pixel)
    const uint32_t vb = 2, hb = 2; // Define block height and width (unit: pixel)
    const uint32_t x_offset = 2, y_offset = 4; // Define block location in the picture (unit: pixel)

    memset(m2m_trans_config, 0, M2M_TRANS_TIMES * sizeof(dma2d_m2m_trans_config_t));
    TEST_ESP_OK(dma2d_m2m_init());

    dma2d_descriptor_t *tx_link_buffer = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, M2M_TRANS_TIMES, 64, MALLOC_CAP_DEFAULT);
    dma2d_descriptor_t *rx_link_buffer = (dma2d_descriptor_t *)heap_caps_aligned_calloc(64, M2M_TRANS_TIMES, 64, MALLOC_CAP_DEFAULT);
    TEST_ASSERT_NOT_NULL(tx_link_buffer);
    TEST_ASSERT_NOT_NULL(rx_link_buffer);
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        tx_dsc[i] = (dma2d_descriptor_t *)((uint32_t)tx_link_buffer + 64 * i);
        rx_dsc[i] = (dma2d_descriptor_t *)((uint32_t)rx_link_buffer + 64 * i);
    }

    uint8_t *prtx;
    uint8_t *prrx;
    uint8_t *tx_buf = heap_caps_aligned_calloc(64, 64 * M2M_TRANS_TIMES, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    uint8_t *rx_buf = heap_caps_aligned_calloc(64, va * ha * 2 * M2M_TRANS_TIMES, sizeof(uint8_t), MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(tx_buf);
    TEST_ASSERT_NOT_NULL(rx_buf);

    SemaphoreHandle_t counting_sem = xSemaphoreCreateCounting(M2M_TRANS_TIMES, 0);

    dma2d_transfer_ability_t transfer_ability_config = {
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_128,
        .desc_burst_en = true,
        .mb_size = DMA2D_MACRO_BLOCK_SIZE_NONE,
    };

    // Preparation
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        // Buffer data preparation
        prtx = tx_buf + i * 64;
        prrx = rx_buf + i * va * ha * 2;
        for (int idx = 0; idx < vb * hb; idx++) {
            prtx[idx * 2] = 0x55 + idx + i;
            prtx[idx * 2 + 1] = 0xAA + idx + i;
        }
        for (int idx = 0; idx < va * ha; idx++) {
            prrx[idx * 2] = 0xFF;
            prrx[idx * 2 + 1] = 0xFF;
        }

        // Writeback TX and RX buffers
        esp_cache_msync((void *)prtx, 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        esp_cache_msync((void *)prrx, va * ha * 2, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        // DMA description preparation
        dma2d_link_dscr_init((uint32_t *)tx_dsc[i], NULL, (void *)prtx,
                             hb, vb,
                             hb, vb,
                             1, 1, dma2d_desc_pixel_format_to_pbyte_value(pixel_format),
                             DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, 0, 0);
        dma2d_link_dscr_init((uint32_t *)rx_dsc[i], NULL, (void *)prrx,
                             ha, va,
                             hb, vb,
                             0, 1, dma2d_desc_pixel_format_to_pbyte_value(pixel_format),
                             DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE, x_offset, y_offset);
        // Writeback the DMA descriptors
        esp_cache_msync((void *)tx_dsc[i], 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        esp_cache_msync((void *)rx_dsc[i], 64, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

        // Construct dma2d_m2m_trans_config_t structure
        m2m_trans_config[i].tx_desc_base_addr = (intptr_t)tx_dsc[i];
        m2m_trans_config[i].rx_desc_base_addr = (intptr_t)rx_dsc[i];
        m2m_trans_config[i].trans_eof_cb = dma2d_m2m_suc_eof_event_cb;
        m2m_trans_config[i].user_data = (void *)counting_sem;
        m2m_trans_config[i].transfer_ability_config = &transfer_ability_config;
    }

    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        printf("trans %d\n", i);
        TEST_ESP_OK(dma2d_m2m(&m2m_trans_config[i]));
    }

    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        xSemaphoreTake(counting_sem, portMAX_DELAY);
        printf("trans %d done\n", i);
    }
    printf("All transactions done!\n");

    // Print the picture and check result
    for (int i = 0; i < M2M_TRANS_TIMES; i++) {
        prtx = tx_buf + i * 64;
        prrx = rx_buf + i * va * ha * 2;

        // Invalidate TX and RX buffers
        esp_cache_msync((void *)prtx, 64, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        esp_cache_msync((void *)prrx, va * ha * 2, ESP_CACHE_MSYNC_FLAG_DIR_M2C);

        printf("pic:\n");
        for (int idx = 0; idx < va * ha; idx++) {
            printf("%02X%02X ", prrx[idx * 2], prrx[idx * 2 + 1]);
            if (idx % ha == (ha - 1)) {
                printf("\n");
            }

            bool pixel_in_window = false;
            for (int window_y = 0; window_y < vb; window_y++) {
                if (idx >= (ha * (y_offset + window_y) + x_offset) && idx < (ha * (y_offset + window_y) + x_offset + hb)) {
                    uint32_t window_x = idx - ha * (y_offset + window_y) - x_offset;
                    TEST_ASSERT_EQUAL(prtx[(window_y * hb + window_x) * 2], prrx[idx * 2]);
                    TEST_ASSERT_EQUAL(prtx[(window_y * hb + window_x) * 2 + 1], prrx[idx * 2 + 1]);
                    pixel_in_window = true;
                    break;
                }
            }
            if (!pixel_in_window) {
                TEST_ASSERT_EQUAL(0XFF, prrx[idx * 2]);
                TEST_ASSERT_EQUAL(0XFF, prrx[idx * 2 + 1]);
            }
        }
    }

    free(tx_link_buffer);
    free(rx_link_buffer);
    free(tx_buf);
    free(rx_buf);
    vSemaphoreDelete(counting_sem);

    TEST_ESP_OK(dma2d_m2m_deinit());
}
