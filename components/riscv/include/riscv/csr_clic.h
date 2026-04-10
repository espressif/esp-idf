/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/interrupt_reg.h"
#include "soc/soc_caps.h"

#if SOC_INT_CLIC_SUPPORTED
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
#define UTVT_CSR    0x007

#if INTTHRESH_STANDARD

/* The ESP32-C5 (MP), C61, H4 and P4 (since REV2) use the standard CLIC specification, for example, it defines the mintthresh CSR */
#define MINTSTATUS_CSR      0xFB1
#define UINTSTATUS_CSR      0xCB1
#define MINTTHRESH_CSR      0x347
#define UINTTHRESH_CSR      0x047

#elif !defined(MINTSTATUS_CSR)
    #error "Non-standard CLIC CSRs must be defined for the current target!"
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

#endif /* SOC_INT_CLIC_SUPPORTED */
