/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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
#elif CONFIG_IDF_TARGET_ESP32P4
#include "soc/lp_clkrst_reg.h"
#include "soc/pmu_reg.h"
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
#if CONFIG_IDF_TARGET_ESP32P4
    //TODO: IDF-7848
    REG_SET_FIELD(PMU_CPU_SW_STALL_REG, core_id ? PMU_HPCORE1_SW_STALL_CODE : PMU_HPCORE0_SW_STALL_CODE, 0x86);
#else
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
#endif // CONFIG_IDF_TARGET_ESP32P4
#endif // SOC_CPU_CORES_NUM > 1
}

void esp_cpu_unstall(int core_id)
{
    assert(core_id >= 0 && core_id < SOC_CPU_CORES_NUM);
#if SOC_CPU_CORES_NUM > 1   // We don't allow stalling of the current core
#if CONFIG_IDF_TARGET_ESP32P4
    //TODO: IDF-7848
    REG_SET_FIELD(PMU_CPU_SW_STALL_REG, core_id ? PMU_HPCORE1_SW_STALL_CODE : PMU_HPCORE0_SW_STALL_CODE, 0);
#else
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
#endif // CONFIG_IDF_TARGET_ESP32P4
#endif // SOC_CPU_CORES_NUM > 1
}

void esp_cpu_reset(int core_id)
{
#if CONFIG_IDF_TARGET_ESP32P4
    //TODO: IDF-7848
    if (core_id == 0)
        REG_SET_BIT(LP_CLKRST_HPCPU_RESET_CTRL0_REG, LP_CLKRST_HPCORE0_SW_RESET);
    else
        REG_SET_BIT(LP_CLKRST_HPCPU_RESET_CTRL0_REG, LP_CLKRST_HPCORE1_SW_RESET);
#else
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
#endif // CONFIG_IDF_TARGET_ESP32P4
}

void esp_cpu_wait_for_intr(void)
{
#if __XTENSA__
    xt_utils_wait_for_intr();
#else
//TODO: IDF-7848
#if !CONFIG_IDF_TARGET_ESP32P4
    // TODO: IDF-5645 (better to implement with ll) C6 register names converted in the #include section at the top
    if (esp_cpu_dbgr_is_attached() && DPORT_REG_GET_BIT(SYSTEM_CPU_PER_CONF_REG, SYSTEM_CPU_WAIT_MODE_FORCE_ON) == 0) {
        /* when SYSTEM_CPU_WAIT_MODE_FORCE_ON is disabled in WFI mode SBA access to memory does not work for debugger,
           so do not enter that mode when debugger is connected */
        return;
    }
#endif
    rv_utils_wait_for_intr();
#endif // __XTENSA__
}

/* -------------------------------------------------- CPU Registers ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------- CPU Interrupts ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// ---------------- Interrupt Descriptors ------------------

#if SOC_CPU_HAS_FLEXIBLE_INTC


#if SOC_INT_CLIC_SUPPORTED

static bool is_intr_num_resv(int ext_intr_num) {
    /* On targets that uses CLIC as the interrupt controller, the first 16 lines (0..15) are reserved for software
     * interrupts, all the other lines starting from 16 and above can be used by external peripheral.
     * in the case of this function, the parameter only refers to the external peripheral index, so if
     * `ext_intr_num` is 0, it refers to interrupt index 16.
     *
     * Only interrupt line 6 is reserved at the moment since it is used for disabling interrupts */
    return ext_intr_num == 6;
}

#else // !SOC_INT_CLIC_SUPPORTED

static bool is_intr_num_resv(int intr_num)
{
    // Workaround to reserve interrupt number 1 for Wi-Fi, 5,8 for Bluetooth, 6 for "permanently disabled interrupt"
    // [TODO: IDF-2465]
    uint32_t reserved = BIT(1) | BIT(5) | BIT(6) | BIT(8);

    // int_num 0,3,4,7 are unavailable for PULP cpu
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2// TODO: IDF-5728 replace with a better macro name
    reserved |= BIT(0) | BIT(3) | BIT(4) | BIT(7);
#endif

    if (reserved & BIT(intr_num)) {
        return true;
    }

    extern int _vector_table;
    extern int _interrupt_handler;
    const intptr_t pc = (intptr_t)(&_vector_table + intr_num);

    /* JAL instructions are relative to the PC there are executed from. */
    const intptr_t destination = pc + riscv_decode_offset_from_jal_instruction(pc);

    return destination != (intptr_t)&_interrupt_handler;
}

#endif // SOC_INT_CLIC_SUPPORTED

void esp_cpu_intr_get_desc(int core_id, int intr_num, esp_cpu_intr_desc_t *intr_desc_ret)
{
    intr_desc_ret->priority = 1;    //Todo: We should make this -1
    intr_desc_ret->type = ESP_CPU_INTR_TYPE_NA;
#if __riscv
    intr_desc_ret->flags = is_intr_num_resv(intr_num) ? ESP_CPU_INTR_DESC_FLAG_RESVD : 0;
#else
    intr_desc_ret->flags = 0;
#endif
}

#else // SOC_CPU_HAS_FLEXIBLE_INTC

typedef struct {
    int priority;
    esp_cpu_intr_type_t type;
    uint32_t flags[SOC_CPU_CORES_NUM];
} intr_desc_t;

#if SOC_CPU_CORES_NUM > 1
// Note: We currently only have dual core targets, so the table initializer is hard coded
const static intr_desc_t intr_desc_table [SOC_CPU_INTR_NUM] = {
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //0
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //1
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } }, //2
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } }, //3
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   0                               } }, //4
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //5
#if CONFIG_FREERTOS_CORETIMER_0
    { 1, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //6
#else
    { 1, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //6
#endif
    { 1, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //7
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //8
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } }, //9
    { 1, ESP_CPU_INTR_TYPE_EDGE,  { 0,                              0                               } }, //10
    { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //11
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0, 0} }, //12
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0, 0} }, //13
    { 7, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //14, NMI
#if CONFIG_FREERTOS_CORETIMER_1
    { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //15
#else
    { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //15
#endif
    { 5, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //16
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } }, //17
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } }, //18
    { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } }, //19
    { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } }, //20
    { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } }, //21
    { 3, ESP_CPU_INTR_TYPE_EDGE,  { ESP_CPU_INTR_DESC_FLAG_RESVD,   0                               } }, //22
    { 3, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              0                               } }, //23
    { 4, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   0                               } }, //24
    { 4, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //25
    { 5, ESP_CPU_INTR_TYPE_LEVEL, { 0,                              ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //26
    { 3, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //27
    { 4, ESP_CPU_INTR_TYPE_EDGE,  { 0,                              0                               } }, //28
    { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL, ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //29
    { 4, ESP_CPU_INTR_TYPE_EDGE,  { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //30
    { 5, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD,   ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //31
};

#else // SOC_CPU_CORES_NUM > 1

const static intr_desc_t intr_desc_table [SOC_CPU_INTR_NUM] = {
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //0
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //1
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //2
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //3
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //4
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //5
#if CONFIG_FREERTOS_CORETIMER_0
    { 1, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //6
#else
    { 1, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //6
#endif
    { 1, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //7
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //8
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //9
    { 1, ESP_CPU_INTR_TYPE_EDGE, { 0                               } },  //10
    { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //11
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //12
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //13
    { 7, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //14, NMI
#if CONFIG_FREERTOS_CORETIMER_1
    { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //15
#else
    { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //15
#endif
    { 5, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //16
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //17
    { 1, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //18
    { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //19
    { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //20
    { 2, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //21
    { 3, ESP_CPU_INTR_TYPE_EDGE,  { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //22
    { 3, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //23
    { 4, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //24
    { 4, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //25
    { 5, ESP_CPU_INTR_TYPE_LEVEL, { 0                               } }, //26
    { 3, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //27
    { 4, ESP_CPU_INTR_TYPE_EDGE,  { 0                               } }, //28
    { 3, ESP_CPU_INTR_TYPE_NA,    { ESP_CPU_INTR_DESC_FLAG_SPECIAL  } }, //29
    { 4, ESP_CPU_INTR_TYPE_EDGE,  { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //30
    { 5, ESP_CPU_INTR_TYPE_LEVEL, { ESP_CPU_INTR_DESC_FLAG_RESVD    } }, //31
};

#endif // SOC_CPU_CORES_NUM > 1

void esp_cpu_intr_get_desc(int core_id, int intr_num, esp_cpu_intr_desc_t *intr_desc_ret)
{
    assert(core_id >= 0 && core_id < SOC_CPU_CORES_NUM);
#if SOC_CPU_CORES_NUM == 1
    core_id = 0; //If this is a single core target, hard code CPU ID to 0
#endif
    intr_desc_ret->priority = intr_desc_table[intr_num].priority;
    intr_desc_ret->type = intr_desc_table[intr_num].type;
    intr_desc_ret->flags = intr_desc_table[intr_num].flags[core_id];
}

#endif // SOC_CPU_HAS_FLEXIBLE_INTC

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

#else // __riscv
    return rv_utils_compare_and_set(addr, compare_value, new_value);
#endif
}
