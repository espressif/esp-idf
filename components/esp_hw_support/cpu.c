/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdint.h>
#include <assert.h>
#include "soc/soc.h"
#include "soc/soc_caps.h"

// TODO: IDF-5645
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
#include "soc/lp_aon_reg.h"
#include "soc/pcr_reg.h"
#define SYSTEM_CPU_PER_CONF_REG PCR_CPU_WAITI_CONF_REG
#define SYSTEM_CPU_WAIT_MODE_FORCE_ON PCR_CPU_WAIT_MODE_FORCE_ON
#else
#include "soc/rtc_cntl_reg.h"
#endif

#include "hal/soc_hal.h"
#include "esp_bit_defs.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_cpu.h"
#if __XTENSA__
#include "xtensa/config/core-isa.h"
#else
#include "soc/system_reg.h"     // For SYSTEM_CPU_PER_CONF_REG
#include "soc/dport_access.h"   // For Dport access
#include "riscv/semihosting.h"
#endif
#if SOC_CPU_HAS_FLEXIBLE_INTC
#include "riscv/instruction_decode.h"
#endif


/* --------------------------------------------------- CPU Control -----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

void esp_cpu_stall(int core_id)
{
    assert(core_id >= 0 && core_id < SOC_CPU_CORES_NUM);
#if SOC_CPU_CORES_NUM > 1   // We don't allow stalling of the current core
    /*
    We need to write the value "0x86" to stall a particular core. The write location is split into two separate
    bit fields named "c0" and "c1", and the two fields are located in different registers. Each core has its own pair of
    "c0" and "c1" bit fields.

    Note: This function can be called when the cache is disabled. We use "ternary if" instead of an array so that the
    "rodata" of the register masks/shifts will be stored in this function's "rodata" section, instead of the source
    file's "rodata" section (see IDF-5214).
    */
    int rtc_cntl_c0_m = (core_id == 0) ? RTC_CNTL_SW_STALL_PROCPU_C0_M : RTC_CNTL_SW_STALL_APPCPU_C0_M;
    int rtc_cntl_c0_s = (core_id == 0) ? RTC_CNTL_SW_STALL_PROCPU_C0_S : RTC_CNTL_SW_STALL_APPCPU_C0_S;
    int rtc_cntl_c1_m = (core_id == 0) ? RTC_CNTL_SW_STALL_PROCPU_C1_M : RTC_CNTL_SW_STALL_APPCPU_C1_M;
    int rtc_cntl_c1_s = (core_id == 0) ? RTC_CNTL_SW_STALL_PROCPU_C1_S : RTC_CNTL_SW_STALL_APPCPU_C1_S;
    CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, rtc_cntl_c0_m);
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, 2 << rtc_cntl_c0_s);
    CLEAR_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, rtc_cntl_c1_m);
    SET_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, 0x21 << rtc_cntl_c1_s);
#endif
}

void esp_cpu_unstall(int core_id)
{
    assert(core_id >= 0 && core_id < SOC_CPU_CORES_NUM);
#if SOC_CPU_CORES_NUM > 1   // We don't allow stalling of the current core
    /*
    We need to write clear the value "0x86" to unstall a particular core. The location of this value is split into
    two separate bit fields named "c0" and "c1", and the two fields are located in different registers. Each core has
    its own pair of "c0" and "c1" bit fields.

    Note: This function can be called when the cache is disabled. We use "ternary if" instead of an array so that the
    "rodata" of the register masks/shifts will be stored in this function's "rodata" section, instead of the source
    file's "rodata" section (see IDF-5214).
    */
    int rtc_cntl_c0_m = (core_id == 0) ? RTC_CNTL_SW_STALL_PROCPU_C0_M : RTC_CNTL_SW_STALL_APPCPU_C0_M;
    int rtc_cntl_c1_m = (core_id == 0) ? RTC_CNTL_SW_STALL_PROCPU_C1_M : RTC_CNTL_SW_STALL_APPCPU_C1_M;
    CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, rtc_cntl_c0_m);
    CLEAR_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, rtc_cntl_c1_m);
#endif
}

void esp_cpu_reset(int core_id)
{
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2// TODO: IDF-5645
    SET_PERI_REG_MASK(LP_AON_CPUCORE0_CFG_REG, LP_AON_CPU_CORE0_SW_RESET);
#else
    assert(core_id >= 0 && core_id < SOC_CPU_CORES_NUM);
#if SOC_CPU_CORES_NUM > 1
    /*
    Note: This function can be called when the cache is disabled. We use "ternary if" instead of an array so that the
    "rodata" of the register masks/shifts will be stored in this function's "rodata" section, instead of the source
    file's "rodata" section (see IDF-5214).
    */
    int rtc_cntl_rst_m = (core_id == 0) ? RTC_CNTL_SW_PROCPU_RST_M : RTC_CNTL_SW_APPCPU_RST_M;
#else // SOC_CPU_CORES_NUM > 1
    int rtc_cntl_rst_m = RTC_CNTL_SW_PROCPU_RST_M;
#endif // SOC_CPU_CORES_NUM > 1
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, rtc_cntl_rst_m);
#endif
}

void esp_cpu_wait_for_intr(void)
{
#if __XTENSA__
    xt_utils_wait_for_intr();
#else
    // TODO: IDF-5645 (better to implement with ll) C6 register names converted in the #include section at the top
    if (esp_cpu_dbgr_is_attached() && DPORT_REG_GET_BIT(SYSTEM_CPU_PER_CONF_REG, SYSTEM_CPU_WAIT_MODE_FORCE_ON) == 0) {
        /* when SYSTEM_CPU_WAIT_MODE_FORCE_ON is disabled in WFI mode SBA access to memory does not work for debugger,
           so do not enter that mode when debugger is connected */
        return;
    }
    rv_utils_wait_for_intr();
#endif // __XTENSA__
}

/* ---------------------------------------------------- Debugging ------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------- Breakpoints/Watchpoints -----------------

#if SOC_CPU_BREAKPOINTS_NUM > 0
esp_err_t esp_cpu_set_breakpoint(int bp_num, const void *bp_addr)
{
    /*
    Todo:
    - Check that bp_num is in range
    */
#if __XTENSA__
    xt_utils_set_breakpoint(bp_num, (uint32_t)bp_addr);
#else
    if (esp_cpu_dbgr_is_attached()) {
        /* If we want to set breakpoint which when hit transfers control to debugger
         * we need to set `action` in `mcontrol` to 1 (Enter Debug Mode).
         * That `action` value is supported only when `dmode` of `tdata1` is set.
         * But `dmode` can be modified by debugger only (from Debug Mode).
         *
         * So when debugger is connected we use special syscall to ask it to set breakpoint for us.
         */
        long args[] = {true, bp_num, (long)bp_addr};
        int ret = semihosting_call_noerrno(ESP_SEMIHOSTING_SYS_BREAKPOINT_SET, args);
        if (ret == 0) {
            return ESP_ERR_INVALID_RESPONSE;
        }
    } else {
        rv_utils_set_breakpoint(bp_num, (uint32_t)bp_addr);
	}
#endif // __XTENSA__
    return ESP_OK;
}

esp_err_t esp_cpu_clear_breakpoint(int bp_num)
{
    /*
    Todo:
    - Check if the bp_num is valid
    */
#if __XTENSA__
    xt_utils_clear_breakpoint(bp_num);
#else
    if (esp_cpu_dbgr_is_attached()) {
        // See description in esp_cpu_set_breakpoint()
        long args[] = {false, bp_num};
        int ret = semihosting_call_noerrno(ESP_SEMIHOSTING_SYS_BREAKPOINT_SET, args);
        if (ret == 0) {
            return ESP_ERR_INVALID_RESPONSE;
        }
    } else {
        rv_utils_clear_breakpoint(bp_num);
	}
#endif // __XTENSA__
    return ESP_OK;
}
#endif // SOC_CPU_BREAKPOINTS_NUM > 0

#if SOC_CPU_WATCHPOINTS_NUM > 0
esp_err_t esp_cpu_set_watchpoint(int wp_num, const void *wp_addr, size_t size, esp_cpu_watchpoint_trigger_t trigger)
{
    /*
    Todo:
    - Check if the wp_num is already in use
    */
    if (wp_num < 0 || wp_num >= SOC_CPU_WATCHPOINTS_NUM) {
        return ESP_ERR_INVALID_ARG;
    }

    // Check that the watched region's start address is naturally aligned to the size of the region
    if ((uint32_t)wp_addr % size) {
        return ESP_ERR_INVALID_ARG;
    }

    // Check if size is 2^n, and size is in the range of [1 ... SOC_CPU_WATCHPOINT_MAX_REGION_SIZE]
    if (size < 1 || size > SOC_CPU_WATCHPOINT_MAX_REGION_SIZE || (size & (size - 1)) != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    bool on_read = (trigger == ESP_CPU_WATCHPOINT_LOAD || trigger == ESP_CPU_WATCHPOINT_ACCESS);
    bool on_write = (trigger == ESP_CPU_WATCHPOINT_STORE || trigger == ESP_CPU_WATCHPOINT_ACCESS);
#if __XTENSA__
    xt_utils_set_watchpoint(wp_num, (uint32_t)wp_addr, size, on_read, on_write);
#else
    if (esp_cpu_dbgr_is_attached()) {
        // See description in esp_cpu_set_breakpoint()
        long args[] = {true, wp_num, (long)wp_addr, (long)size,
                       (long)((on_read ? ESP_SEMIHOSTING_WP_FLG_RD : 0) | (on_write ? ESP_SEMIHOSTING_WP_FLG_WR : 0))
                      };
        int ret = semihosting_call_noerrno(ESP_SEMIHOSTING_SYS_WATCHPOINT_SET, args);
        if (ret == 0) {
            return ESP_ERR_INVALID_RESPONSE;
        }
    } else {
        rv_utils_set_watchpoint(wp_num, (uint32_t)wp_addr, size, on_read, on_write);
	}
#endif // __XTENSA__
    return ESP_OK;
}

esp_err_t esp_cpu_clear_watchpoint(int wp_num)
{
    /*
    Todo:
    - Check if the wp_num is valid
    */
#if __XTENSA__
    xt_utils_clear_watchpoint(wp_num);
#else
    if (esp_cpu_dbgr_is_attached()) {
        // See description in esp_cpu_dbgr_is_attached()
        long args[] = {false, wp_num};
        int ret = semihosting_call_noerrno(ESP_SEMIHOSTING_SYS_WATCHPOINT_SET, args);
        if (ret == 0) {
            return ESP_ERR_INVALID_RESPONSE;
        }
    } else {
        rv_utils_clear_watchpoint(wp_num);
	}
#endif // __XTENSA__
    return ESP_OK;
}
#endif // SOC_CPU_WATCHPOINTS_NUM > 0

/* ------------------------------------------------------ Misc ---------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

#if __XTENSA__ && XCHAL_HAVE_S32C1I && CONFIG_SPIRAM
static DRAM_ATTR uint32_t external_ram_cas_lock = 0;
#endif

bool esp_cpu_compare_and_set(volatile uint32_t *addr, uint32_t compare_value, uint32_t new_value)
{
#if __XTENSA__
    bool ret;
#if XCHAL_HAVE_S32C1I && CONFIG_SPIRAM
    // Check if the target address is in external RAM
    if ((uint32_t)addr >= SOC_EXTRAM_DATA_LOW && (uint32_t)addr < SOC_EXTRAM_DATA_HIGH) {
        /* The target address is in external RAM, thus the native CAS instruction cannot be used. Instead, we achieve
        atomicity by disabling interrupts and then acquiring an external RAM CAS lock. */
        uint32_t intr_level;
        __asm__ __volatile__ ("rsil %0, " XTSTR(XCHAL_EXCM_LEVEL) "\n"
                              : "=r"(intr_level));
        if (!xt_utils_compare_and_set(&external_ram_cas_lock, 0, 1)) {
            // External RAM CAS lock already taken. Exit
            ret = false;
            goto exit;
        }
        // Now we compare and set the target address
        ret = (*addr == compare_value);
        if (ret) {
            *addr = new_value;
        }
        // Release the external RAM CAS lock
        external_ram_cas_lock = 0;
exit:
        // Reenable interrupts
        __asm__ __volatile__ ("memw \n"
                              "wsr %0, ps\n"
                              :: "r"(intr_level));
    } else
#endif  // XCHAL_HAVE_S32C1I && CONFIG_SPIRAM
    {
        // The target address is in internal RAM. Use the CPU's native CAS instruction
        ret = xt_utils_compare_and_set(addr, compare_value, new_value);
    }
    return ret;
#else // __XTENSA__
    // Single core targets don't have atomic CAS instruction. So access method is the same for internal and external RAM
    return rv_utils_compare_and_set(addr, compare_value, new_value);
#endif
}
