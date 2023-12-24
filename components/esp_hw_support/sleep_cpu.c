/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_crc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/sleep_event.h"
#include "sdkconfig.h"

#if SOC_PMU_SUPPORTED
#include "esp_private/esp_pmu.h"
#else
#include "hal/rtc_hal.h"
#endif

#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
#include "esp_private/system_internal.h"
#include "hal/clk_gate_ll.h"
#include "hal/uart_hal.h"
#endif

#include "soc/rtc_periph.h"

#ifdef CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/rtc.h"
#include "riscv/rvsleep-frames.h"
#include "soc/intpri_reg.h"
#include "soc/extmem_reg.h"
#include "soc/plic_reg.h"
#include "soc/clint_reg.h"
#include "esp32c6/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/rtc.h"
#include "riscv/rvsleep-frames.h"
#include "soc/intpri_reg.h"
#include "soc/extmem_reg.h"
#include "soc/plic_reg.h"
#include "soc/clint_reg.h"
#include "esp32h2/rom/cache.h"
#endif

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
#if SOC_PM_SUPPORT_CPU_PD && SOC_PM_CPU_RETENTION_BY_RTCCNTL
    rtc_cntl_sleep_retent_t retent;
#elif SOC_PM_SUPPORT_CPU_PD && SOC_PM_CPU_RETENTION_BY_SW
    struct {
        RvCoreCriticalSleepFrame *critical_frame;
        RvCoreNonCriticalSleepFrame *non_critical_frame;
        cpu_domain_dev_sleep_frame_t *intpri_frame;
        cpu_domain_dev_sleep_frame_t *cache_config_frame;
        cpu_domain_dev_sleep_frame_t *plic_frame;
        cpu_domain_dev_sleep_frame_t *clint_frame;
    } retent;
#endif
} sleep_cpu_retention_t;

static DRAM_ATTR __attribute__((unused)) sleep_cpu_retention_t s_cpu_retention;

#if SOC_PM_SUPPORT_TAGMEM_PD && SOC_PM_CPU_RETENTION_BY_RTCCNTL

#if CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP
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

    s_cpu_retention.retent.tagmem.icache.start_point = index;
    s_cpu_retention.retent.tagmem.icache.size = (sets * waysgrp) & 0xff;
    s_cpu_retention.retent.tagmem.icache.vld_size = s_cpu_retention.retent.tagmem.icache.size;
    if (code_seg_size < imode.cache_size / imode.cache_ways) {
        s_cpu_retention.retent.tagmem.icache.vld_size = (code_seg_size / imode.cache_line_size) * waysgrp;
    }
    s_cpu_retention.retent.tagmem.icache.enable = (code_seg_size != 0) ? 1 : 0;
    icache_tagmem_blk_gs = s_cpu_retention.retent.tagmem.icache.vld_size ? s_cpu_retention.retent.tagmem.icache.vld_size : sets * waysgrp;
    icache_tagmem_blk_gs = ALIGNUP(4, icache_tagmem_blk_gs);
    ESP_LOGD(TAG, "I-cache size:%"PRIu32" KiB, line size:%d B, ways:%d, sets:%"PRIu32", index:%"PRIu32", tag block groups:%"PRIu32"", (imode.cache_size>>10),
            imode.cache_line_size, imode.cache_ways, sets, index, icache_tagmem_blk_gs);

    /* calculate/prepare d-cache tag memory retention parameters */
    Cache_Get_Mode(&dmode);
    sets = dmode.cache_size / dmode.cache_ways / dmode.cache_line_size;
    index = (data_seg_vaddr / dmode.cache_line_size) % sets;
    waysgrp = dmode.cache_ways >> 2;

    data_seg_size = ALIGNUP(dmode.cache_line_size, data_seg_size);

    s_cpu_retention.retent.tagmem.dcache.start_point = index;
    s_cpu_retention.retent.tagmem.dcache.size = (sets * waysgrp) & 0x1ff;
    s_cpu_retention.retent.tagmem.dcache.vld_size = s_cpu_retention.retent.tagmem.dcache.size;
#ifndef CONFIG_ESP32S3_DATA_CACHE_16KB
    if (data_seg_size < dmode.cache_size / dmode.cache_ways) {
        s_cpu_retention.retent.tagmem.dcache.vld_size = (data_seg_size / dmode.cache_line_size) * waysgrp;
    }
    s_cpu_retention.retent.tagmem.dcache.enable = (data_seg_size != 0) ? 1 : 0;
#else
    s_cpu_retention.retent.tagmem.dcache.enable = 1;
#endif
    dcache_tagmem_blk_gs = s_cpu_retention.retent.tagmem.dcache.vld_size ? s_cpu_retention.retent.tagmem.dcache.vld_size : sets * waysgrp;
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
        if (s_cpu_retention.retent.tagmem.link_addr == NULL) {
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
                s_cpu_retention.retent.tagmem.link_addr = rtc_cntl_hal_dma_link_init(buf,
                                      buf + RTC_HAL_DMA_LINK_NODE_SIZE, tagmem_sz, NULL);
            } else {
                s_cpu_retention.retent.tagmem.icache.enable = 0;
                s_cpu_retention.retent.tagmem.dcache.enable = 0;
                s_cpu_retention.retent.tagmem.link_addr = NULL;
                return ESP_ERR_NO_MEM;
            }
        }
#else // CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP
        s_cpu_retention.retent.tagmem.icache.enable = 0;
        s_cpu_retention.retent.tagmem.dcache.enable = 0;
        s_cpu_retention.retent.tagmem.link_addr = NULL;
#endif // CONFIG_PM_RESTORE_CACHE_TAGMEM_AFTER_LIGHT_SLEEP
    return ESP_OK;
}

static esp_err_t esp_sleep_tagmem_pd_low_deinit(void)
{
#if SOC_PM_SUPPORT_TAGMEM_PD
        if (s_cpu_retention.retent.tagmem.link_addr) {
            heap_caps_free(s_cpu_retention.retent.tagmem.link_addr);
            s_cpu_retention.retent.tagmem.icache.enable = 0;
            s_cpu_retention.retent.tagmem.dcache.enable = 0;
            s_cpu_retention.retent.tagmem.link_addr = NULL;
        }
#endif
    return ESP_OK;
}
#endif // SOC_PM_SUPPORT_TAGMEM_PD && SOC_PM_CPU_RETENTION_BY_RTCCNTL

#if SOC_PM_SUPPORT_CPU_PD && SOC_PM_CPU_RETENTION_BY_RTCCNTL

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

#if SOC_PM_SUPPORT_TAGMEM_PD && SOC_PM_CPU_RETENTION_BY_RTCCNTL
    if (esp_sleep_tagmem_pd_low_init() != ESP_OK) {
#ifdef CONFIG_ESP32S3_DATA_CACHE_16KB
        esp_sleep_cpu_pd_low_deinit();
        return ESP_ERR_NO_MEM;
#endif
    }
#endif
    return ESP_OK;
}

esp_err_t esp_sleep_cpu_pd_low_deinit(void)
{
    if (s_cpu_retention.retent.cpu_pd_mem) {
        heap_caps_free(s_cpu_retention.retent.cpu_pd_mem);
        s_cpu_retention.retent.cpu_pd_mem = NULL;
    }

#if SOC_PM_SUPPORT_TAGMEM_PD && SOC_PM_CPU_RETENTION_BY_RTCCNTL
    if (esp_sleep_tagmem_pd_low_deinit() != ESP_OK) {
#ifdef CONFIG_ESP32S3_DATA_CACHE_16KB
        esp_sleep_cpu_pd_low_deinit();
        return ESP_ERR_NO_MEM;
#endif
    }
#endif
    return ESP_OK;
}

void sleep_enable_cpu_retention(void)
{
    rtc_cntl_hal_enable_cpu_retention(&s_cpu_retention.retent);

#if SOC_PM_SUPPORT_TAGMEM_PD
    rtc_cntl_hal_enable_tagmem_retention(&s_cpu_retention.retent);
#endif
}

void IRAM_ATTR sleep_disable_cpu_retention(void)
{
    rtc_cntl_hal_disable_cpu_retention(&s_cpu_retention.retent);

#if SOC_PM_SUPPORT_TAGMEM_PD
    rtc_cntl_hal_disable_tagmem_retention(&s_cpu_retention.retent);
#endif
}

#endif


#if SOC_PM_SUPPORT_CPU_PD && SOC_PM_CPU_RETENTION_BY_SW

#define CUSTOM_CSR_PCER_MACHINE        0x7e0
#define CUSTOM_CSR_PCMR_MACHINE        0x7e1
#define CUSTOM_CSR_PCCR_MACHINE        0x7e2
#define CUSTOM_CSR_CPU_TESTBUS_CTRL    0x7e3
#define CUSTOM_CSR_PCER_USER           0x800
#define CUSTOM_CSR_PCMR_USER           0x801
#define CUSTOM_CSR_PCCR_USER           0x802
#define CUSTOM_CSR_GPIO_OEN_USER       0x803
#define CUSTOM_CSR_GPIO_IN_USER        0x804
#define CUSTOM_CSR_GPIO_OUT_USER       0x805
#define CUSTOM_CSR_CO_EXCEPTION_CAUSE  0x7f0
#define CUSTOM_CSR_CO_HWLP             0x7f1
#define CUSTOM_CSR_CO_AIA              0x7f2

extern RvCoreCriticalSleepFrame *rv_core_critical_regs_frame;

static void * cpu_domain_dev_sleep_frame_alloc_and_init(const cpu_domain_dev_regs_region_t *regions, const int region_num)
{
    const int region_sz = sizeof(cpu_domain_dev_regs_region_t) * region_num;
    int regs_frame_sz = 0;
    for (int num = 0; num < region_num; num++) {
        regs_frame_sz += regions[num].end - regions[num].start;
    }
    void *frame = heap_caps_malloc(sizeof(cpu_domain_dev_sleep_frame_t) + region_sz + regs_frame_sz, MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
    if (frame) {
        cpu_domain_dev_regs_region_t *region = (cpu_domain_dev_regs_region_t *)(frame + sizeof(cpu_domain_dev_sleep_frame_t));
        memcpy(region, regions, region_num * sizeof(cpu_domain_dev_regs_region_t));
        void *regs_frame = frame + sizeof(cpu_domain_dev_sleep_frame_t) + region_sz;
        memset(regs_frame, 0, regs_frame_sz);
        *(cpu_domain_dev_sleep_frame_t *)frame = (cpu_domain_dev_sleep_frame_t) {
            .region = region,
            .region_num = region_num,
            .regs_frame = (uint32_t *)regs_frame
        };
    }
    return frame;
}

static inline void * cpu_domain_intpri_sleep_frame_alloc_and_init(void)
{
    const static cpu_domain_dev_regs_region_t regions[] = {
        { .start = INTPRI_CORE0_CPU_INT_ENABLE_REG, .end = INTPRI_RND_ECO_LOW_REG + 4 },
        { .start = INTPRI_RND_ECO_HIGH_REG, .end = INTPRI_RND_ECO_HIGH_REG + 4 }
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

static inline void * cpu_domain_cache_config_sleep_frame_alloc_and_init(void)
{
    const static cpu_domain_dev_regs_region_t regions[] = {
#if CONFIG_IDF_TARGET_ESP32C6
        { .start = EXTMEM_L1_CACHE_CTRL_REG, .end = EXTMEM_L1_CACHE_CTRL_REG + 4 },
        { .start = EXTMEM_L1_CACHE_WRAP_AROUND_CTRL_REG, .end = EXTMEM_L1_CACHE_WRAP_AROUND_CTRL_REG + 4 }
#elif CONFIG_IDF_TARGET_ESP32H2
        { .start = CACHE_L1_CACHE_CTRL_REG, .end = CACHE_L1_CACHE_CTRL_REG + 4 },
        { .start = CACHE_L1_CACHE_WRAP_AROUND_CTRL_REG, .end = CACHE_L1_CACHE_WRAP_AROUND_CTRL_REG + 4 }
#endif
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

static inline void * cpu_domain_plic_sleep_frame_alloc_and_init(void)
{
    const static cpu_domain_dev_regs_region_t regions[] = {
        { .start = PLIC_MXINT_ENABLE_REG, .end = PLIC_MXINT_CLAIM_REG + 4 },
        { .start = PLIC_MXINT_CONF_REG,   .end = PLIC_MXINT_CONF_REG + 4  },
        { .start = PLIC_UXINT_ENABLE_REG, .end = PLIC_UXINT_CLAIM_REG + 4 },
        { .start = PLIC_UXINT_CONF_REG,   .end = PLIC_UXINT_CONF_REG + 4  }
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

static inline void * cpu_domain_clint_sleep_frame_alloc_and_init(void)
{
    const static cpu_domain_dev_regs_region_t regions[] = {
        { .start = CLINT_MINT_SIP_REG, .end = CLINT_MINT_MTIMECMP_H_REG + 4 },
        { .start = CLINT_UINT_SIP_REG, .end = CLINT_UINT_UTIMECMP_H_REG + 4 }
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

static esp_err_t esp_sleep_cpu_retention_init_impl(void)
{
    if (s_cpu_retention.retent.critical_frame == NULL) {
        void *frame = heap_caps_calloc(1, RV_SLEEP_CTX_FRMSZ, MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
        if (frame == NULL) {
            goto err;
        }
        s_cpu_retention.retent.critical_frame = (RvCoreCriticalSleepFrame *)frame;
        rv_core_critical_regs_frame = (RvCoreCriticalSleepFrame *)frame;
    }
    if (s_cpu_retention.retent.non_critical_frame == NULL) {
        void *frame = heap_caps_calloc(1, sizeof(RvCoreNonCriticalSleepFrame), MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
        if (frame == NULL) {
            goto err;
        }
        s_cpu_retention.retent.non_critical_frame = (RvCoreNonCriticalSleepFrame *)frame;
    }
    if (s_cpu_retention.retent.intpri_frame == NULL) {
        void *frame = cpu_domain_intpri_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        s_cpu_retention.retent.intpri_frame = (cpu_domain_dev_sleep_frame_t *)frame;
    }
    if (s_cpu_retention.retent.cache_config_frame == NULL) {
        void *frame = cpu_domain_cache_config_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        s_cpu_retention.retent.cache_config_frame = (cpu_domain_dev_sleep_frame_t *)frame;
    }
    if (s_cpu_retention.retent.plic_frame == NULL) {
        void *frame = cpu_domain_plic_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        s_cpu_retention.retent.plic_frame = (cpu_domain_dev_sleep_frame_t *)frame;
    }
    if (s_cpu_retention.retent.clint_frame == NULL) {
        void *frame = cpu_domain_clint_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        s_cpu_retention.retent.clint_frame = (cpu_domain_dev_sleep_frame_t *)frame;
    }
    return ESP_OK;
err:
    esp_sleep_cpu_retention_deinit();
    return ESP_ERR_NO_MEM;
}

static esp_err_t esp_sleep_cpu_retention_deinit_impl(void)
{
    if (s_cpu_retention.retent.critical_frame) {
        heap_caps_free((void *)s_cpu_retention.retent.critical_frame);
        s_cpu_retention.retent.critical_frame = NULL;
        rv_core_critical_regs_frame = NULL;
    }
    if (s_cpu_retention.retent.non_critical_frame) {
        heap_caps_free((void *)s_cpu_retention.retent.non_critical_frame);
        s_cpu_retention.retent.non_critical_frame = NULL;
    }
    if (s_cpu_retention.retent.intpri_frame) {
        heap_caps_free((void *)s_cpu_retention.retent.intpri_frame);
        s_cpu_retention.retent.intpri_frame = NULL;
    }
    if (s_cpu_retention.retent.cache_config_frame) {
        heap_caps_free((void *)s_cpu_retention.retent.cache_config_frame);
        s_cpu_retention.retent.cache_config_frame = NULL;
    }
    if (s_cpu_retention.retent.plic_frame) {
        heap_caps_free((void *)s_cpu_retention.retent.plic_frame);
        s_cpu_retention.retent.plic_frame = NULL;
    }
    if (s_cpu_retention.retent.clint_frame) {
        heap_caps_free((void *)s_cpu_retention.retent.clint_frame);
        s_cpu_retention.retent.clint_frame = NULL;
    }
    return ESP_OK;
}

static inline IRAM_ATTR uint32_t save_mstatus_and_disable_global_int(void)
{
    uint32_t mstatus;
    __asm__ __volatile__ (
            "csrr   %0, mstatus\n"
            "csrci  mstatus, 0x8\n"
            : "=r"(mstatus)
        );
    return mstatus;
}

static inline IRAM_ATTR void restore_mstatus(uint32_t mstatus)
{
    __asm__ __volatile__ ("csrw mstatus, %0\n" :: "r"(mstatus));
}

static IRAM_ATTR RvCoreNonCriticalSleepFrame * rv_core_noncritical_regs_save(void)
{
    assert(s_cpu_retention.retent.non_critical_frame);
    RvCoreNonCriticalSleepFrame *frame = s_cpu_retention.retent.non_critical_frame;
    frame->mscratch  = RV_READ_CSR(mscratch);
    frame->mideleg   = RV_READ_CSR(mideleg);
    frame->misa      = RV_READ_CSR(misa);
    frame->tselect   = RV_READ_CSR(tselect);
    frame->tdata1    = RV_READ_CSR(tdata1);
    frame->tdata2    = RV_READ_CSR(tdata2);
    frame->tcontrol  = RV_READ_CSR(tcontrol);

    frame->pmpaddr0  = RV_READ_CSR(pmpaddr0);
    frame->pmpaddr1  = RV_READ_CSR(pmpaddr1);
    frame->pmpaddr2  = RV_READ_CSR(pmpaddr2);
    frame->pmpaddr3  = RV_READ_CSR(pmpaddr3);
    frame->pmpaddr4  = RV_READ_CSR(pmpaddr4);
    frame->pmpaddr5  = RV_READ_CSR(pmpaddr5);
    frame->pmpaddr6  = RV_READ_CSR(pmpaddr6);
    frame->pmpaddr7  = RV_READ_CSR(pmpaddr7);
    frame->pmpaddr8  = RV_READ_CSR(pmpaddr8);
    frame->pmpaddr9  = RV_READ_CSR(pmpaddr9);
    frame->pmpaddr10 = RV_READ_CSR(pmpaddr10);
    frame->pmpaddr11 = RV_READ_CSR(pmpaddr11);
    frame->pmpaddr12 = RV_READ_CSR(pmpaddr12);
    frame->pmpaddr13 = RV_READ_CSR(pmpaddr13);
    frame->pmpaddr14 = RV_READ_CSR(pmpaddr14);
    frame->pmpaddr15 = RV_READ_CSR(pmpaddr15);
    frame->pmpcfg0   = RV_READ_CSR(pmpcfg0);
    frame->pmpcfg1   = RV_READ_CSR(pmpcfg1);
    frame->pmpcfg2   = RV_READ_CSR(pmpcfg2);
    frame->pmpcfg3   = RV_READ_CSR(pmpcfg3);

#if SOC_CPU_HAS_PMA
    frame->pmaaddr0  = RV_READ_CSR(CSR_PMAADDR(0));
    frame->pmaaddr1  = RV_READ_CSR(CSR_PMAADDR(1));
    frame->pmaaddr2  = RV_READ_CSR(CSR_PMAADDR(2));
    frame->pmaaddr3  = RV_READ_CSR(CSR_PMAADDR(3));
    frame->pmaaddr4  = RV_READ_CSR(CSR_PMAADDR(4));
    frame->pmaaddr5  = RV_READ_CSR(CSR_PMAADDR(5));
    frame->pmaaddr6  = RV_READ_CSR(CSR_PMAADDR(6));
    frame->pmaaddr7  = RV_READ_CSR(CSR_PMAADDR(7));
    frame->pmaaddr8  = RV_READ_CSR(CSR_PMAADDR(8));
    frame->pmaaddr9  = RV_READ_CSR(CSR_PMAADDR(9));
    frame->pmaaddr10 = RV_READ_CSR(CSR_PMAADDR(10));
    frame->pmaaddr11 = RV_READ_CSR(CSR_PMAADDR(11));
    frame->pmaaddr12 = RV_READ_CSR(CSR_PMAADDR(12));
    frame->pmaaddr13 = RV_READ_CSR(CSR_PMAADDR(13));
    frame->pmaaddr14 = RV_READ_CSR(CSR_PMAADDR(14));
    frame->pmaaddr15 = RV_READ_CSR(CSR_PMAADDR(15));
    frame->pmacfg0   = RV_READ_CSR(CSR_PMACFG(0));
    frame->pmacfg1   = RV_READ_CSR(CSR_PMACFG(1));
    frame->pmacfg2   = RV_READ_CSR(CSR_PMACFG(2));
    frame->pmacfg3   = RV_READ_CSR(CSR_PMACFG(3));
    frame->pmacfg4   = RV_READ_CSR(CSR_PMACFG(4));
    frame->pmacfg5   = RV_READ_CSR(CSR_PMACFG(5));
    frame->pmacfg6   = RV_READ_CSR(CSR_PMACFG(6));
    frame->pmacfg7   = RV_READ_CSR(CSR_PMACFG(7));
    frame->pmacfg8   = RV_READ_CSR(CSR_PMACFG(8));
    frame->pmacfg9   = RV_READ_CSR(CSR_PMACFG(9));
    frame->pmacfg10   = RV_READ_CSR(CSR_PMACFG(10));
    frame->pmacfg11   = RV_READ_CSR(CSR_PMACFG(11));
    frame->pmacfg12   = RV_READ_CSR(CSR_PMACFG(12));
    frame->pmacfg13   = RV_READ_CSR(CSR_PMACFG(13));
    frame->pmacfg14   = RV_READ_CSR(CSR_PMACFG(14));
    frame->pmacfg15   = RV_READ_CSR(CSR_PMACFG(15));
#endif // SOC_CPU_HAS_PMA

    frame->utvec     = RV_READ_CSR(utvec);
    frame->ustatus   = RV_READ_CSR(ustatus);
    frame->uepc      = RV_READ_CSR(uepc);
    frame->ucause    = RV_READ_CSR(ucause);

    frame->mpcer     = RV_READ_CSR(CUSTOM_CSR_PCER_MACHINE);
    frame->mpcmr     = RV_READ_CSR(CUSTOM_CSR_PCMR_MACHINE);
    frame->mpccr     = RV_READ_CSR(CUSTOM_CSR_PCCR_MACHINE);
    frame->cpu_testbus_ctrl = RV_READ_CSR(CUSTOM_CSR_CPU_TESTBUS_CTRL);
    frame->upcer     = RV_READ_CSR(CUSTOM_CSR_PCER_USER);
    frame->upcmr     = RV_READ_CSR(CUSTOM_CSR_PCMR_USER);
    frame->upccr     = RV_READ_CSR(CUSTOM_CSR_PCCR_USER);
    frame->ugpio_oen = RV_READ_CSR(CUSTOM_CSR_GPIO_OEN_USER);
    frame->ugpio_in  = RV_READ_CSR(CUSTOM_CSR_GPIO_IN_USER);
    frame->ugpio_out = RV_READ_CSR(CUSTOM_CSR_GPIO_OUT_USER);
    return frame;
}

static IRAM_ATTR void rv_core_noncritical_regs_restore(RvCoreNonCriticalSleepFrame *frame)
{
    assert(frame);
    RV_WRITE_CSR(mscratch, frame->mscratch);
    RV_WRITE_CSR(mideleg,  frame->mideleg);
    RV_WRITE_CSR(misa,     frame->misa);
    RV_WRITE_CSR(tselect,  frame->tselect);
    RV_WRITE_CSR(tdata1,   frame->tdata1);
    RV_WRITE_CSR(tdata2,   frame->tdata2);
    RV_WRITE_CSR(tcontrol, frame->tcontrol);
    RV_WRITE_CSR(pmpaddr0, frame->pmpaddr0);
    RV_WRITE_CSR(pmpaddr1, frame->pmpaddr1);
    RV_WRITE_CSR(pmpaddr2, frame->pmpaddr2);
    RV_WRITE_CSR(pmpaddr3, frame->pmpaddr3);
    RV_WRITE_CSR(pmpaddr4, frame->pmpaddr4);
    RV_WRITE_CSR(pmpaddr5, frame->pmpaddr5);
    RV_WRITE_CSR(pmpaddr6, frame->pmpaddr6);
    RV_WRITE_CSR(pmpaddr7, frame->pmpaddr7);
    RV_WRITE_CSR(pmpaddr8, frame->pmpaddr8);
    RV_WRITE_CSR(pmpaddr9, frame->pmpaddr9);
    RV_WRITE_CSR(pmpaddr10,frame->pmpaddr10);
    RV_WRITE_CSR(pmpaddr11,frame->pmpaddr11);
    RV_WRITE_CSR(pmpaddr12,frame->pmpaddr12);
    RV_WRITE_CSR(pmpaddr13,frame->pmpaddr13);
    RV_WRITE_CSR(pmpaddr14,frame->pmpaddr14);
    RV_WRITE_CSR(pmpaddr15,frame->pmpaddr15);
    RV_WRITE_CSR(pmpcfg0,  frame->pmpcfg0);
    RV_WRITE_CSR(pmpcfg1,  frame->pmpcfg1);
    RV_WRITE_CSR(pmpcfg2,  frame->pmpcfg2);
    RV_WRITE_CSR(pmpcfg3,  frame->pmpcfg3);

#if SOC_CPU_HAS_PMA
    RV_WRITE_CSR(CSR_PMAADDR(0), frame->pmaaddr0);
    RV_WRITE_CSR(CSR_PMAADDR(1), frame->pmaaddr1);
    RV_WRITE_CSR(CSR_PMAADDR(2), frame->pmaaddr2);
    RV_WRITE_CSR(CSR_PMAADDR(3), frame->pmaaddr3);
    RV_WRITE_CSR(CSR_PMAADDR(4), frame->pmaaddr4);
    RV_WRITE_CSR(CSR_PMAADDR(5), frame->pmaaddr5);
    RV_WRITE_CSR(CSR_PMAADDR(6), frame->pmaaddr6);
    RV_WRITE_CSR(CSR_PMAADDR(7), frame->pmaaddr7);
    RV_WRITE_CSR(CSR_PMAADDR(8), frame->pmaaddr8);
    RV_WRITE_CSR(CSR_PMAADDR(9), frame->pmaaddr9);
    RV_WRITE_CSR(CSR_PMAADDR(10),frame->pmaaddr10);
    RV_WRITE_CSR(CSR_PMAADDR(11),frame->pmaaddr11);
    RV_WRITE_CSR(CSR_PMAADDR(12),frame->pmaaddr12);
    RV_WRITE_CSR(CSR_PMAADDR(13),frame->pmaaddr13);
    RV_WRITE_CSR(CSR_PMAADDR(14),frame->pmaaddr14);
    RV_WRITE_CSR(CSR_PMAADDR(15),frame->pmaaddr15);
    RV_WRITE_CSR(CSR_PMACFG(0),  frame->pmacfg0);
    RV_WRITE_CSR(CSR_PMACFG(1),  frame->pmacfg1);
    RV_WRITE_CSR(CSR_PMACFG(2),  frame->pmacfg2);
    RV_WRITE_CSR(CSR_PMACFG(3),  frame->pmacfg3);
    RV_WRITE_CSR(CSR_PMACFG(4),  frame->pmacfg4);
    RV_WRITE_CSR(CSR_PMACFG(5),  frame->pmacfg5);
    RV_WRITE_CSR(CSR_PMACFG(6),  frame->pmacfg6);
    RV_WRITE_CSR(CSR_PMACFG(7),  frame->pmacfg7);
    RV_WRITE_CSR(CSR_PMACFG(8),  frame->pmacfg8);
    RV_WRITE_CSR(CSR_PMACFG(9),  frame->pmacfg9);
    RV_WRITE_CSR(CSR_PMACFG(10),  frame->pmacfg10);
    RV_WRITE_CSR(CSR_PMACFG(11),  frame->pmacfg11);
    RV_WRITE_CSR(CSR_PMACFG(12),  frame->pmacfg12);
    RV_WRITE_CSR(CSR_PMACFG(13),  frame->pmacfg13);
    RV_WRITE_CSR(CSR_PMACFG(14),  frame->pmacfg14);
    RV_WRITE_CSR(CSR_PMACFG(15),  frame->pmacfg15);
#endif //SOC_CPU_HAS_PMA

    RV_WRITE_CSR(utvec,    frame->utvec);
    RV_WRITE_CSR(ustatus,  frame->ustatus);
    RV_WRITE_CSR(uepc,     frame->uepc);
    RV_WRITE_CSR(ucause,   frame->ucause);

    RV_WRITE_CSR(CUSTOM_CSR_PCER_MACHINE, frame->mpcer);
    RV_WRITE_CSR(CUSTOM_CSR_PCMR_MACHINE, frame->mpcmr);
    RV_WRITE_CSR(CUSTOM_CSR_PCCR_MACHINE, frame->mpccr);
    RV_WRITE_CSR(CUSTOM_CSR_CPU_TESTBUS_CTRL, frame->cpu_testbus_ctrl);
    RV_WRITE_CSR(CUSTOM_CSR_PCER_USER,    frame->upcer);
    RV_WRITE_CSR(CUSTOM_CSR_PCMR_USER,    frame->upcmr);
    RV_WRITE_CSR(CUSTOM_CSR_PCCR_USER,    frame->upccr);
    RV_WRITE_CSR(CUSTOM_CSR_GPIO_OEN_USER,frame->ugpio_oen);
    RV_WRITE_CSR(CUSTOM_CSR_GPIO_IN_USER, frame->ugpio_in);
    RV_WRITE_CSR(CUSTOM_CSR_GPIO_OUT_USER,frame->ugpio_out);
}

static IRAM_ATTR void cpu_domain_dev_regs_save(cpu_domain_dev_sleep_frame_t *frame)
{
    assert(frame);
    cpu_domain_dev_regs_region_t *region = frame->region;
    uint32_t *regs_frame = frame->regs_frame;

    int offset = 0;
    for (int i = 0; i < frame->region_num; i++) {
        for (uint32_t addr = region[i].start; addr < region[i].end; addr+=4) {
            regs_frame[offset++] = *(uint32_t *)addr;
        }
    }
}

static IRAM_ATTR void cpu_domain_dev_regs_restore(cpu_domain_dev_sleep_frame_t *frame)
{
    assert(frame);
    cpu_domain_dev_regs_region_t *region = frame->region;
    uint32_t *regs_frame = frame->regs_frame;

    int offset = 0;
    for (int i = 0; i < frame->region_num; i++) {
        for (uint32_t addr = region[i].start; addr < region[i].end; addr+=4) {
            *(uint32_t *)addr = regs_frame[offset++];
        }
    }
}

#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
static IRAM_ATTR void update_retention_frame_crc(uint32_t *frame_ptr, uint32_t frame_check_size, uint32_t *frame_crc_ptr)
{
    *(frame_crc_ptr) = esp_crc32_le(0, (void *)frame_ptr, frame_check_size);
}

static IRAM_ATTR void validate_retention_frame_crc(uint32_t *frame_ptr, uint32_t frame_check_size, uint32_t *frame_crc_ptr)
{
    if(*(frame_crc_ptr) != esp_crc32_le(0, (void *)(frame_ptr), frame_check_size)){
        // resume uarts
        for (int i = 0; i < SOC_UART_NUM; ++i) {
#ifndef CONFIG_IDF_TARGET_ESP32
            if (!uart_ll_is_enabled(i)) {
                continue;
            }
#endif
            uart_ll_force_xon(i);
        }

        /* Since it is still in the critical now, use ESP_EARLY_LOG */
        ESP_EARLY_LOGE(TAG, "Sleep retention frame is corrupted");
        esp_restart_noos();
    }
}
#endif

extern RvCoreCriticalSleepFrame * rv_core_critical_regs_save(void);
extern RvCoreCriticalSleepFrame * rv_core_critical_regs_restore(void);
typedef uint32_t (* sleep_cpu_entry_cb_t)(uint32_t, uint32_t, uint32_t, bool);

static IRAM_ATTR esp_err_t do_cpu_retention(sleep_cpu_entry_cb_t goto_sleep,
        uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp)
{
    RvCoreCriticalSleepFrame * frame = rv_core_critical_regs_save();
    if ((frame->pmufunc & 0x3) == 0x1) {
        esp_sleep_execute_event_callbacks(SLEEP_EVENT_SW_CPU_TO_MEM_END, (void *)0);
#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
        /* Minus 2 * sizeof(long) is for bypass `pmufunc` and `frame_crc` field */
        update_retention_frame_crc((uint32_t*)frame, RV_SLEEP_CTX_FRMSZ - 2 * sizeof(long), (uint32_t *)(&frame->frame_crc));
#endif
        REG_WRITE(LIGHT_SLEEP_WAKE_STUB_ADDR_REG, (uint32_t)rv_core_critical_regs_restore);
        return (*goto_sleep)(wakeup_opt, reject_opt, lslp_mem_inf_fpu, dslp);
    }
#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    else {
        validate_retention_frame_crc((uint32_t*)frame, RV_SLEEP_CTX_FRMSZ - 2 * sizeof(long), (uint32_t *)(&frame->frame_crc));
    }
#endif

    return pmu_sleep_finish();
}

esp_err_t IRAM_ATTR esp_sleep_cpu_retention(uint32_t (*goto_sleep)(uint32_t, uint32_t, uint32_t, bool),
        uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp)
{
    esp_sleep_execute_event_callbacks(SLEEP_EVENT_SW_CPU_TO_MEM_START, (void *)0);
    uint32_t mstatus = save_mstatus_and_disable_global_int();

    cpu_domain_dev_regs_save(s_cpu_retention.retent.plic_frame);
    cpu_domain_dev_regs_save(s_cpu_retention.retent.clint_frame);
    cpu_domain_dev_regs_save(s_cpu_retention.retent.intpri_frame);
    cpu_domain_dev_regs_save(s_cpu_retention.retent.cache_config_frame);
    RvCoreNonCriticalSleepFrame *frame = rv_core_noncritical_regs_save();

#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    /* Minus sizeof(long) is for bypass `frame_crc` field */
    update_retention_frame_crc((uint32_t*)frame, sizeof(RvCoreNonCriticalSleepFrame) - sizeof(long), (uint32_t *)(&frame->frame_crc));
#endif

    esp_err_t err = do_cpu_retention(goto_sleep, wakeup_opt, reject_opt, lslp_mem_inf_fpu, dslp);

#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    validate_retention_frame_crc((uint32_t*)frame, sizeof(RvCoreNonCriticalSleepFrame) - sizeof(long), (uint32_t *)(&frame->frame_crc));
#endif

    rv_core_noncritical_regs_restore(frame);
    cpu_domain_dev_regs_restore(s_cpu_retention.retent.cache_config_frame);
    cpu_domain_dev_regs_restore(s_cpu_retention.retent.intpri_frame);
    cpu_domain_dev_regs_restore(s_cpu_retention.retent.clint_frame);
    cpu_domain_dev_regs_restore(s_cpu_retention.retent.plic_frame);
    restore_mstatus(mstatus);
    return err;
}

#endif // SOC_PM_SUPPORT_CPU_PD && SOC_PM_CPU_RETENTION_BY_SW


#if SOC_PM_SUPPORT_CPU_PD

esp_err_t esp_sleep_cpu_retention_init(void)
{
    esp_err_t err = ESP_OK;
#if SOC_PM_CPU_RETENTION_BY_RTCCNTL
    err = esp_sleep_cpu_pd_low_init();
#elif SOC_PM_CPU_RETENTION_BY_SW
    err = esp_sleep_cpu_retention_init_impl();
#endif
    return err;
}

esp_err_t esp_sleep_cpu_retention_deinit(void)
{
    esp_err_t err = ESP_OK;
#if SOC_PM_CPU_RETENTION_BY_RTCCNTL
    err = esp_sleep_cpu_pd_low_deinit();
#elif SOC_PM_CPU_RETENTION_BY_SW
    err = esp_sleep_cpu_retention_deinit_impl();
#endif
    return err;
}

bool cpu_domain_pd_allowed(void)
{
#if SOC_PM_CPU_RETENTION_BY_RTCCNTL
    return (s_cpu_retention.retent.cpu_pd_mem != NULL);
#elif SOC_PM_CPU_RETENTION_BY_SW
    return (s_cpu_retention.retent.critical_frame != NULL) && \
         (s_cpu_retention.retent.non_critical_frame != NULL) && \
         (s_cpu_retention.retent.intpri_frame != NULL) && \
         (s_cpu_retention.retent.cache_config_frame != NULL) && \
         (s_cpu_retention.retent.plic_frame != NULL) && \
         (s_cpu_retention.retent.clint_frame != NULL);
#else
    return false;
#endif
}

esp_err_t sleep_cpu_configure(bool light_sleep_enable)
{
#if CONFIG_PM_POWER_DOWN_CPU_IN_LIGHT_SLEEP
    if (light_sleep_enable) {
        ESP_RETURN_ON_ERROR(esp_sleep_cpu_retention_init(), TAG, "Failed to enable CPU power down during light sleep.");
    } else {
        ESP_RETURN_ON_ERROR(esp_sleep_cpu_retention_deinit(), TAG, "Failed to release CPU retention memory");
    }
#endif
    return ESP_OK;
}

#endif
