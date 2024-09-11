/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <inttypes.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_check.h"
#include "esp_ipc_isr.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "soc/rtc_periph.h"
#include "soc/soc_caps.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/sleep_event.h"
#include "sdkconfig.h"
#include "hal/rtc_hal.h"
#include "esp32s3/rom/cache.h"

static __attribute__((unused)) const char *TAG = "sleep";

typedef struct {
    uint32_t start;
    uint32_t end;
} cpu_domain_dev_regs_region_t;

typedef struct {
    cpu_domain_dev_regs_region_t *region;
    int region_num;
    uint32_t *regs_frame;
} cpu_domain_dev_sleep_frame_t;

/**
 * Internal structure which holds all requested light sleep cpu retention parameters
 */
typedef struct {
    rtc_cntl_sleep_retent_t retent;
} sleep_cpu_retention_t;

static DRAM_ATTR  sleep_cpu_retention_t s_cpu_retention;


#if CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP
static rtc_cntl_sleep_cache_tag_retent_t *s_tag_mem = &s_cpu_retention.retent.tagmem;
static uint32_t cache_tagmem_retention_setup(uint32_t code_seg_vaddr, uint32_t code_seg_size, uint32_t data_seg_vaddr, uint32_t data_seg_size)
{
    uint32_t sets;   /* i/d-cache total set counts */
    uint32_t index;  /* virtual address mapping i/d-cache row offset */
    uint32_t waysgrp;
    uint32_t icache_tagmem_blk_gs, dcache_tagmem_blk_gs;
    struct cache_mode imode = { .icache = 1 };
    struct cache_mode dmode = { .icache = 0 };

    /* calculate/prepare i-cache tag memory retention parameters */
    Cache_Get_Mode(&imode);
    sets = imode.cache_size / imode.cache_ways / imode.cache_line_size;
    index = (code_seg_vaddr / imode.cache_line_size) % sets;
    waysgrp = imode.cache_ways >> 2;

    code_seg_size = ALIGNUP(imode.cache_line_size, code_seg_size);

    s_tag_mem->icache.start_point = index;
    s_tag_mem->icache.size = (sets * waysgrp) & 0xff;
    s_tag_mem->icache.vld_size = s_tag_mem->icache.size;
    if (code_seg_size < imode.cache_size / imode.cache_ways) {
        s_tag_mem->icache.vld_size = (code_seg_size / imode.cache_line_size) * waysgrp;
    }
    s_tag_mem->icache.enable = (code_seg_size != 0) ? 1 : 0;
    icache_tagmem_blk_gs = s_tag_mem->icache.vld_size ? s_tag_mem->icache.vld_size : sets * waysgrp;
    icache_tagmem_blk_gs = ALIGNUP(4, icache_tagmem_blk_gs);
    ESP_LOGD(TAG, "I-cache size:%"PRIu32" KiB, line size:%d B, ways:%d, sets:%"PRIu32", index:%"PRIu32", tag block groups:%"PRIu32"", (imode.cache_size>>10),
            imode.cache_line_size, imode.cache_ways, sets, index, icache_tagmem_blk_gs);

    /* calculate/prepare d-cache tag memory retention parameters */
    Cache_Get_Mode(&dmode);
    sets = dmode.cache_size / dmode.cache_ways / dmode.cache_line_size;
    index = (data_seg_vaddr / dmode.cache_line_size) % sets;
    waysgrp = dmode.cache_ways >> 2;

    data_seg_size = ALIGNUP(dmode.cache_line_size, data_seg_size);

    s_tag_mem->dcache.start_point = index;
    s_tag_mem->dcache.size = (sets * waysgrp) & 0x1ff;
    s_tag_mem->dcache.vld_size = s_tag_mem->dcache.size;
#ifndef CONFIG_ESP32S3_DATA_CACHE_16KB
    if (data_seg_size < dmode.cache_size / dmode.cache_ways) {
        s_tag_mem->dcache.vld_size = (data_seg_size / dmode.cache_line_size) * waysgrp;
    }
    s_tag_mem->dcache.enable = (data_seg_size != 0) ? 1 : 0;
#else
    s_tag_mem->dcache.enable = 1;
#endif
    dcache_tagmem_blk_gs = s_tag_mem->dcache.vld_size ? s_tag_mem->dcache.vld_size : sets * waysgrp;
    dcache_tagmem_blk_gs = ALIGNUP(4, dcache_tagmem_blk_gs);
    ESP_LOGD(TAG, "D-cache size:%"PRIu32" KiB, line size:%d B, ways:%d, sets:%"PRIu32", index:%"PRIu32", tag block groups:%"PRIu32"", (dmode.cache_size>>10),
            dmode.cache_line_size, dmode.cache_ways, sets, index, dcache_tagmem_blk_gs);

    /* For I or D cache tagmem retention, backup and restore are performed through
     * RTC DMA (its bus width is 128 bits), For I/D Cache tagmem blocks (i-cache
     * tagmem blocks = 92 bits, d-cache tagmem blocks = 88 bits), RTC DMA automatically
     * aligns its bit width to 96 bits, therefore, 3 times RTC DMA can transfer 4
     * i/d-cache tagmem blocks (128 bits * 3 = 96 bits * 4) */
    return (((icache_tagmem_blk_gs + dcache_tagmem_blk_gs) << 2) * 3);
}
#endif // CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP

static esp_err_t esp_sleep_tagmem_pd_low_init(void)
{
#if CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP
    if (s_tag_mem->link_addr == NULL) {
        extern char _stext[], _etext[];
        uint32_t code_start = (uint32_t)_stext;
        uint32_t code_size = (uint32_t)(_etext - _stext);
#if !(CONFIG_SPIRAM && CONFIG_SOC_PM_SUPPORT_TAGMEM_PD)
        extern char _rodata_start[], _rodata_reserved_end[];
        uint32_t data_start = (uint32_t)_rodata_start;
        uint32_t data_size = (uint32_t)(_rodata_reserved_end - _rodata_start);
#else
        uint32_t data_start = SOC_DROM_LOW;
        uint32_t data_size = SOC_EXTRAM_DATA_SIZE;
#endif
        ESP_LOGI(TAG, "Code start at 0x%08"PRIx32", total %"PRIu32", data start at 0x%08"PRIx32", total %"PRIu32" Bytes",
                code_start, code_size, data_start, data_size);
        uint32_t tagmem_sz = cache_tagmem_retention_setup(code_start, code_size, data_start, data_size);
        void *buf = heap_caps_aligned_calloc(SOC_RTC_CNTL_TAGMEM_PD_DMA_ADDR_ALIGN, 1,
                                            tagmem_sz + RTC_HAL_DMA_LINK_NODE_SIZE,
                                            MALLOC_CAP_RETENTION);
        if (buf) {
            s_tag_mem->link_addr = rtc_cntl_hal_dma_link_init(buf,
                                  buf + RTC_HAL_DMA_LINK_NODE_SIZE, tagmem_sz, NULL);
        } else {
            s_tag_mem->icache.enable = 0;
            s_tag_mem->dcache.enable = 0;
            s_tag_mem->link_addr = NULL;
            return ESP_ERR_NO_MEM;
        }
    }
#else // CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP
    s_tag_mem->icache.enable = 0;
    s_tag_mem->dcache.enable = 0;
    s_tag_mem->link_addr = NULL;
#endif // CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP
    return ESP_OK;
}

static esp_err_t esp_sleep_tagmem_pd_low_deinit(void)
{
    if (s_tag_mem->link_addr) {
        heap_caps_free(s_tag_mem->link_addr);
        s_tag_mem->icache.enable = 0;
        s_tag_mem->dcache.enable = 0;
        s_tag_mem->link_addr = NULL;
    }
    return ESP_OK;
}

esp_err_t esp_sleep_cpu_pd_low_init(void)
{
    if (s_cpu_retention.retent.cpu_pd_mem == NULL) {
        void *buf = heap_caps_aligned_calloc(SOC_RTC_CNTL_CPU_PD_DMA_ADDR_ALIGN, 1,
                                            SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE + RTC_HAL_DMA_LINK_NODE_SIZE,
                                            MALLOC_CAP_RETENTION);
        if (buf) {
            s_cpu_retention.retent.cpu_pd_mem = rtc_cntl_hal_dma_link_init(buf,
                                  buf + RTC_HAL_DMA_LINK_NODE_SIZE, SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE, NULL);
        } else {
            return ESP_ERR_NO_MEM;
        }
    }

    if (esp_sleep_tagmem_pd_low_init() != ESP_OK) {
#ifdef CONFIG_ESP32S3_DATA_CACHE_16KB
        esp_sleep_cpu_pd_low_deinit();
        return ESP_ERR_NO_MEM;
#endif
    }

    return ESP_OK;
}

esp_err_t esp_sleep_cpu_pd_low_deinit(void)
{
    if (s_cpu_retention.retent.cpu_pd_mem) {
        heap_caps_free(s_cpu_retention.retent.cpu_pd_mem);
        s_cpu_retention.retent.cpu_pd_mem = NULL;
    }
    if (esp_sleep_tagmem_pd_low_deinit() != ESP_OK) {
#ifdef CONFIG_ESP32S3_DATA_CACHE_16KB
        esp_sleep_cpu_pd_low_deinit();
        return ESP_ERR_NO_MEM;
#endif
    }
    return ESP_OK;
}

void sleep_enable_cpu_retention(void)
{
    rtc_cntl_hal_enable_cpu_retention(&s_cpu_retention.retent);
    rtc_cntl_hal_enable_tagmem_retention(&s_cpu_retention.retent);
}

void IRAM_ATTR sleep_disable_cpu_retention(void)
{
    rtc_cntl_hal_disable_cpu_retention(&s_cpu_retention.retent);
    rtc_cntl_hal_disable_tagmem_retention(&s_cpu_retention.retent);
}

esp_err_t esp_sleep_cpu_retention_init(void)
{
    return esp_sleep_cpu_pd_low_init();
}

esp_err_t esp_sleep_cpu_retention_deinit(void)
{
    return esp_sleep_cpu_pd_low_deinit();
}

bool cpu_domain_pd_allowed(void)
{
    return (s_cpu_retention.retent.cpu_pd_mem != NULL);
}

esp_err_t sleep_cpu_configure(bool light_sleep_enable)
{
#if ESP_SLEEP_POWER_DOWN_CPU
    if (light_sleep_enable) {
        ESP_RETURN_ON_ERROR(esp_sleep_cpu_retention_init(), TAG, "Failed to enable CPU power down during light sleep.");
    } else {
        ESP_RETURN_ON_ERROR(esp_sleep_cpu_retention_deinit(), TAG, "Failed to release CPU retention memory");
    }
#endif
    return ESP_OK;
}
