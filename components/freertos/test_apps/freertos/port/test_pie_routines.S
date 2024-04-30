/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc_caps.h"

/* PIE instructions set is currently only supported in GCC compiler */
#if SOC_CPU_HAS_PIE

    .text
    .align      4

/**
 * @brief Performs the unsigned sum of two 4-word vectors using the PIE.
 *
 * @param a0 First vector
 * @param a1 Second vector
 * @param a2 Destination to store the sum
 */
    .type pie_vector_unsigned_add, @function
    .global pie_vector_unsigned_add
pie_vector_unsigned_add:
    esp.vld.128.ip  q0, a0, 0
    esp.vld.128.ip  q1, a1, 0
    esp.vadd.u32    q2, q0, q1
    esp.vst.128.ip  q2, a2, 0
    ret
    .size  pie_vector_unsigned_add, .-pie_vector_unsigned_add


/**
 * @brief Performs the signed sum of two 4-word vectors using the PIE.
 *
 * @param a0 First vector
 * @param a1 Second vector
 * @param a2 Destination to store the sum
 */
    .type pie_vector_signed_add, @function
    .global pie_vector_signed_add
pie_vector_signed_add:
    esp.vld.128.ip  q0, a0, 0
    esp.vld.128.ip  q1, a1, 0
    esp.vadd.s32    q2, q0, q1
    esp.vst.128.ip  q2, a2, 0
    ret
    .size  pie_vector_signed_add, .-pie_vector_signed_add

#endif /* SOC_CPU_HAS_PIE */
