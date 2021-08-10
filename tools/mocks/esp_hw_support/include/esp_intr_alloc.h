/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * NOTE: this is not the original header file from the esp_hw_support component.
 * It is a stripped-down copy to support mocking.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Function prototype for interrupt handler function */
typedef void (*intr_handler_t)(void *arg);

/** Interrupt handler associated data structure */
typedef struct intr_handle_data_t intr_handle_data_t;

/** Handle to an interrupt handler */
typedef intr_handle_data_t *intr_handle_t ;

#ifdef __cplusplus
}
#endif
