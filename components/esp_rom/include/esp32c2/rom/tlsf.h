/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Set the function to call for filling memory region when
 * poisoning is configured.
 *
 * @param pfunc The callback function to trigger for poisoning
 * a memory region.
 */
void tlsf_poison_fill_pfunc_set(void *pfunc);

/*!
 * @brief Set the function to call for checking memory region when
 * poisoning is configured.
 *
 * @param pfunc The callback function to trigger for checking
 * the content of a memory region.
 */
void tlsf_poison_check_pfunc_set(void *pfunc);

#ifdef __cplusplus
}
#endif
