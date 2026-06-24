/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Function prototype for interrupt handler function */
typedef void (*intr_handler_t)(void *arg);

typedef struct vector_desc_t vector_desc_t;

struct vector_desc_t {
    int source: 8;
    intr_handler_t isr;
    void *arg;
    vector_desc_t *next;
};

/**
 * @brief Route peripheral interrupt sources to CPU's interrupt port via interrupt matrix
 *
 * Since the interrupt matrix controls the secure (TEE) interrupt source mapping to the
 * TEE-reserved interrupt pin, this hardware module is controlled by the TEE.
 * This API is provided as a Secure Service to the REE for the configuration of
 * non-secure (REE) interrupts.
 *
 * @param cpu_no CPU core number to route the interrupt to
 * @param model_num Peripheral interrupt source number
 * @param intr_num CPU external interrupt number to assign
 */
void esp_tee_route_intr_matrix(int cpu_no, uint32_t model_num, uint32_t intr_num);

/**
 * @brief Check if an interrupt source is protected from REE access
 *
 * @param source Peripheral interrupt source number
 * @return true if the interrupt source is protected, false otherwise
 */
bool esp_tee_is_intr_src_protected(int source);

/**
 * @brief Protect an interrupt source from REE access
 *
 * @param source Peripheral interrupt source number
 */
void esp_tee_protect_intr_src(int source);

/**
 * @brief Register an interrupt handler
 *
 * @param arg Pointer to the interrupt descriptor
 * @return 0 on success, non-zero on failure
 */
int esp_tee_intr_register(void *arg);

/**
 * @brief Deregister an interrupt handler
 *
 * @param arg Pointer to the interrupt descriptor
 * @return 0 on success, non-zero on failure
 */
int esp_tee_intr_deregister(void *arg);

#ifdef __cplusplus
}
#endif

#endif //__ASSEMBLER__
