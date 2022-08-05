/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "hal/rtc_hal.h"
#include "esp_private/sleep_retention.h"
#include "sdkconfig.h"

#ifdef CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#endif

static __attribute__((unused)) const char *TAG = "sleep";

/**
 * Internal structure which holds all requested light sleep memory retention parameters
 */
typedef struct {
    rtc_cntl_sleep_retent_t retent;
} sleep_retention_t;

static DRAM_ATTR __attribute__((unused)) sleep_retention_t s_retention;

#if SOC_PM_SUPPORT_TAGMEM_PD

#if CONFIG_PM_POWER_DOWN_TAGMEM_IN_LIGHT_SLEEP
static int cache_tagmem_retention_setup(uint32_t code_seg_vaddr, uint32_t code_seg_size, uint32_t data_seg_vaddr, uint32_t data_seg_size)
{
    int sets;   /* i/d-cache total set counts */
    int index;  /* virtual address mapping i/d-cache row offset */
    int waysgrp;
    int icache_tagmem_blk_gs, dcache_tagmem_blk_gs;
    struct cache_mode imode = { .icache = 1 };
    struct cache_mode dmode = { .icache = 0 };

    /* calculate/prepare i-cache tag memory retention parameters */
    Cache_Get_Mode(&imode);
    sets = imode.cache_size / imode.cache_ways / imode.cache_line_size;
    index = (code_seg_vaddr / imode.cache_line_size) % sets;
    waysgrp = imode.cache_ways >> 2;

    code_seg_size = ALIGNUP(imode.cache_line_size, code_seg_size);

    s_retention.retent.tagmem.icache.start_point = index;
    s_retention.retent.tagmem.icache.size = (sets * waysgrp) & 0xff;
    s_retention.retent.tagmem.icache.vld_size = s_retention.retent.tagmem.icache.size;
    if (code_seg_size < imode.cache_size / imode.cache_ways) {
        s_retention.retent.tagmem.icache.vld_size = (code_seg_size / imode.cache_line_size) * waysgrp;
    }
    s_retention.retent.tagmem.icache.enable = (code_seg_size != 0) ? 1 : 0;
    icache_tagmem_blk_gs = s_retention.retent.tagmem.icache.vld_size ? s_retention.retent.tagmem.icache.vld_size : sets * waysgrp;
    icache_tagmem_blk_gs = ALIGNUP(4, icache_tagmem_blk_gs);
    ESP_LOGD(TAG, "I-cache size:%d KiB, line size:%d B, ways:%d, sets:%d, index:%d, tag block groups:%d", (imode.cache_size>>10),
            imode.cache_line_size, imode.cache_ways, sets, index, icache_tagmem_blk_gs);

    /* calculate/prepare d-cache tag memory retention parameters */
    Cache_Get_Mode(&dmode);
    sets = dmode.cache_size / dmode.cache_ways / dmode.cache_line_size;
    index = (data_seg_vaddr / dmode.cache_line_size) % sets;
    waysgrp = dmode.cache_ways >> 2;

    data_seg_size = ALIGNUP(dmode.cache_line_size, data_seg_size);

    s_retention.retent.tagmem.dcache.start_point = index;
    s_retention.retent.tagmem.dcache.size = (sets * waysgrp) & 0x1ff;
    s_retention.retent.tagmem.dcache.vld_size = s_retention.retent.tagmem.dcache.size;
#ifndef CONFIG_ESP32S3_DATA_CACHE_16KB
    if (data_seg_size < dmode.cache_size / dmode.cache_ways) {
        s_retention.retent.tagmem.dcache.vld_size = (data_seg_size / dmode.cache_line_size) * waysgrp;
    }
    s_retention.retent.tagmem.dcache.enable = (data_seg_size != 0) ? 1 : 0;
#else
    s_retention.retent.tagmem.dcache.enable = 1;
#endif
    dcache_tagmem_blk_gs = s_retention.retent.tagmem.dcache.vld_size ? s_retention.retent.tagmem.dcache.vld_size : sets * waysgrp;
    dcache_tagmem_blk_gs = ALIGNUP(4, dcache_tagmem_blk_gs);
    ESP_LOGD(TAG, "D-cache size:%d KiB, line size:%d B, ways:%d, sets:%d, index:%d, tag block groups:%d", (dmode.cache_size>>10),
            dmode.cache_line_size, dmode.cache_ways, sets, index, dcache_tagmem_blk_gs);

    /* For I or D cache tagmem retention, backup and restore are performed through
     * RTC DMA (its bus width is 128 bits), For I/D Cache tagmem blocks (i-cache
     * tagmem blocks = 92 bits, d-cache tagmem blocks = 88 bits), RTC DMA automatically
     * aligns its bit width to 96 bits, therefore, 3 times RTC DMA can transfer 4
     * i/d-cache tagmem blocks (128 bits * 3 = 96 bits * 4) */
    return (((icache_tagmem_blk_gs + dcache_tagmem_blk_gs) << 2) * 3);
}
#endif // CONFIG_PM_POWER_DOWN_TAGMEM_IN_LIGHT_SLEEP

static esp_err_t esp_sleep_tagmem_pd_low_init(bool enable)
{
    if (enable) {
#if CONFIG_PM_POWER_DOWN_TAGMEM_IN_LIGHT_SLEEP
        if (s_retention.retent.tagmem.link_addr == NULL) {
            extern char _stext[], _etext[];
            uint32_t code_start = (uint32_t)_stext;
            uint32_t code_size = (uint32_t)(_etext - _stext);
#if !(CONFIG_SPIRAM && CONFIG_IDF_TARGET_ESP32S3)
            extern char _rodata_start[], _rodata_reserved_end[];
            uint32_t data_start = (uint32_t)_rodata_start;
            uint32_t data_size = (uint32_t)(_rodata_reserved_end - _rodata_start);
#else
            uint32_t data_start = SOC_DROM_LOW;
            uint32_t data_size = SOC_EXTRAM_DATA_SIZE;
#endif
            ESP_LOGI(TAG, "Code start at %08x, total %.2f KiB, data start at %08x, total %.2f KiB",
                    code_start, (float)code_size/1024, data_start, (float)data_size/1024);
            int tagmem_sz = cache_tagmem_retention_setup(code_start, code_size, data_start, data_size);
            void *buf = heap_caps_aligned_alloc(SOC_RTC_CNTL_TAGMEM_PD_DMA_ADDR_ALIGN,
                                                tagmem_sz + RTC_HAL_DMA_LINK_NODE_SIZE,
                                                MALLOC_CAP_RETENTION);
            if (buf) {
                memset(buf, 0, tagmem_sz + RTC_HAL_DMA_LINK_NODE_SIZE);
                s_retention.retent.tagmem.link_addr = rtc_cntl_hal_dma_link_init(buf,
                                      buf + RTC_HAL_DMA_LINK_NODE_SIZE, tagmem_sz, NULL);
            } else {
                s_retention.retent.tagmem.icache.enable = 0;
                s_retention.retent.tagmem.dcache.enable = 0;
                s_retention.retent.tagmem.link_addr = NULL;
                return ESP_ERR_NO_MEM;
            }
        }
#else // CONFIG_PM_POWER_DOWN_TAGMEM_IN_LIGHT_SLEEP
        s_retention.retent.tagmem.icache.enable = 0;
        s_retention.retent.tagmem.dcache.enable = 0;
        s_retention.retent.tagmem.link_addr = NULL;
#endif // CONFIG_PM_POWER_DOWN_TAGMEM_IN_LIGHT_SLEEP
    } else {
#if SOC_PM_SUPPORT_TAGMEM_PD
        if (s_retention.retent.tagmem.link_addr) {
            heap_caps_free(s_retention.retent.tagmem.link_addr);
            s_retention.retent.tagmem.icache.enable = 0;
            s_retention.retent.tagmem.dcache.enable = 0;
            s_retention.retent.tagmem.link_addr = NULL;
        }
#endif
    }
    return ESP_OK;
}

#endif // SOC_PM_SUPPORT_TAGMEM_PD

#if SOC_PM_SUPPORT_CPU_PD

esp_err_t esp_sleep_cpu_pd_low_init(bool enable)
{
    if (enable) {
        if (s_retention.retent.cpu_pd_mem == NULL) {
            void *buf = heap_caps_aligned_alloc(SOC_RTC_CNTL_CPU_PD_DMA_ADDR_ALIGN,
                                                SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE + RTC_HAL_DMA_LINK_NODE_SIZE,
                                                MALLOC_CAP_RETENTION);
            if (buf) {
                memset(buf, 0, SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE + RTC_HAL_DMA_LINK_NODE_SIZE);
                s_retention.retent.cpu_pd_mem = rtc_cntl_hal_dma_link_init(buf,
                                      buf + RTC_HAL_DMA_LINK_NODE_SIZE, SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE, NULL);
            } else {
                return ESP_ERR_NO_MEM;
            }
        }
    } else {
        if (s_retention.retent.cpu_pd_mem) {
            heap_caps_free(s_retention.retent.cpu_pd_mem);
            s_retention.retent.cpu_pd_mem = NULL;
        }
    }
#if SOC_PM_SUPPORT_TAGMEM_PD
    if (esp_sleep_tagmem_pd_low_init(enable) != ESP_OK) {
#ifdef CONFIG_ESP32S3_DATA_CACHE_16KB
        esp_sleep_cpu_pd_low_init(false);
        return ESP_ERR_NO_MEM;
#endif
    }
#endif
    return ESP_OK;
}

bool cpu_domain_pd_allowed(void)
{
    return (s_retention.retent.cpu_pd_mem != NULL);
}

#endif // SOC_PM_SUPPORT_CPU_PD

#if SOC_PM_SUPPORT_CPU_PD || SOC_PM_SUPPORT_TAGMEM_PD

void sleep_enable_memory_retention(void)
{
#if SOC_PM_SUPPORT_CPU_PD
    rtc_cntl_hal_enable_cpu_retention(&s_retention.retent);
#endif
#if SOC_PM_SUPPORT_TAGMEM_PD
    rtc_cntl_hal_enable_tagmem_retention(&s_retention.retent);
#endif
}

void IRAM_ATTR sleep_disable_memory_retention(void)
{
#if SOC_PM_SUPPORT_CPU_PD
    rtc_cntl_hal_disable_cpu_retention(&s_retention.retent);
#endif
#if SOC_PM_SUPPORT_TAGMEM_PD
    rtc_cntl_hal_disable_tagmem_retention(&s_retention.retent);
#endif
}

#endif // SOC_PM_SUPPORT_CPU_PD || SOC_PM_SUPPORT_TAGMEM_PD
