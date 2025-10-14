/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
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
#include "riscv/csr_clic.h"

#if SOC_INT_CLIC_SUPPORTED

#include "soc/clic_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_SECURE_ENABLE_TEE
#define IS_PRV_M_MODE()  (RV_READ_CSR(CSR_PRV_MODE) == PRV_M)
#else
#define IS_PRV_M_MODE()  (1UL)
#endif


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
    uint32_t threshold;
    if (IS_PRV_M_MODE()) {
        threshold = RV_READ_CSR(MINTTHRESH_CSR);
    } else {
        threshold = RV_READ_CSR(UINTTHRESH_CSR);
    }
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
 * @brief Set the XTVT CSR value (based on the current privilege mode),
 * used as a base address for the interrupt jump table
 */
FORCE_INLINE_ATTR void rv_utils_set_xtvt(uint32_t xtvt_val)
{
    if (IS_PRV_M_MODE()) {
        RV_WRITE_CSR(MTVT_CSR, xtvt_val);
    } else {
        RV_WRITE_CSR(UTVT_CSR, xtvt_val);
    }
}

#if SOC_CPU_SUPPORT_WFE
/**
 * @brief Set the MEXSTATUS_WFFEN value, used to enable/disable wait for event mode.
 */
FORCE_INLINE_ATTR void rv_utils_wfe_mode_enable(bool en)
{
    if (en) {
        RV_SET_CSR(MEXSTATUS, MEXSTATUS_WFFEN);
    } else {
        RV_CLEAR_CSR(MEXSTATUS, MEXSTATUS_WFFEN);
    }
}
#endif

/**
 * @brief Get the current CPU raw interrupt level
 */
FORCE_INLINE_ATTR uint32_t rv_utils_get_interrupt_level_regval(void)
{
    if (IS_PRV_M_MODE()) {
        return RV_READ_CSR(MINTSTATUS_CSR);
    } else {
        return RV_READ_CSR(UINTSTATUS_CSR);
    }
}

/**
 * @brief Get the current CPU interrupt level, between 0 and 2^NLBITS - 1
 */
FORCE_INLINE_ATTR uint32_t rv_utils_get_interrupt_level(void)
{
    uint32_t xintstatus;
    if (IS_PRV_M_MODE()) {
        xintstatus = RV_READ_CSR(MINTSTATUS_CSR);
    } else {
        xintstatus = RV_READ_CSR(UINTSTATUS_CSR);
    }
    /* Extract the level from this field */
    return CLIC_STATUS_TO_INT(xintstatus);
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
    if (IS_PRV_M_MODE()) {
        RV_WRITE_CSR(MINTTHRESH_CSR, restoreval);
    } else {
        RV_WRITE_CSR(UINTTHRESH_CSR, restoreval);
    }
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
    if (IS_PRV_M_MODE()) {
        return RV_SWAP_CSR(MINTTHRESH_CSR, intlevel);
    } else {
        return RV_SWAP_CSR(UINTTHRESH_CSR, intlevel);
    }
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
