/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
#include "riscv/csr.h"
#include "soc/soc_caps.h"
#include "soc/rtc_periph.h"
#include "esp_private/esp_pmu.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/sleep_event.h"
#include "sdkconfig.h"

#include "esp32c5/rom/rtc.h"
#include "esp32c5/rom/cache.h"
#include "rvsleep-frames.h"
#include "sleep_cpu_retention.h"

#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
#include "esp_private/system_internal.h"
#include "hal/uart_hal.h"
#endif


ESP_LOG_ATTR_TAG(TAG, "sleep");

static DRAM_ATTR  sleep_cpu_retention_t s_cpu_retention;

#define CUSTOM_CSR_MTVT               (0x307)
#define CUSTOM_CSR_MINTTHRESH         (0x347)
#define CUSTOM_CSR_MXSTATUS           (0x7c0)
#define CUSTOM_CSR_MHCR               (0x7c1)
#define CUSTOM_CSR_MHINT              (0x7c5)
#define CUSTOM_CSR_MEXSTATUS          (0x7e1)
#define CUSTOM_CSR_JVT                (0x017)

extern RvCoreCriticalSleepFrame *rv_core_critical_regs_frame;

FORCE_INLINE_ATTR void save_csr_disable_global_int(uint32_t *mstatus_val, uint32_t *mintthresh_val)
{
#if __riscv_zcmp && SOC_CPU_ZCMP_WORKAROUND
    *mintthresh_val = rv_utils_set_intlevel_regval(0xff);
#else
    (void) mintthresh_val;
#endif
    *mstatus_val = RV_READ_MSTATUS_AND_DISABLE_INTR();
}

FORCE_INLINE_ATTR void restore_csr_enable_global_int(uint32_t mstatus_val, uint32_t mintthresh_val)
{
    RV_WRITE_CSR(mstatus, mstatus_val);
#if __riscv_zcmp && SOC_CPU_ZCMP_WORKAROUND
    rv_utils_restore_intlevel_regval(mintthresh_val);
#else
    (void) mintthresh_val;
#endif
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

    RV_SLEEP_SAVE_PMP_REGS(frame->pmpaddr, frame->pmpcfg);

    RV_SLEEP_SAVE_PMA_REGS(frame->pmaaddr, frame->pmacfg);

    frame->mcycle     = RV_READ_CSR(mcycle);

    frame->mtvt       = RV_READ_CSR(CUSTOM_CSR_MTVT);
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
    RV_SLEEP_RESTORE_PMP_REGS(frame->pmpaddr, frame->pmpcfg);

    RV_SLEEP_RESTORE_PMA_REGS(frame->pmaaddr, frame->pmacfg);

    RV_WRITE_CSR(mcycle,   frame->mcycle);

    RV_WRITE_CSR(CUSTOM_CSR_MTVT, frame->mtvt);
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
        update_retention_frame_crc((uint32_t*)frame, RV_SLEEP_CTX_SZ1 - 2 * sizeof(long), (uint32_t *)(&frame->frame_crc));
#endif
        REG_WRITE(RTC_SLEEP_WAKE_STUB_ADDR_REG, (uint32_t)rv_core_critical_regs_restore);
        return (*goto_sleep)(wakeup_opt, reject_opt, lslp_mem_inf_fpu, dslp);
    }
#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    else {
        validate_retention_frame_crc((uint32_t*)frame, RV_SLEEP_CTX_SZ1 - 2 * sizeof(long), (uint32_t *)(&frame->frame_crc));
    }
#endif

    return pmu_sleep_finish(dslp);
}

esp_err_t IRAM_ATTR esp_sleep_cpu_retention(uint32_t (*goto_sleep)(uint32_t, uint32_t, uint32_t, bool),
        uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp)
{
    uint32_t mstatus = 0;
    uint32_t mintthresh = 0;
    esp_sleep_execute_event_callbacks(SLEEP_EVENT_SW_CPU_TO_MEM_START, (void *)0);
    save_csr_disable_global_int(&mstatus, &mintthresh);

    cpu_domain_dev_regs_save(s_cpu_retention.retent.clic_frame);
    cpu_domain_dev_regs_save(s_cpu_retention.retent.clint_frame);
    cpu_domain_dev_regs_save(s_cpu_retention.retent.cache_config_frame);
    RvCoreNonCriticalSleepFrame *frame = rv_core_noncritical_regs_save();

#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
    /* Minus sizeof(long) is for bypass `frame_crc` field */
    update_retention_frame_crc((uint32_t*)frame, sizeof(RvCoreNonCriticalSleepFrame) - sizeof(long), (uint32_t *)(&frame->frame_crc));
#endif
    esp_err_t err = do_cpu_retention(goto_sleep, wakeup_opt, reject_opt, lslp_mem_inf_fpu, dslp);

    if (err == ESP_OK) {
#if CONFIG_PM_CHECK_SLEEP_RETENTION_FRAME
        validate_retention_frame_crc((uint32_t*)frame, sizeof(RvCoreNonCriticalSleepFrame) - sizeof(long), (uint32_t *)(&frame->frame_crc));
#endif

        rv_core_noncritical_regs_restore(frame);
        cpu_domain_dev_regs_restore(s_cpu_retention.retent.cache_config_frame);
        cpu_domain_dev_regs_restore(s_cpu_retention.retent.clint_frame);
        cpu_domain_dev_regs_restore(s_cpu_retention.retent.clic_frame);
    }
    restore_csr_enable_global_int(mstatus, mintthresh);
    return err;
}

esp_err_t esp_sleep_cpu_retention_init(void)
{
    return esp_sleep_cpu_retention_init_impl(& s_cpu_retention);
}

esp_err_t esp_sleep_cpu_retention_deinit(void)
{
    return esp_sleep_cpu_retention_deinit_impl(& s_cpu_retention);
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
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    if (light_sleep_enable) {
        ESP_RETURN_ON_ERROR(esp_sleep_cpu_retention_init(), TAG, "Failed to enable CPU power down during light sleep.");
    } else {
        ESP_RETURN_ON_ERROR(esp_sleep_cpu_retention_deinit(), TAG, "Failed to release CPU retention memory");
    }
#endif
    return ESP_OK;
}
