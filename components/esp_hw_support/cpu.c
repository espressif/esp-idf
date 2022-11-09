/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdint.h>
#include <assert.h>
#include "soc/soc.h"
#include "soc/soc_caps.h"

// TODO: IDF-5645
#if CONFIG_IDF_TARGET_ESP32C6
#include "soc/lp_aon_reg.h"
#include "soc/pcr_reg.h"
#define SYSTEM_CPU_PER_CONF_REG PCR_CPU_WAITI_CONF_REG
#define SYSTEM_CPU_WAIT_MODE_FORCE_ON PCR_CPU_WAIT_MODE_FORCE_ON
#else
#include "soc/rtc_cntl_reg.h"
#endif

#include "hal/soc_hal.h"
#include "hal/mpu_hal.h"
#include "esp_bit_defs.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_cpu.h"
#include "esp_memory_utils.h"
#include "esp_fault.h"
#if __XTENSA__
#include "xtensa/config/core-isa.h"
#else
#include "soc/system_reg.h"     // For SYSTEM_CPU_PER_CONF_REG
#include "soc/dport_access.h"   // For Dport access
#include "riscv/semihosting.h"
#include "riscv/csr.h"      // For PMP_ENTRY. [refactor-todo] create PMP abstraction in rv_utils.h
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
#if CONFIG_IDF_TARGET_ESP32C6 // TODO: IDF-5645
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

/* -------------------------------------------------- CPU Registers ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------- CPU Interrupts ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// ---------------- Interrupt Descriptors ------------------

#if SOC_CPU_HAS_FLEXIBLE_INTC

static bool is_intr_num_resv(int intr_num)
{
    // Workaround to reserve interrupt number 1 for Wi-Fi, 5,8 for Bluetooth, 6 for "permanently disabled interrupt"
    // [TODO: IDF-2465]
    uint32_t reserved = BIT(1) | BIT(5) | BIT(6) | BIT(8);

    // int_num 0,3,4,7 are inavaliable for PULP cpu
#if CONFIG_IDF_TARGET_ESP32C6 // TODO: IDF-5728 replace with a better macro name
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

/* -------------------------------------------------- Memory Ports -----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
void esp_cpu_configure_region_protection(void)
{
    /* Note: currently this is configured the same on all Xtensa targets
     *
     * Both chips have the address space divided into 8 regions, 512MB each.
     */
    const int illegal_regions[] = {0, 4, 5, 6, 7}; // 0x00000000, 0x80000000, 0xa0000000, 0xc0000000, 0xe0000000
    for (size_t i = 0; i < sizeof(illegal_regions) / sizeof(illegal_regions[0]); ++i) {
        mpu_hal_set_region_access(illegal_regions[i], MPU_REGION_ILLEGAL);
    }

    mpu_hal_set_region_access(1, MPU_REGION_RW); // 0x20000000
}
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H4
void esp_cpu_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) Note: ESP32-C3/H4 CPU doesn't support overlapping PMP regions
     *
     * 2) Therefore, we use TOR (top of range) entries to map the whole address
     * space, bottom to top.
     *
     * 3) There are not enough entries to describe all the memory regions 100% accurately.
     *
     * 4) This means some gaps (invalid memory) are accessible. Priority for extending regions
     * to cover gaps is to extend read-only or read-execute regions or read-only regions only
     * (executing unmapped addresses should always fault with invalid instruction, read-only means
     * stores will correctly fault even if reads may return some invalid value.)
     *
     * 5) Entries are grouped in order with some static asserts to try and verify everything is
     * correct.
     */
    const unsigned NONE = PMP_L | PMP_TOR;
    const unsigned R       = PMP_L | PMP_TOR | PMP_R;
    const unsigned RW      = PMP_L | PMP_TOR | PMP_R | PMP_W;
    const unsigned RX      = PMP_L | PMP_TOR | PMP_R | PMP_X;
    const unsigned RWX     = PMP_L | PMP_TOR | PMP_R | PMP_W | PMP_X;

    // 1. Gap at bottom of address space
    PMP_ENTRY_SET(0, SOC_DEBUG_LOW, NONE);

    // 2. Debug region
    PMP_ENTRY_SET(1, SOC_DEBUG_HIGH, RWX);
    _Static_assert(SOC_DEBUG_LOW < SOC_DEBUG_HIGH, "Invalid CPU debug region");

    // 3. Gap between debug region & DROM (flash cache)
    PMP_ENTRY_SET(2, SOC_DROM_LOW, NONE);
    _Static_assert(SOC_DEBUG_HIGH < SOC_DROM_LOW, "Invalid PMP entry order");

    // 4. DROM (flash cache)
    // 5. Gap between DROM & DRAM
    // (Note: To save PMP entries these two are merged into one read-only region)
    PMP_ENTRY_SET(3, SOC_DRAM_LOW, R);
    _Static_assert(SOC_DROM_LOW < SOC_DROM_HIGH, "Invalid DROM region");
    _Static_assert(SOC_DROM_HIGH < SOC_DRAM_LOW, "Invalid PMP entry order");

    // 6. DRAM
    PMP_ENTRY_SET(4, SOC_DRAM_HIGH, RW);
    _Static_assert(SOC_DRAM_LOW < SOC_DRAM_HIGH, "Invalid DRAM region");

    // 7. Gap between DRAM and Mask DROM
    // 8. Mask DROM
    // (Note: to save PMP entries these two are merged into one read-only region)
    PMP_ENTRY_SET(5, SOC_DROM_MASK_HIGH, R);
    _Static_assert(SOC_DRAM_HIGH < SOC_DROM_MASK_LOW, "Invalid PMP entry order");
    _Static_assert(SOC_DROM_MASK_LOW < SOC_DROM_MASK_HIGH, "Invalid mask DROM region");

    // 9. Gap between mask DROM and mask IROM
    // 10. Mask IROM
    // (Note: to save PMP entries these two are merged into one RX region)
    PMP_ENTRY_SET(6, SOC_IROM_MASK_HIGH, RX);
    _Static_assert(SOC_DROM_MASK_HIGH < SOC_IROM_MASK_LOW, "Invalid PMP entry order");
    _Static_assert(SOC_IROM_MASK_LOW < SOC_IROM_MASK_HIGH, "Invalid mask IROM region");

    // 11. Gap between mask IROM & IRAM
    PMP_ENTRY_SET(7, SOC_IRAM_LOW, NONE);
    _Static_assert(SOC_IROM_MASK_HIGH < SOC_IRAM_LOW, "Invalid PMP entry order");

    // 12. IRAM
    PMP_ENTRY_SET(8, SOC_IRAM_HIGH, RWX);
    _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid IRAM region");

    // 13. Gap between IRAM and IROM
    // 14. IROM (flash cache)
    // (Note: to save PMP entries these two are merged into one RX region)
    PMP_ENTRY_SET(9, SOC_IROM_HIGH, RX);
    _Static_assert(SOC_IRAM_HIGH < SOC_IROM_LOW, "Invalid PMP entry order");
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid IROM region");

    // 15. Gap between IROM & RTC slow memory
    PMP_ENTRY_SET(10, SOC_RTC_IRAM_LOW, NONE);
    _Static_assert(SOC_IROM_HIGH < SOC_RTC_IRAM_LOW, "Invalid PMP entry order");

    // 16. RTC fast memory
    PMP_ENTRY_SET(11, SOC_RTC_IRAM_HIGH, RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");

    // 17. Gap between RTC fast memory & peripheral addresses
    PMP_ENTRY_SET(12, SOC_PERIPHERAL_LOW, NONE);
    _Static_assert(SOC_RTC_IRAM_HIGH < SOC_PERIPHERAL_LOW, "Invalid PMP entry order");

    // 18. Peripheral addresses
    PMP_ENTRY_SET(13, SOC_PERIPHERAL_HIGH, RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");

    // 19. End of address space
    PMP_ENTRY_SET(14, UINT32_MAX, NONE); // all but last 4 bytes
    PMP_ENTRY_SET(15, UINT32_MAX, PMP_L | PMP_NA4);  // last 4 bytes
}
#elif CONFIG_IDF_TARGET_ESP32C2
#if CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT && !BOOTLOADER_BUILD
extern int _iram_end;
extern int _data_start;
#define IRAM_END        (int)&_iram_end
#define DRAM_START      (int)&_data_start
#else
#define IRAM_END        SOC_DIRAM_IRAM_HIGH
#define DRAM_START      SOC_DIRAM_DRAM_LOW
#endif

#ifdef BOOTLOADER_BUILD
// Without L bit set
#define CONDITIONAL_NONE        0x0
#define CONDITIONAL_RX          PMP_R | PMP_X
#define CONDITIONAL_RW          PMP_R | PMP_W
#else
// With L bit set
#define CONDITIONAL_NONE        NONE
#define CONDITIONAL_RX          RX
#define CONDITIONAL_RW          RW
#endif

void esp_cpu_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) ESP32-C2 CPU support overlapping PMP regions, configuration is based on static priority
     * feature(lowest numbered entry has highest priority).
     *
     * 2) Therefore, we use TOR (top of range) and NAOPT entries to map the effective area.
     * Finally, define any address without access permission.
     *
     * 3) 3-15 PMPADDR entries be hardcoded to fixed value, 0-2 PMPADDR be programmed to split ID SRAM
     * as IRAM/DRAM. All PMPCFG entryies be available.
     *
     * 4) Ideally, PMPADDR 0-2 entries should be configured twice, once during bootloader startup and another during app startup.
     *    However, the CPU currently always executes in machine mode and to enforce these permissions in machine mode, we need
     *    to set the Lock (L) bit but if set once, it cannot be reconfigured. So, we only configure 0-2 PMPADDR during app startup.
     */
    const unsigned NONE    = PMP_L ;
    const unsigned R       = PMP_L | PMP_R;
    const unsigned X       = PMP_L | PMP_X;
    const unsigned RW      = PMP_L | PMP_R | PMP_W;
    const unsigned RX      = PMP_L | PMP_R | PMP_X;
    const unsigned RWX     = PMP_L | PMP_R | PMP_W | PMP_X;

    /* There are 4 configuration scenarios for PMPADDR 0-2
     *
     * 1. Bootloader build:
     *    - We cannot set the lock bit as we need to reconfigure it again for the application.
     *      We configure PMPADDR 0-1 to cover entire valid IRAM range and PMPADDR 2-3 to cover entire valid DRAM range.
     *
     * 2. Application build with CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT enabled
     *    - We split the SRAM into IRAM and DRAM such that IRAM region cannot be accessed via DBUS
     *      and DRAM region cannot be accessed via IBUS. We use _iram_end and _data_start markers to set the boundaries.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     *
     * 3. Application build with CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT disabled
     *    - The IRAM-DRAM split is not enabled so we just need to ensure that access to only valid address ranges are successful
     *      so for that we set PMPADDR 0-1 to cover entire valid IRAM range and PMPADDR 2-3 to cover entire DRAM region.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     *
     * 4. CPU is in OCD debug mode
     *    - The IRAM-DRAM split is not enabled so that OpenOCD can write and execute from IRAM.
     *      We set PMPADDR 0-1 to cover entire valid IRAM range and PMPADDR 2-3 to cover entire DRAM region.
     *      We also lock these entries so the R/W/X permissions are enforced even for machine mode
     *
     *  PMPADDR 3-15 are hard-coded and are appicable to both, bootloader and application. So we configure and lock
     *  these during BOOTLOADER build itself. During application build, reconfiguration of these PMPADDR entries
     *  are silently ignored by the CPU
     */

    if (esp_cpu_dbgr_is_attached()) {
        // Anti-FI check that cpu is really in ocd mode
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());

        // 1. IRAM
        PMP_ENTRY_SET(0, SOC_DIRAM_IRAM_LOW, NONE);
        PMP_ENTRY_SET(1, SOC_DIRAM_IRAM_HIGH, PMP_TOR | RWX);

        // 2. DRAM
        PMP_ENTRY_SET(2, SOC_DIRAM_DRAM_LOW, NONE);
        PMP_ENTRY_CFG_SET(3, PMP_TOR | RW);
    } else {
        // 1. IRAM
        PMP_ENTRY_SET(0, SOC_DIRAM_IRAM_LOW, CONDITIONAL_NONE);
        PMP_ENTRY_SET(1, IRAM_END, PMP_TOR | CONDITIONAL_RX);

        // 2. DRAM
        PMP_ENTRY_SET(2, DRAM_START, CONDITIONAL_NONE);
        PMP_ENTRY_CFG_SET(3, PMP_TOR | CONDITIONAL_RW);
    }

    // 3. Debug region
    PMP_ENTRY_CFG_SET(4, PMP_NAPOT | RWX);

    // 4. DROM (flash dcache)
    PMP_ENTRY_CFG_SET(5, PMP_NAPOT | R);

    // 5. DROM_MASK
    PMP_ENTRY_CFG_SET(6, NONE);
    PMP_ENTRY_CFG_SET(7, PMP_TOR | R);

    // 6. IROM_MASK
    PMP_ENTRY_CFG_SET(8, NONE);
    PMP_ENTRY_CFG_SET(9, PMP_TOR | RX);

    // 7. IROM (flash icache)
    PMP_ENTRY_CFG_SET(10, PMP_NAPOT | RX);

    // 8. Peripheral addresses
    PMP_ENTRY_CFG_SET(11, PMP_NAPOT | RW);

    // 9. SRAM (used as ICache)
    PMP_ENTRY_CFG_SET(12, PMP_NAPOT | X);

    // 10. no access to any address below(0x0-0xFFFF_FFFF)
    PMP_ENTRY_CFG_SET(13, PMP_NA4 | NONE);// last 4 bytes(0xFFFFFFFC)
    PMP_ENTRY_CFG_SET(14, NONE);
    PMP_ENTRY_CFG_SET(15, PMP_TOR | NONE);
}
#elif CONFIG_IDF_TARGET_ESP32C6 // TODO: IDF-5642
void esp_cpu_configure_region_protection(void)
{
    /* Notes on implementation:
     *
     * 1) Note: ESP32-C6 CPU doesn't support overlapping PMP regions
     *
     * 2) Therefore, we use TOR (top of range) entries to map the whole address
     * space, bottom to top.
     *
     * 3) There are not enough entries to describe all the memory regions 100% accurately.
     *
     * 4) This means some gaps (invalid memory) are accessible. Priority for extending regions
     * to cover gaps is to extend read-only or read-execute regions or read-only regions only
     * (executing unmapped addresses should always fault with invalid instruction, read-only means
     * stores will correctly fault even if reads may return some invalid value.)
     *
     * 5) Entries are grouped in order with some static asserts to try and verify everything is
     * correct.
     */
    const unsigned NONE = PMP_L | PMP_TOR;
    const unsigned RW      = PMP_L | PMP_TOR | PMP_R | PMP_W;
    const unsigned RX      = PMP_L | PMP_TOR | PMP_R | PMP_X;
    const unsigned RWX     = PMP_L | PMP_TOR | PMP_R | PMP_W | PMP_X;

    // 1. Gap at bottom of address space
    PMP_ENTRY_SET(0, SOC_DEBUG_LOW, NONE);

    // 2. Debug region
    PMP_ENTRY_SET(1, SOC_DEBUG_HIGH, RWX);
    _Static_assert(SOC_DEBUG_LOW < SOC_DEBUG_HIGH, "Invalid CPU debug region");

    // 3. Gap between debug region & IROM
    PMP_ENTRY_SET(2, SOC_IROM_MASK_LOW, NONE);
    _Static_assert(SOC_DEBUG_HIGH < SOC_IROM_MASK_LOW, "Invalid PMP entry order");

    // 4. ROM
    PMP_ENTRY_SET(3, SOC_DROM_MASK_HIGH, RX);
    _Static_assert(SOC_IROM_MASK_LOW < SOC_DROM_MASK_HIGH, "Invalid ROM region");

    // 5. Gap between ROM & RAM
    PMP_ENTRY_SET(4, SOC_IRAM_LOW, NONE);
    _Static_assert(SOC_DROM_MASK_HIGH < SOC_IRAM_LOW, "Invalid PMP entry order");

    // 6. RAM
    PMP_ENTRY_SET(5, SOC_IRAM_HIGH, RWX);
    _Static_assert(SOC_IRAM_LOW < SOC_IRAM_HIGH, "Invalid RAM region");

    // 7. Gap between DRAM and I_Cache
    PMP_ENTRY_SET(6, SOC_IROM_LOW, NONE);
    _Static_assert(SOC_IRAM_HIGH < SOC_IROM_LOW, "Invalid PMP entry order");

    // 8. I_Cache (flash)
    PMP_ENTRY_SET(7, SOC_IROM_HIGH, RWX);
    _Static_assert(SOC_IROM_LOW < SOC_IROM_HIGH, "Invalid I_Cache region");

    // 9. D_Cache (flash)
    PMP_ENTRY_SET(8, SOC_DROM_HIGH, RW);
    _Static_assert(SOC_DROM_LOW < SOC_DROM_HIGH, "Invalid D_Cache region");

    // 10. Gap between D_Cache & LP_RAM
    PMP_ENTRY_SET(9, SOC_RTC_IRAM_LOW, NONE);
    _Static_assert(SOC_DROM_HIGH < SOC_RTC_IRAM_LOW, "Invalid PMP entry order");

    // 16. LP memory
    PMP_ENTRY_SET(10, SOC_RTC_IRAM_HIGH, RWX);
    _Static_assert(SOC_RTC_IRAM_LOW < SOC_RTC_IRAM_HIGH, "Invalid RTC IRAM region");

    // 17. Gap between LP memory & peripheral addresses
    PMP_ENTRY_SET(11, SOC_PERIPHERAL_LOW, NONE);
    _Static_assert(SOC_RTC_IRAM_HIGH < SOC_PERIPHERAL_LOW, "Invalid PMP entry order");

    // 18. Peripheral addresses
    PMP_ENTRY_SET(12, SOC_PERIPHERAL_HIGH, RW);
    _Static_assert(SOC_PERIPHERAL_LOW < SOC_PERIPHERAL_HIGH, "Invalid peripheral region");

    // 19. End of address space
    PMP_ENTRY_SET(13, UINT32_MAX, NONE); // all but last 4 bytes
    PMP_ENTRY_SET(14, UINT32_MAX, PMP_L | PMP_NA4);  // last 4 bytes
}
#endif

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
    }
    rv_utils_set_breakpoint(bp_num, (uint32_t)bp_addr);
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
    }
    rv_utils_clear_breakpoint(bp_num);
#endif // __XTENSA__
    return ESP_OK;
}
#endif // SOC_CPU_BREAKPOINTS_NUM > 0

#if SOC_CPU_WATCHPOINTS_NUM > 0
esp_err_t esp_cpu_set_watchpoint(int wp_num, const void *wp_addr, size_t size, esp_cpu_watchpoint_trigger_t trigger)
{
    /*
    Todo:
    - Check that wp_num is in range
    - Check if the wp_num is already in use
    */
    // Check if size is 2^n, where n is in [0...6]
    if (size < 1 || size > 64 || (size & (size - 1)) != 0) {
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
    }
    rv_utils_set_watchpoint(wp_num, (uint32_t)wp_addr, size, on_read, on_write);
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
    }
    rv_utils_clear_watchpoint(wp_num);
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
