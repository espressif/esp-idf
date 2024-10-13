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
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_rom_crc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "riscv/csr.h"
#include "soc/soc_caps.h"
#include "soc/intpri_reg.h"
#include "soc/cache_reg.h"
#include "soc/clic_reg.h"
#include "soc/clint_reg.h"
#include "soc/rtc_periph.h"
#include "esp_private/esp_pmu.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/sleep_event.h"
#include "sdkconfig.h"

#include "esp32c5/rom/rtc.h"
#include "esp32c5/rom/cache.h"
#include "rvsleep-frames.h"

#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
#include "esp_private/system_internal.h"
#include "hal/uart_hal.h"
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
    struct {
        RvCoreCriticalSleepFrame *critical_frame;
        RvCoreNonCriticalSleepFrame *non_critical_frame;
        cpu_domain_dev_sleep_frame_t *cache_config_frame;
        cpu_domain_dev_sleep_frame_t *clic_frame;
        cpu_domain_dev_sleep_frame_t *clint_frame;
    } retent;
} sleep_cpu_retention_t;

static DRAM_ATTR  sleep_cpu_retention_t s_cpu_retention;

#define CUSTOM_CSR_MTVT               (0x307)
#define CUSTOM_CSR_MINTTHRESH         (0x347)
#define CUSTOM_CSR_MXSTATUS           (0x7c0)
#define CUSTOM_CSR_MHCR               (0x7c1)
#define CUSTOM_CSR_MHINT              (0x7c5)
#define CUSTOM_CSR_MEXSTATUS          (0x7e1)
#define CUSTOM_CSR_JVT                (0x017)

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


static inline void * cpu_domain_cache_config_sleep_frame_alloc_and_init(void)
{
    const static cpu_domain_dev_regs_region_t regions[] = {
        { .start = CACHE_L1_ICACHE_CTRL_REG, .end = CACHE_L1_BYPASS_CACHE_CONF_REG + 4 },
        { .start = CACHE_L1_CACHE_AUTOLOAD_CTRL_REG, .end = CACHE_L1_CACHE_AUTOLOAD_SCT1_SIZE_REG + 4 },
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

static inline void * cpu_domain_clint_sleep_frame_alloc_and_init(void)
{
    const static cpu_domain_dev_regs_region_t regions[] = {
        { .start = CLINT_MINT_SIP_REG,        .end = CLINT_MINT_SIP_REG + 4        },
        { .start = CLINT_MINT_MTIMECMP_L_REG, .end = CLINT_MINT_MTIMECMP_H_REG + 4 },
        { .start = CLINT_MINT_TIMECTL_REG,    .end = CLINT_MINT_TIMECTL_REG + 4    },
        { .start = CLINT_MINT_MTIME_L_REG,    .end = CLINT_MINT_MTIME_H_REG + 4    }
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

static inline void * cpu_domain_clic_sleep_frame_alloc_and_init(void)
{
    const static cpu_domain_dev_regs_region_t regions[] = {
        { .start = CLIC_INT_CONFIG_REG, .end = CLIC_INT_THRESH_REG + 4 },
        { .start = CLIC_INT_CTRL_REG(0), .end = CLIC_INT_CTRL_REG(47) + 4 },
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
    if (s_cpu_retention.retent.cache_config_frame == NULL) {
        void *frame = cpu_domain_cache_config_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        s_cpu_retention.retent.cache_config_frame = (cpu_domain_dev_sleep_frame_t *)frame;
    }
    if (s_cpu_retention.retent.clic_frame == NULL) {
        void *frame = cpu_domain_clic_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        s_cpu_retention.retent.clic_frame = (cpu_domain_dev_sleep_frame_t *)frame;
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
    if (s_cpu_retention.retent.cache_config_frame) {
        heap_caps_free((void *)s_cpu_retention.retent.cache_config_frame);
        s_cpu_retention.retent.cache_config_frame = NULL;
    }
    if (s_cpu_retention.retent.clic_frame) {
        heap_caps_free((void *)s_cpu_retention.retent.clic_frame);
        s_cpu_retention.retent.clic_frame = NULL;
    }
    if (s_cpu_retention.retent.clint_frame) {
        heap_caps_free((void *)s_cpu_retention.retent.clint_frame);
        s_cpu_retention.retent.clint_frame = NULL;
    }
    return ESP_OK;
}

FORCE_INLINE_ATTR uint32_t save_mstatus_and_disable_global_int(void)
{
    return RV_READ_MSTATUS_AND_DISABLE_INTR();
}

FORCE_INLINE_ATTR void restore_mstatus(uint32_t mstatus_val)
{
    RV_WRITE_CSR(mstatus, mstatus_val);
}

static IRAM_ATTR RvCoreNonCriticalSleepFrame * rv_core_noncritical_regs_save(void)
{
    assert(s_cpu_retention.retent.non_critical_frame);
    RvCoreNonCriticalSleepFrame *frame = s_cpu_retention.retent.non_critical_frame;
    frame->mscratch  = RV_READ_CSR(mscratch);
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

    frame->mcycle     = RV_READ_CSR(mcycle);

    frame->mtvt       = RV_READ_CSR(CUSTOM_CSR_MTVT);
    frame->mintthresh = RV_READ_CSR(CUSTOM_CSR_MINTTHRESH);
    frame->mxstatus   = RV_READ_CSR(CUSTOM_CSR_MXSTATUS);
    frame->mhcr       = RV_READ_CSR(CUSTOM_CSR_MHCR);
    frame->mhint      = RV_READ_CSR(CUSTOM_CSR_MHINT);
    frame->mexstatus  = RV_READ_CSR(CUSTOM_CSR_MEXSTATUS);
    frame->jvt        = RV_READ_CSR(CUSTOM_CSR_JVT);
    return frame;
}

static IRAM_ATTR void rv_core_noncritical_regs_restore(RvCoreNonCriticalSleepFrame *frame)
{
    assert(frame);
    RV_WRITE_CSR(mscratch, frame->mscratch);
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

    RV_WRITE_CSR(mcycle,   frame->mcycle);

    RV_WRITE_CSR(CUSTOM_CSR_MTVT, frame->mtvt);
    RV_WRITE_CSR(CUSTOM_CSR_MINTTHRESH, frame->mintthresh);
    RV_WRITE_CSR(CUSTOM_CSR_MXSTATUS, frame->mxstatus);
    RV_WRITE_CSR(CUSTOM_CSR_MHCR, frame->mhcr);
    RV_WRITE_CSR(CUSTOM_CSR_MHINT, frame->mhint);
    RV_WRITE_CSR(CUSTOM_CSR_MEXSTATUS, frame->mexstatus);
    RV_WRITE_CSR(CUSTOM_CSR_JVT, frame->jvt);
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
    *(frame_crc_ptr) = esp_rom_crc32_le(0, (void *)frame_ptr, frame_check_size);
}

static IRAM_ATTR void validate_retention_frame_crc(uint32_t *frame_ptr, uint32_t frame_check_size, uint32_t *frame_crc_ptr)
{
    if(*(frame_crc_ptr) != esp_rom_crc32_le(0, (void *)(frame_ptr), frame_check_size)){
        // resume uarts
        for (int i = 0; i < SOC_UART_NUM; ++i) {
            if (!uart_ll_is_enabled(i)) {
                continue;
            }
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
        REG_WRITE(RTC_SLEEP_WAKE_STUB_ADDR_REG, (uint32_t)rv_core_critical_regs_restore);
        return (*goto_sleep)(wakeup_opt, reject_opt, lslp_mem_inf_fpu, dslp);
    }
#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    else {
        validate_retention_frame_crc((uint32_t*)frame, RV_SLEEP_CTX_FRMSZ - 2 * sizeof(long), (uint32_t *)(&frame->frame_crc));
    }
#endif

    return pmu_sleep_finish(dslp);
}

esp_err_t IRAM_ATTR esp_sleep_cpu_retention(uint32_t (*goto_sleep)(uint32_t, uint32_t, uint32_t, bool),
        uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp)
{
    esp_sleep_execute_event_callbacks(SLEEP_EVENT_SW_CPU_TO_MEM_START, (void *)0);
    uint32_t mstatus = save_mstatus_and_disable_global_int();

    cpu_domain_dev_regs_save(s_cpu_retention.retent.clic_frame);
    cpu_domain_dev_regs_save(s_cpu_retention.retent.clint_frame);
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
    cpu_domain_dev_regs_restore(s_cpu_retention.retent.clint_frame);
    cpu_domain_dev_regs_restore(s_cpu_retention.retent.clic_frame);
    restore_mstatus(mstatus);
    return err;
}

esp_err_t esp_sleep_cpu_retention_init(void)
{
    return esp_sleep_cpu_retention_init_impl();
}

esp_err_t esp_sleep_cpu_retention_deinit(void)
{
    return esp_sleep_cpu_retention_deinit_impl();
}

bool cpu_domain_pd_allowed(void)
{
    return (s_cpu_retention.retent.critical_frame != NULL) && \
         (s_cpu_retention.retent.non_critical_frame != NULL) && \
         (s_cpu_retention.retent.cache_config_frame != NULL) && \
         (s_cpu_retention.retent.clic_frame != NULL) && \
         (s_cpu_retention.retent.clint_frame != NULL);
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
