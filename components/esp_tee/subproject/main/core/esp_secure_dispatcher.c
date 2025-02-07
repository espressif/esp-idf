/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdarg.h>
#include "esp_log.h"
#include "esp_tee.h"
#include "secure_service_num.h"

#define ESP_TEE_MAX_INPUT_ARG 10

static const char *TAG = "esp_tee_sec_disp";

extern const secure_service_entry_t tee_sec_srv_tbl_int_mem[];
extern const secure_service_entry_t tee_sec_srv_tbl_ext_mem[];

/* ---------------------------------------------- Secure Service Dispatcher ------------------------------------------------- */

static const secure_service_entry_t *find_service_by_id(uint32_t id)
{
    if (id >= MAX_SECURE_SERVICES_ID) {
        return NULL;
    }

    if (id < SECURE_SERVICES_SPLIT_ID) {
        return &tee_sec_srv_tbl_int_mem[id];
    } else {
        return &tee_sec_srv_tbl_ext_mem[id];
    }

    return NULL;
}

/**
 * @brief Handles incoming secure service requests to the TEE.
 *        Validates and routes each request to the appropriate
 *        secure service implementation.
 */
int esp_tee_service_dispatcher(int argc, va_list ap)
{
    if (argc > ESP_TEE_MAX_INPUT_ARG) {
        ESP_LOGE(TAG, "Input arguments overflow! Received %d, Permitted %d",
                 argc, ESP_TEE_MAX_INPUT_ARG);
        return -1;
    }

    int ret = -1;
    uint32_t argv[ESP_TEE_MAX_INPUT_ARG] = {0};

    uint32_t sid = va_arg(ap, uint32_t);
    argc--;

    const secure_service_entry_t *service = find_service_by_id(sid);
    if (service == NULL) {
        ESP_LOGE(TAG, "Invalid service ID!");
        return ret;
    }

    if (argc != service->nargs) {
        ESP_LOGE(TAG, "Invalid number of arguments for service %d!", sid);
        return ret;
    }

    void *fp_secure_service = (void *)service->func;

    for (int i = 0; i < argc; i++) {
        argv[i] = va_arg(ap, uint32_t);
    }
    uint32_t *argp = &argv[0];

    asm volatile(
        "mv t0, %1 \n"            // t0 = argc
        "mv t1, %3 \n"            // t1 = argp

        "li t2, 8 \n"              // t2 = 8 (max register args)
        "ble t0, t2, load_regs \n" // If argc <= 8 (a0-a7), skip stack routine

        // Store extra args (argc > 8) on stack
        "mv t3, sp \n"
        "addi t1, t1, 32 \n"

        "stack_loop: \n"
        "lw t4, 0(t1) \n"
        "sw t4, 0(t3) \n"
        "addi t1, t1, 4 \n"
        "addi t3, t3, 4 \n"
        "addi t0, t0, -1 \n"
        "bge t0, t2, stack_loop \n"

        // Load the first 8 arguments into a0-a7
        "load_regs: \n"
        "lw a0, 0(%3) \n"
        "lw a1, 4(%3) \n"
        "lw a2, 8(%3) \n"
        "lw a3, 12(%3) \n"
        "lw a4, 16(%3) \n"
        "lw a5, 20(%3) \n"
        "lw a6, 24(%3) \n"
        "lw a7, 28(%3) \n"
        "fence \n"

        "mv t1, %2 \n"            // Load function pointer
        "jalr 0(t1) \n"           // Call function
        "mv %0, a0 \n"            // Store return value
        : "=r"(ret)
        : "r"(argc), "r"(fp_secure_service), "r"(argp)
        : "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
        "t0", "t1", "t2", "t3", "t4"
    );

    return ret;
}
