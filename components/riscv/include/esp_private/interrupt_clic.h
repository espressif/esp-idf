/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include <stdint.h>
#include <assert.h>
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "riscv/csr.h"

#if SOC_INT_CLIC_SUPPORTED

#include "soc/clic_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set global masking level. When using the CLIC, all interrupt priority levels less than or equal to the threshold
 * level are masked. Since the default priority for the interrupt is 1, set this macro to 0 to enable them all.
 */
#define RVHAL_INTR_ENABLE_THRESH    0

/**
 * @brief Bitmask to enable the vector mode when writing MTVEC CSR.
 * Setting mode field to 3 treats `MTVT + 4 * interrupt_id` as the service entry address for HW vectored interrupts.
 */
#define MTVEC_MODE_CSR          3

/**
 * If the target is using the CLIC as the interrupt controller, we have 32 external interrupt lines and 16 internal
 * lines. Let's consider the internal ones reserved and not mappable to any handler.
 */
#define RV_EXTERNAL_INT_COUNT   32
#define RV_EXTERNAL_INT_OFFSET  16

/**
 * @brief CSR to set the interrupt jump table address is MTVT.
 */
#define MTVT_CSR    0x307


#if CONFIG_IDF_TARGET_ESP32P4

/**
 * The ESP32-P4 and the beta version of the ESP32-C5 implement a non-standard version of the CLIC:
 *  - The interrupt threshold is configured via a memory-mapped register instead of a CSR
 *  - The mintstatus CSR is at 0x346 instead of 0xFB1 as per the official specification
 */
#define INTTHRESH_STANDARD  0
#define MINTSTATUS_CSR      0x346

#elif CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C61

/* The ESP32-C5 (MP) and C61 use the standard CLIC specification, for example, it defines the mintthresh CSR */
#define INTTHRESH_STANDARD  1
#define MINTSTATUS_CSR      0xFB1
#define MINTTHRESH_CSR      0x347

#else
    #error "Check the implementation of the CLIC on this target."
#endif

/**
 * @brief Convert a priority level from 8-bit to NLBITS and NLBITS to 8-bit
 *
 * On CLIC, the interrupt threshold is stored in the upper (NLBITS) of the mintthresh register, with the other (8 - NLBITS)
 * defaulted to 1. We form the interrupt level bits here to avoid doing this at run time
 */
#define NLBITS_SHIFT            (8 - NLBITS)
#define NLBITS_MASK             ((1 << NLBITS) - 1)
#define BYTE_TO_NLBITS(level)   (((level) >> NLBITS_SHIFT) & NLBITS_MASK)
/* Align the level to the left, and put 1 in the lowest bits */
#define NLBITS_TO_BYTE(level)   (((level) << NLBITS_SHIFT) | ((1 << NLBITS_SHIFT) - 1))

/**
 * @brief In the minstatus CSR, the `mil` field is present from bit 24 to bit 31 (included)
 */
#define MINTSTATUS_MIL_S        24
#define MINTSTATUS_MIL_V        0xFF


#if INTTHRESH_STANDARD
    /* Helper macro to translate absolute interrupt level to CLIC interrupt threshold bits in the mintthresh reg */
    #define CLIC_INT_THRESH(intlevel)       (NLBITS_TO_BYTE(intlevel))

    /* Helper macro to translate a CLIC interrupt threshold bits to an absolute interrupt level */
    #define CLIC_THRESH_TO_INT(intlevel)    (BYTE_TO_NLBITS((intlevel)))

    /* Helper macro to translate a CLIC status threshold bits to an absolute interrupt level */
    #define CLIC_STATUS_TO_INT(status)      (BYTE_TO_NLBITS((status >> MINTSTATUS_MIL_S) & MINTSTATUS_MIL_V))
#else
    /* For the non-standard intthresh implementation the threshold is stored in the upper 8 bits of CLIC_CPU_INT_THRESH reg */
    /* Helper macro to translate absolute interrupt level to CLIC interrupt threshold bits in the mintthresh reg */
    #define CLIC_INT_THRESH(intlevel)       (NLBITS_TO_BYTE(intlevel) << CLIC_CPU_INT_THRESH_S)

    /* Helper macro to translate a CLIC interrupt threshold bits to an absolute interrupt level */
    #define CLIC_THRESH_TO_INT(intlevel)    (BYTE_TO_NLBITS((intlevel >> CLIC_CPU_INT_THRESH_S) & CLIC_CPU_INT_THRESH_V))

    /* Helper macro to translate a CLIC status threshold bits to an absolute interrupt level */
    #define CLIC_STATUS_TO_INT(status)      (BYTE_TO_NLBITS((status >> MINTSTATUS_MIL_S) & MINTSTATUS_MIL_V))
#endif //INTTHRESH_STANDARD

/* Helper macro to set interrupt level RVHAL_EXCM_LEVEL. Used during critical sections */
#define RVHAL_EXCM_LEVEL_CLIC           (CLIC_INT_THRESH(RVHAL_EXCM_LEVEL - 1))

/* Helper macro to enable interrupts. */
#define RVHAL_INTR_ENABLE_THRESH_CLIC   (CLIC_INT_THRESH(RVHAL_INTR_ENABLE_THRESH))





FORCE_INLINE_ATTR void assert_valid_rv_int_num(int rv_int_num)
{
    assert(rv_int_num < RV_EXTERNAL_INT_COUNT && "Invalid CPU interrupt number");
}

/* -------------------------------------------------------------------------- */
/*                  Interrupt Configuration part of rv_utils                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Get the current CPU threshold level
 */
FORCE_INLINE_ATTR uint32_t rv_utils_get_interrupt_threshold(void)
{
#if INTTHRESH_STANDARD
    uint32_t threshold = RV_READ_CSR(MINTTHRESH_CSR);
#else
    uint32_t threshold = REG_READ(CLIC_INT_THRESH_REG);
#endif
    return CLIC_THRESH_TO_INT(threshold);
}


/**
 * @brief Set the MTVT CSR value, used as a base address for the interrupt jump table
 */
FORCE_INLINE_ATTR void rv_utils_set_mtvt(uint32_t mtvt_val)
{
    RV_WRITE_CSR(MTVT_CSR, mtvt_val);
}

/**
 * @brief Get the current CPU raw interrupt level
 */
FORCE_INLINE_ATTR uint32_t rv_utils_get_interrupt_level_regval(void)
{
    return RV_READ_CSR(MINTSTATUS_CSR);
}

/**
 * @brief Get the current CPU interrupt level, between 0 and 2^NLBITS - 1
 */
FORCE_INLINE_ATTR uint32_t rv_utils_get_interrupt_level(void)
{
    const uint32_t mintstatus = RV_READ_CSR(MINTSTATUS_CSR);
    /* Extract the level from this field */
    return CLIC_STATUS_TO_INT(mintstatus);
}

/**
 * @brief Restore the CPU interrupt level to the value returned by `rv_utils_set_intlevel_regval`.
 *
 * @note This function doesn't process its parameter, it is therefore faster than its counterpart, `rv_utils_restore_intlevel`, and should be used in places where speed is important (spinlocks, critical sections, ...)
 *
 * @param restoreval Former raw interrupt level, it is NOT necessarily a value between 0 and 7, this is hardware and configuration dependent.
 */
FORCE_INLINE_ATTR void rv_utils_restore_intlevel_regval(uint32_t restoreval)
{
#if INTTHRESH_STANDARD
    RV_WRITE_CSR(MINTTHRESH_CSR, restoreval);
#else
    REG_WRITE(CLIC_INT_THRESH_REG, restoreval);
    /**
     * After writing the threshold register, the new threshold is not directly taken into account by the CPU.
     * By executing ~8 nop instructions, or by performing a memory load right now, the previous memory write
     * operations is forced, making the new threshold active. It is then safe to re-enable MIE bit in mstatus.
     */
    REG_READ(CLIC_INT_THRESH_REG);
#endif
}

/**
 * @brief Restore the CPU interrupt level to the given priority.
 *
 * @note On the CLIC, the interrupt level mask is inclusive, so this function would mask all interrupts between 0 and `restoreval`.
 */
FORCE_INLINE_ATTR void rv_utils_restore_intlevel(uint32_t restoreval)
{
    rv_utils_restore_intlevel_regval(CLIC_INT_THRESH(restoreval));
}

/**
 * @brief Set the interrupt threshold to `intlevel` while getting the current level.
 *
 * @note This function doesn't process its parameter nor the returned value, it is therefore faster than its counterpart, `rv_utils_set_intlevel`, and should be used in places where speed is important (spinlocks, critical sections, ...)
 *
 * @param intlevel New raw interrupt level, it is NOT necessarily a value between 0 and 7, this is hardware and configuration dependent.
 *
 * @return Current raw interrupt level, can be restored by calling `rv_utils_restore_intlevel_regval`.
 */
FORCE_INLINE_ATTR uint32_t rv_utils_set_intlevel_regval(uint32_t intlevel)
{
#if INTTHRESH_STANDARD
    return RV_SWAP_CSR(MINTTHRESH_CSR, intlevel);
#else // !INTTHRESH_STANDARD
    uint32_t old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    uint32_t old_thresh = REG_READ(CLIC_INT_THRESH_REG);

    /* This function expects the interrupt level to be available in the format needed for mintthresh reg.
     * Providing an absolute interrupt level will result in incorrect behavior.
     * See CLIC_INT_THRESH() macro for details of how the interrupt level must be provided. */
    rv_utils_restore_intlevel_regval(intlevel);

    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);

    /* We return the mintthresh register value and NOT the absolute interrupt threshold level.
     * This is done to avoid extra bit manipulations during critical sections. */
    return old_thresh;
#endif // INTTHRESH_STANDARD
}

FORCE_INLINE_ATTR uint32_t rv_utils_set_intlevel(uint32_t intlevel)
{
    const uint32_t former = rv_utils_set_intlevel_regval(CLIC_INT_THRESH(intlevel));
    return CLIC_THRESH_TO_INT(former);
}

FORCE_INLINE_ATTR uint32_t rv_utils_mask_int_level_lower_than(uint32_t intlevel)
{
    /* CLIC's set interrupt level is inclusive, i.e. it DOES mask the level set.
     * For example, if we want to mask interrupt level lower than 3, we have to set the intlevel to 2. */
    return rv_utils_set_intlevel(intlevel - 1);
}

/**
 * @brief Get the enabled interrupts on the current CPU.
 *
 * @return Bit mask of the enabled interrupts
 */
FORCE_INLINE_ATTR uint32_t rv_utils_intr_get_enabled_mask(void)
{
    unsigned intr_ena_mask = 0;
    unsigned intr_num;
    for (intr_num = 0; intr_num < 32; intr_num++) {
        if (REG_GET_BIT(CLIC_INT_CTRL_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET), CLIC_INT_IE))
            intr_ena_mask |= BIT(intr_num);
    }
    return intr_ena_mask;
}

/**
 * @brief Acknowledge an edge interrupt
 *
 * @param intr_num Interrupt number (from 0 to 31)
 */
FORCE_INLINE_ATTR void rv_utils_intr_edge_ack(uint32_t intr_num)
{
    REG_SET_BIT(CLIC_INT_CTRL_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET) , CLIC_INT_IP);
}

#ifdef __cplusplus
}
#endif

#endif /* SOC_INT_CLIC_SUPPORTED */
