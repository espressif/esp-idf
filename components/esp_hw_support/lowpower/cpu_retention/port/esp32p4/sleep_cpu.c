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
#include "esp_crc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "riscv/csr.h"
#include "soc/cache_reg.h"
#include "soc/clic_reg.h"
#include "soc/rtc_periph.h"
#include "soc/soc_caps.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/sleep_event.h"
#include "sdkconfig.h"
#include "esp_private/esp_pmu.h"

#include "esp32p4/rom/ets_sys.h"
#include "esp32p4/rom/rtc.h"
#include "esp32p4/rom/cache.h"
#include "rvsleep-frames.h"

#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
#include "esp_private/system_internal.h"
#include "hal/uart_hal.h"
#endif


#if ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE
#include <stdatomic.h>
#include "soc/hp_system_reg.h"
typedef enum {
    SMP_IDLE,
    SMP_BACKUP_START,
    SMP_BACKUP_DONE,
    SMP_RESTORE_START,
    SMP_RESTORE_DONE,
    SMP_SKIP_RETENTION,
} smp_retention_state_t;

static TCM_DRAM_ATTR smp_retention_state_t s_smp_retention_state[portNUM_PROCESSORS];
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
        RvCoreCriticalSleepFrame *critical_frame[portNUM_PROCESSORS];
        RvCoreNonCriticalSleepFrame *non_critical_frame[portNUM_PROCESSORS];
        cpu_domain_dev_sleep_frame_t *cache_config_frame;
        cpu_domain_dev_sleep_frame_t *clic_frame[portNUM_PROCESSORS];
    } retent;
} sleep_cpu_retention_t;

static TCM_DRAM_ATTR __attribute__((unused)) sleep_cpu_retention_t s_cpu_retention;

extern RvCoreCriticalSleepFrame *rv_core_critical_regs_frame[portNUM_PROCESSORS];

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
        { .start = CACHE_L2_CACHE_CTRL_REG, .end = CACHE_L2_CACHE_BLOCKSIZE_CONF_REG + 4 }
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions, sizeof(regions) / sizeof(regions[0]));
}

static inline void * cpu_domain_clic_sleep_frame_alloc_and_init(uint8_t core_id)
{
    const static cpu_domain_dev_regs_region_t regions[portNUM_PROCESSORS][2] = {
       [0 ... portNUM_PROCESSORS - 1] = {
            { .start = CLIC_INT_CONFIG_REG, .end = CLIC_INT_THRESH_REG + 4 },
            { .start = CLIC_INT_CTRL_REG(0), .end = CLIC_INT_CTRL_REG(47) + 4 },
        }
    };
    return cpu_domain_dev_sleep_frame_alloc_and_init(regions[core_id], sizeof(regions[core_id]) / sizeof(cpu_domain_dev_regs_region_t));
}

static esp_err_t esp_sleep_cpu_retention_init_impl(void)
{
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (s_cpu_retention.retent.critical_frame[core_id] == NULL) {
            void *frame = heap_caps_calloc(1, RV_SLEEP_CTX_FRMSZ, MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
            if (frame == NULL) {
                goto err;
            }
            s_cpu_retention.retent.critical_frame[core_id] = (RvCoreCriticalSleepFrame *)frame;
            rv_core_critical_regs_frame[core_id] = (RvCoreCriticalSleepFrame *)frame;
        }
        if (s_cpu_retention.retent.non_critical_frame[core_id] == NULL) {
            void *frame = heap_caps_calloc(1, sizeof(RvCoreNonCriticalSleepFrame), MALLOC_CAP_32BIT|MALLOC_CAP_INTERNAL);
            if (frame == NULL) {
                goto err;
            }
            s_cpu_retention.retent.non_critical_frame[core_id] = (RvCoreNonCriticalSleepFrame *)frame;
        }
    }
    if (s_cpu_retention.retent.cache_config_frame == NULL) {
        void *frame = cpu_domain_cache_config_sleep_frame_alloc_and_init();
        if (frame == NULL) {
            goto err;
        }
        s_cpu_retention.retent.cache_config_frame = (cpu_domain_dev_sleep_frame_t *)frame;
    }
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (s_cpu_retention.retent.clic_frame[core_id] == NULL) {
            void *frame = cpu_domain_clic_sleep_frame_alloc_and_init(core_id);
            if (frame == NULL) {
                goto err;
            }
            s_cpu_retention.retent.clic_frame[core_id] = (cpu_domain_dev_sleep_frame_t *)frame;
        }
    }
#if ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        atomic_init(&s_smp_retention_state[core_id], SMP_IDLE);
    }
#endif
    return ESP_OK;
err:
    esp_sleep_cpu_retention_deinit();
    return ESP_ERR_NO_MEM;
}

static esp_err_t esp_sleep_cpu_retention_deinit_impl(void)
{
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (s_cpu_retention.retent.critical_frame[core_id]) {
            heap_caps_free((void *)s_cpu_retention.retent.critical_frame[core_id]);
            s_cpu_retention.retent.critical_frame[core_id] = NULL;
            rv_core_critical_regs_frame[core_id] = NULL;
        }
        if (s_cpu_retention.retent.non_critical_frame[core_id]) {
            heap_caps_free((void *)s_cpu_retention.retent.non_critical_frame[core_id]);
            s_cpu_retention.retent.non_critical_frame[core_id] = NULL;
        }
    }
    if (s_cpu_retention.retent.cache_config_frame) {
        heap_caps_free((void *)s_cpu_retention.retent.cache_config_frame);
        s_cpu_retention.retent.cache_config_frame = NULL;
    }
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        if (s_cpu_retention.retent.clic_frame[core_id]) {
            heap_caps_free((void *)s_cpu_retention.retent.clic_frame[core_id]);
            s_cpu_retention.retent.clic_frame[core_id] = NULL;
        }
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

static TCM_IRAM_ATTR RvCoreNonCriticalSleepFrame * rv_core_noncritical_regs_save(void)
{
    RvCoreNonCriticalSleepFrame *frame = s_cpu_retention.retent.non_critical_frame[esp_cpu_get_core_id()];

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
    frame->mcycle    = RV_READ_CSR(mcycle);
    return frame;
}

static TCM_IRAM_ATTR void rv_core_noncritical_regs_restore(void)
{
    RvCoreNonCriticalSleepFrame *frame = s_cpu_retention.retent.non_critical_frame[esp_cpu_get_core_id()];

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
    RV_WRITE_CSR(mcycle, frame->mcycle);
}

static TCM_IRAM_ATTR void cpu_domain_dev_regs_save(cpu_domain_dev_sleep_frame_t *frame)
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

static TCM_IRAM_ATTR void cpu_domain_dev_regs_restore(cpu_domain_dev_sleep_frame_t *frame)
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
static TCM_IRAM_ATTR void update_retention_frame_crc(uint32_t *frame_ptr, uint32_t frame_check_size, uint32_t *frame_crc_ptr)
{
    *(frame_crc_ptr) = esp_crc32_le(0, (void *)frame_ptr, frame_check_size);
}

static TCM_IRAM_ATTR void validate_retention_frame_crc(uint32_t *frame_ptr, uint32_t frame_check_size, uint32_t *frame_crc_ptr)
{
    if(*(frame_crc_ptr) != esp_crc32_le(0, (void *)(frame_ptr), frame_check_size)){
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

static TCM_IRAM_ATTR esp_err_t do_cpu_retention(sleep_cpu_entry_cb_t goto_sleep,
        uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp)
{
    uint8_t core_id = esp_cpu_get_core_id();
    rv_core_critical_regs_save();

    RvCoreCriticalSleepFrame * frame = s_cpu_retention.retent.critical_frame[core_id];
    if ((frame->pmufunc & 0x3) == 0x1) {
        esp_sleep_execute_event_callbacks(SLEEP_EVENT_SW_CPU_TO_MEM_END, (void *)0);
#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
        /* Minus 2 * sizeof(long) is for bypass `pmufunc` and `frame_crc` field */
        update_retention_frame_crc((uint32_t*)frame, RV_SLEEP_CTX_SZ1 - 2 * sizeof(long), (uint32_t *)(&frame->frame_crc));
#endif
        REG_WRITE(RTC_SLEEP_WAKE_STUB_ADDR_REG, (uint32_t)rv_core_critical_regs_restore);

#if ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE
        atomic_store(&s_smp_retention_state[core_id], SMP_BACKUP_DONE);
        while (atomic_load(&s_smp_retention_state[!core_id]) != SMP_BACKUP_DONE) {
            ;
        }
#endif

        return (*goto_sleep)(wakeup_opt, reject_opt, lslp_mem_inf_fpu, dslp);
    }
#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    else {
        validate_retention_frame_crc((uint32_t*)frame, RV_SLEEP_CTX_SZ1 - 2 * sizeof(long), (uint32_t *)(&frame->frame_crc));
    }
#endif

    return pmu_sleep_finish();
}

esp_err_t TCM_IRAM_ATTR esp_sleep_cpu_retention(uint32_t (*goto_sleep)(uint32_t, uint32_t, uint32_t, bool),
        uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp)
{
    esp_sleep_execute_event_callbacks(SLEEP_EVENT_SW_CPU_TO_MEM_START, (void *)0);
    uint32_t mstatus = save_mstatus_and_disable_global_int();
    uint8_t core_id = esp_cpu_get_core_id();
#if ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE
    atomic_store(&s_smp_retention_state[core_id], SMP_BACKUP_START);
#endif
    cpu_domain_dev_regs_save(s_cpu_retention.retent.clic_frame[core_id]);
    cpu_domain_dev_regs_save(s_cpu_retention.retent.cache_config_frame);
    rv_core_noncritical_regs_save();

#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    RvCoreNonCriticalSleepFrame *frame = s_cpu_retention.retent.non_critical_frame[core_id];
    /* Minus sizeof(long) is for bypass `frame_crc` field */
    update_retention_frame_crc((uint32_t*)frame, sizeof(RvCoreNonCriticalSleepFrame) - sizeof(long), (uint32_t *)(&frame->frame_crc));
#endif

    esp_err_t err = do_cpu_retention(goto_sleep, wakeup_opt, reject_opt, lslp_mem_inf_fpu, dslp);

#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    validate_retention_frame_crc((uint32_t*)frame, sizeof(RvCoreNonCriticalSleepFrame) - sizeof(long), (uint32_t *)(&frame->frame_crc));
#endif

#if ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE
    // Start core1
    if (core_id == 0) {
        REG_SET_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_CPU_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_CORE1_GLOBAL);
    }

    atomic_store(&s_smp_retention_state[core_id], SMP_RESTORE_START);
#endif

    rv_core_noncritical_regs_restore();
    cpu_domain_dev_regs_restore(s_cpu_retention.retent.cache_config_frame);
    cpu_domain_dev_regs_restore(s_cpu_retention.retent.clic_frame[core_id]);
    restore_mstatus(mstatus);

#if ESP_SLEEP_POWER_DOWN_CPU && !CONFIG_FREERTOS_UNICORE
    atomic_store(&s_smp_retention_state[core_id], SMP_RESTORE_DONE);
#endif
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
    bool allowed = true;
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        allowed &= (s_cpu_retention.retent.critical_frame[core_id] != NULL);
        allowed &= (s_cpu_retention.retent.non_critical_frame[core_id] != NULL);
    }
    allowed &= (s_cpu_retention.retent.cache_config_frame != NULL);
    for (uint8_t core_id = 0; core_id < portNUM_PROCESSORS; ++core_id) {
        allowed &= (s_cpu_retention.retent.clic_frame[core_id] != NULL);
    }
    return allowed;
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

#if !CONFIG_FREERTOS_UNICORE
#if ESP_SLEEP_POWER_DOWN_CPU
static TCM_IRAM_ATTR void smp_core_do_retention(void)
{
    uint8_t core_id = esp_cpu_get_core_id();

    if (core_id == 0) {
        WRITE_PERI_REG(HP_SYSTEM_CPU_INT_FROM_CPU_2_REG, 0);
    } else {
        WRITE_PERI_REG(HP_SYSTEM_CPU_INT_FROM_CPU_3_REG, 0);
    }

    // Wait another core start to do retention
    bool smp_skip_retention = false;
    while (1) {
        smp_retention_state_t another_core_state = atomic_load(&s_smp_retention_state[!core_id]);
        if (another_core_state == SMP_SKIP_RETENTION) {
            // If another core skips the retention, the current core should also have to skip it.
            smp_skip_retention = true;
            break;
        } else if (another_core_state == SMP_BACKUP_START) {
            break;
        }
    }

    if (!smp_skip_retention) {
        atomic_store(&s_smp_retention_state[core_id], SMP_BACKUP_START);
        rv_core_noncritical_regs_save();
        cpu_domain_dev_regs_save(s_cpu_retention.retent.clic_frame[core_id]);
        rv_core_critical_regs_save();
        RvCoreCriticalSleepFrame *frame_critical = s_cpu_retention.retent.critical_frame[core_id];
        if ((frame_critical->pmufunc & 0x3) == 0x1) {
            atomic_store(&s_smp_retention_state[core_id], SMP_BACKUP_DONE);
            // wait another core trigger sleep and wakeup
            esp_cpu_wait_for_intr();
            while (1) {
                ;
            }
        } else {
            // Start core1
            if (core_id == 0) {
                REG_SET_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_CORE1_CPU_CLK_EN);
                REG_CLR_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_CORE1_GLOBAL);
            }
            atomic_store(&s_smp_retention_state[core_id], SMP_RESTORE_START);
            cpu_domain_dev_regs_restore(s_cpu_retention.retent.clic_frame[core_id]);
            rv_core_noncritical_regs_restore();
            atomic_store(&s_smp_retention_state[core_id], SMP_RESTORE_DONE);
        }
    }
    // wait another core out sleep
    while (atomic_load(&s_smp_retention_state[!core_id]) != SMP_IDLE) {
        ;
    }
    atomic_store(&s_smp_retention_state[core_id], SMP_IDLE);
}


TCM_IRAM_ATTR void esp_sleep_cpu_skip_retention(void) {
    atomic_store(&s_smp_retention_state[esp_cpu_get_core_id()], SMP_SKIP_RETENTION);
}
#endif

void sleep_smp_cpu_sleep_prepare(void)
{
#if ESP_SLEEP_POWER_DOWN_CPU
    while (atomic_load(&s_smp_retention_state[!esp_cpu_get_core_id()]) != SMP_IDLE) {
        ;
    }
    esp_ipc_isr_call((esp_ipc_isr_func_t)smp_core_do_retention, NULL);
#else
    esp_ipc_isr_stall_other_cpu();
#endif
}

void sleep_smp_cpu_wakeup_prepare(void)
{
#if ESP_SLEEP_POWER_DOWN_CPU
    uint8_t core_id = esp_cpu_get_core_id();
    if (atomic_load(&s_smp_retention_state[core_id]) == SMP_RESTORE_DONE) {
        while (atomic_load(&s_smp_retention_state[!core_id]) != SMP_RESTORE_DONE) {
            ;
        }
    }
    atomic_store(&s_smp_retention_state[core_id], SMP_IDLE);
#else
    esp_ipc_isr_release_other_cpu();
#endif
}
#endif //!CONFIG_FREERTOS_UNICORE
