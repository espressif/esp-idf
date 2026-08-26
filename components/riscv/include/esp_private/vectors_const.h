/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once


/**
 * The interrupt bit in `mcause` register is always bit 31 regardless of the interrupt controller used
 */
#define VECTORS_MCAUSE_INTBIT_MASK    (0x80000000)


#if SOC_INT_CLIC_SUPPORTED

/* When using the CLIC as their interrupt controller, the `mcause` register contains more information than
 * the interrupt bit and cause:
 * MINHV[30]: CPU is fetching vector interrupt entry address or not
 * MPP[29:28]: MSTATUS.MPP[1:0]
 * MPIL[23:16]: interrupt level before entering interrupt ISR
 *
 * Define the mask that will only keep the cause.
 */
#define VECTORS_MCAUSE_REASON_MASK   (0x00000fff)

#else // !if SOC_INT_CLIC_SUPPORTED

/**
 * For targets that use the former INTC or CLINT/PLIC, the `mcause` shouldn't contain any more information
 * but let's be safe and keep the 32 possible cause values.
 */
#define VECTORS_MCAUSE_REASON_MASK   (0x0000001f)

#endif
