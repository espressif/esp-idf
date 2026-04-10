/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-P4 LP Mailbox register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/lp_mailbox_struct.h"
#include "soc/lp_mailbox_reg.h"
#include "hal/misc.h"
#include "esp_attr.h"

#define LP_MAILBOX_LL_MSG_COUNT     16U

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Get a message (32-bit value) from the LP mailbox.
 *
 * @param dev Pointer to the LP mailbox device structure.
 * @param index Index of the message to retrieve (must be less than LP_MAILBOX_LL_MSG_COUNT).
 *
 * @return The 32-bit message value at the specified index, or 0 if the index is out of range.
 */
FORCE_INLINE_ATTR uint32_t lp_mailbox_ll_get_message(lp_mb_dev_t *dev, int index)
{
    if (index < LP_MAILBOX_LL_MSG_COUNT) {
        return (&dev->message_0.val)[index];
    }
    return 0;
}

/**
 * @brief Set a message in the LP mailbox.
 *
 * @note Writing a message in the mailbox will set the corresponding message's intr_raw bit for
 * both the LP and HP registers, regardless of the writer!
 *
 * @param dev Pointer to the LP mailbox device structure.
 * @param index Index of the message to set (must be less than LP_MAILBOX_LL_MSG_COUNT).
 * @param val Message (32-bit value) to write to the specified message index.
 */
FORCE_INLINE_ATTR void lp_mailbox_ll_set_message(lp_mb_dev_t *dev, int index, uint32_t val)
{
    if (index < LP_MAILBOX_LL_MSG_COUNT) {
        (&dev->message_0.val)[index] = val;
    }
}

/**
 * @brief Get the raw status of the LP core interrupt register.
 *
 * @param dev Pointer to the LP mailbox device structure.
 *
 * @return Raw interrupt status value.
 */
FORCE_INLINE_ATTR uint32_t lp_mailbox_ll_get_lp_intr_raw(lp_mb_dev_t *dev)
{
    return dev->lp_int_raw.val;
}

/**
 * @brief Clear LP core interrupt register bits.
 *
 * @param dev Pointer to the LP mailbox device structure.
 * @param mask Bitmask of interrupts to clear, bit `i` represents message `i`.
 */
FORCE_INLINE_ATTR void lp_mailbox_ll_lp_intr_clear(lp_mb_dev_t *dev, uint32_t mask)
{
    dev->lp_int_clr.val = mask;
}

/**
 * @brief Get the LP core interrupt register status.
 *
 * @param dev Pointer to the LP mailbox device structure.
 *
 * @return Interrupt status value.
 */
FORCE_INLINE_ATTR uint32_t lp_mailbox_ll_lp_intr_status(lp_mb_dev_t *dev)
{
    return dev->lp_int_st.val;
}

/**
 * @brief Enable mailbox interrupts by mask for the LP core.
 *
 * @param dev Pointer to the LP mailbox device structure.
 * @param mask Bitmask of interrupts to enable, bit `i` represents message `i`.
 *
 * @return Updated interrupt enable register value.
 */
FORCE_INLINE_ATTR uint32_t lp_mailbox_ll_lp_intr_enable_mask(lp_mb_dev_t *dev, uint32_t mask)
{
    dev->lp_int_ena.val |= mask;
    return dev->lp_int_ena.val;
}

/**
 * @brief Disable LP core mailbox interrupts by mask.
 *
 * @param dev Pointer to the LP mailbox device structure.
 * @param mask Bitmask of interrupts to disable, bit `i` represents message `i`.
 *
 * @return Updated interrupt enable register value.
 */

FORCE_INLINE_ATTR uint32_t lp_mailbox_ll_lp_intr_disable_mask(lp_mb_dev_t *dev, uint32_t mask)
{
    dev->lp_int_ena.val &= ~mask;
    return dev->lp_int_ena.val;
}

/**
 * @brief Get the raw status of the HP core interrupt register.
 *
 * @param dev Pointer to the LP mailbox device structure.
 *
 * @return Raw interrupt status value.
 */
FORCE_INLINE_ATTR uint32_t lp_mailbox_ll_get_hp_intr_raw(lp_mb_dev_t *dev)
{
    return dev->hp_int_raw.val;
}

/**
 * @brief Clear HP core interrupt register bits.
 *
 * @param dev Pointer to the LP mailbox device structure.
 * @param mask Bitmask of interrupts to clear, bit `i` represents message `i`.
 */
FORCE_INLINE_ATTR void lp_mailbox_ll_hp_intr_clear(lp_mb_dev_t *dev, uint32_t mask)
{
    dev->hp_int_clr.val = mask;
}

/**
 * @brief Get the HP core interrupt register status.
 *
 * @param dev Pointer to the LP mailbox device structure.
 *
 * @return Interrupt status value.
 */
FORCE_INLINE_ATTR uint32_t lp_mailbox_ll_hp_intr_status(lp_mb_dev_t *dev)
{
    return dev->hp_int_st.val;
}

/**
 * @brief Enable mailbox interrupts by mask for the HP core.
 *
 * @param dev Pointer to the LP mailbox device structure.
 * @param mask Bitmask of interrupts to enable, bit `i` represents message `i`.
 *
 * @return Updated interrupt enable register value.
 */
FORCE_INLINE_ATTR uint32_t lp_mailbox_ll_hp_intr_enable_mask(lp_mb_dev_t *dev, uint32_t mask)
{
    dev->hp_int_ena.val |= mask;
    return dev->hp_int_ena.val;
}

/**
 * @brief Disable HP core mailbox interrupts by mask.
 *
 * @param dev Pointer to the LP mailbox device structure.
 * @param mask Bitmask of interrupts to disable, bit `i` represents message `i`.
 *
 * @return Updated interrupt enable register value.
 */
FORCE_INLINE_ATTR uint32_t lp_mailbox_ll_hp_intr_disable_mask(lp_mb_dev_t *dev, uint32_t mask)
{
    dev->hp_int_ena.val &= ~mask;
    return dev->hp_int_ena.val;
}

#ifdef __cplusplus
}
#endif
