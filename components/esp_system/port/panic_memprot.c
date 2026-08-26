/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#include "esp_err.h"
#include "esp_private/panic_internal.h"
#include "esp_private/esp_memprot_internal.h"
#include "esp_memprot.h"

static esp_memp_intr_source_t s_memp_intr = {MEMPROT_TYPE_INVALID, -1};

#define PRINT_MEMPROT_ERROR(err) \
        do { \
            panic_print_str("N/A (error "); \
            panic_print_str(esp_err_to_name(err)); \
            panic_print_str(")"); \
        } while(0)

/**
 * Function called when a memory protection error occurs (PMS). It prints details such as the
 * explanation of why the panic occurred.
 */
static void print_memprot_err_details(const void *frame __attribute__((unused)))
{
    if (s_memp_intr.mem_type == MEMPROT_TYPE_INVALID && s_memp_intr.core == -1) {
        panic_print_str("  - no details available -\r\n");
        return;
    }

    //common memprot fault info
    panic_print_str("  memory type: ");
    panic_print_str(esp_mprot_mem_type_to_str(s_memp_intr.mem_type));

    panic_print_str("\r\n  faulting address: ");
    void *faulting_addr;
    esp_err_t res = esp_mprot_get_violate_addr(s_memp_intr.mem_type, &faulting_addr, s_memp_intr.core);
    if (res == ESP_OK) {
        panic_print_str("0x");
        panic_print_hex((int)faulting_addr);
    } else {
        PRINT_MEMPROT_ERROR(res);
    }

    panic_print_str("\r\n  world: ");
    esp_mprot_pms_world_t world;
    res = esp_mprot_get_violate_world(s_memp_intr.mem_type, &world, s_memp_intr.core);
    if (res == ESP_OK) {
        panic_print_str(esp_mprot_pms_world_to_str(world));
    } else {
        PRINT_MEMPROT_ERROR(res);
    }

    panic_print_str("\r\n  operation type: ");
    uint32_t operation;
    res = esp_mprot_get_violate_operation(s_memp_intr.mem_type, &operation, s_memp_intr.core);
    if (res == ESP_OK) {
        panic_print_str(esp_mprot_oper_type_to_str(operation));
    } else {
        PRINT_MEMPROT_ERROR(res);
    }

    if (esp_mprot_has_byte_enables(s_memp_intr.mem_type)) {
        panic_print_str("\r\n  byte-enables: ");
        uint32_t byte_enables;
        res = esp_mprot_get_violate_byte_enables(s_memp_intr.mem_type, &byte_enables, s_memp_intr.core);
        if (res == ESP_OK) {
            panic_print_hex(byte_enables);
        } else {
            PRINT_MEMPROT_ERROR(res);
        }
    }

    panic_print_str("\r\n");
}

bool panic_memprot_fill_info(panic_info_t *info)
{
    info->reason = "Memory protection fault";
    info->details = print_memprot_err_details;
    info->core = esp_mprot_get_active_intr(&s_memp_intr) == ESP_OK ? s_memp_intr.core : -1;

    return true;
}
