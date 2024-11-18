/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <inttypes.h>

#include "esp_attr.h"
#include "esp_check.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include "soc/spi_mem_reg.h"
#include "esp_private/startup_internal.h"

static const char *TAG = "sleep_mmu";

typedef struct {
    uint32_t start;
    uint32_t end;
} mmu_domain_dev_regs_region_t;

typedef struct {
    mmu_domain_dev_regs_region_t *region;
    int region_num;
    uint32_t *regs_frame;
} mmu_domain_dev_sleep_frame_t;

/**
 * Internal structure which holds all requested light sleep mmu retention parameters
 */
typedef struct {
    struct {
        mmu_domain_dev_sleep_frame_t *mmu_table_frame;
    } retent;
} sleep_mmu_retention_t;

static DRAM_ATTR __attribute__((unused)) sleep_mmu_retention_t s_mmu_retention;

static void * mmu_domain_dev_sleep_frame_alloc_and_init(const mmu_domain_dev_regs_region_t *regions, const int region_num)
{
    const int region_sz = sizeof(mmu_domain_dev_regs_region_t) * region_num;
    int regs_frame_sz = 0;
    for (int num = 0; num < region_num; num++) {
        regs_frame_sz += regions[num].end - regions[num].start;
    }
    void *frame = heap_caps_malloc(sizeof(mmu_domain_dev_sleep_frame_t) + region_sz + regs_frame_sz, MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
    if (frame) {
        mmu_domain_dev_regs_region_t *region = (mmu_domain_dev_regs_region_t *)(frame + sizeof(mmu_domain_dev_sleep_frame_t));
        memcpy(region, regions, region_num * sizeof(mmu_domain_dev_regs_region_t));
        void *regs_frame = frame + sizeof(mmu_domain_dev_sleep_frame_t) + region_sz;
        memset(regs_frame, 0, regs_frame_sz);
        *(mmu_domain_dev_sleep_frame_t *)frame = (mmu_domain_dev_sleep_frame_t) {
            .region = region,
            .region_num = region_num,
            .regs_frame = (uint32_t *)regs_frame
        };
    }
    return frame;
}

static inline void * mmu_domain_mmu_table_sleep_frame_alloc_and_init(void)
{
    #define MMU_TABLE_SIZE (512 * 4)
    const static mmu_domain_dev_regs_region_t regions[] = {
        { .start = SPI_MEM_MMU_ITEM_CONTENT_REG(0), .end = SPI_MEM_MMU_ITEM_CONTENT_REG(0) + MMU_TABLE_SIZE}
    };
    return mmu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

static IRAM_ATTR void mmu_domain_dev_regs_save(mmu_domain_dev_sleep_frame_t *frame)
{
    assert(frame);
    mmu_domain_dev_regs_region_t *region = frame->region;
    uint32_t *regs_frame = frame->regs_frame;

    int offset = 0;
    for (int i = 0; i < frame->region_num; i++) {
        for (uint32_t addr = region[i].start; addr < region[i].end; addr+=4) {
            REG_WRITE(SPI_MEM_MMU_ITEM_INDEX_REG(0), offset);
            regs_frame[offset++] = REG_READ(SPI_MEM_MMU_ITEM_CONTENT_REG(0));
        }
    }
}

static IRAM_ATTR void mmu_domain_dev_regs_restore(mmu_domain_dev_sleep_frame_t *frame)
{
    assert(frame);
    mmu_domain_dev_regs_region_t *region = frame->region;
    uint32_t *regs_frame = frame->regs_frame;

    int offset = 0;
    for (int i = 0; i < frame->region_num; i++) {
        for (uint32_t addr = region[i].start; addr < region[i].end; addr+=4) {
            REG_WRITE(SPI_MEM_MMU_ITEM_INDEX_REG(0), offset);
            REG_WRITE(SPI_MEM_MMU_ITEM_CONTENT_REG(0),regs_frame[offset++]);
        }
    }
}

IRAM_ATTR void esp_sleep_mmu_retention(bool backup_or_restore)
{
    if (backup_or_restore) {
        mmu_domain_dev_regs_save(s_mmu_retention.retent.mmu_table_frame);
    } else {
        mmu_domain_dev_regs_restore(s_mmu_retention.retent.mmu_table_frame);
    }
}

static esp_err_t esp_sleep_mmu_retention_deinit(void)
{
    if (s_mmu_retention.retent.mmu_table_frame) {
        heap_caps_free((void *)s_mmu_retention.retent.mmu_table_frame);
        s_mmu_retention.retent.mmu_table_frame = NULL;
    }
    return ESP_OK;
}

static esp_err_t esp_sleep_mmu_retention_init(void)
{
    if (s_mmu_retention.retent.mmu_table_frame == NULL) {
        void *frame = mmu_domain_mmu_table_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        s_mmu_retention.retent.mmu_table_frame = (mmu_domain_dev_sleep_frame_t *)frame;
    }
    return ESP_OK;
err:
    esp_sleep_mmu_retention_deinit();
    return ESP_ERR_NO_MEM;
}

bool mmu_domain_pd_allowed(void)
{
    return (s_mmu_retention.retent.mmu_table_frame != NULL);
}

ESP_SYSTEM_INIT_FN(sleep_mmu_startup_init, SECONDARY, BIT(0), 108)
{
    esp_err_t ret;
    ret = esp_sleep_mmu_retention_init();
    if (ret != ESP_OK) {
        ESP_EARLY_LOGW(TAG, "Failed to enable TOP power down during light sleep.");
    }
    return ESP_OK;
}
