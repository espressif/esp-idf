/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include "soc/soc.h"
#include "sdkconfig.h"
#include "esp_cpu.h"
#include "esp_attr.h"

#include "riscv/rv_utils.h"

#define ESP_TEE_APP_CFG_MAGIC 0x3348AAED

#define ESP_TEE_API_MAJOR_VER  1
#define ESP_TEE_API_MINOR_VER  0
#define ESP_TEE_API_PATCH_VER  0

/**
 * @brief CPU privilege mode
 */
typedef enum {
    ESP_CPU_NS_MODE = 0,    /* Corresponds to the RISC-V User (U) mode */
    ESP_CPU_S_MODE =  3,    /* Corresponds to the RISC-V Machine (M) mode */
} esp_cpu_priv_mode_t;

/**
 * @brief Configuration structure defining the interface between TEE and REE (user) app
 *
 * This configuration structure is embedded in the REE (user) app's IRAM section.
 * The TEE reads and updates this structure before switching to the REE, and then
 * write-protects it.
 *
 * @note All accesses to this structure must be 32-bit aligned since it resides in
 *       the (user app) IRAM section.
 */
typedef struct {
    uint32_t magic_word;
    uint32_t api_major_version;
    uint32_t api_minor_version;
    uint32_t reserved[2];
    /* TEE-related fields */
    void *s_entry_addr;
    void *s_int_handler;
    /* REE-related fields */
    void *ns_entry_addr;
    void *ns_int_handler;
    void *ns_iram_end;
    void *ns_irom_end;
    void *ns_drom_end;
} __attribute__((aligned(4))) __attribute__((__packed__)) esp_tee_config_t;

extern esp_tee_config_t esp_tee_app_config;

#endif // ifndef __ASSEMBLER__

#if !ESP_TEE_BUILD
#include "private/esp_tee_app.h"
#else
#include "private/esp_tee_binary.h"
#endif

/* Offsets of some values in esp_tee_config_t that are used by assembly code */
#define ESP_TEE_CFG_OFFS_S_ENTRY_ADDR      0x14
#define ESP_TEE_CFG_OFFS_S_INTR_HANDLER    0x18
#define ESP_TEE_CFG_OFFS_NS_ENTRY_ADDR     0x1C
#define ESP_TEE_CFG_OFFS_NS_INTR_HANDLER   0x20

#ifndef __ASSEMBLER__
/* Check the offsets are correct using the C compiler */
ESP_STATIC_ASSERT(offsetof(esp_tee_config_t, s_entry_addr) == ESP_TEE_CFG_OFFS_S_ENTRY_ADDR, "offset macro is wrong");
ESP_STATIC_ASSERT(offsetof(esp_tee_config_t, s_int_handler) == ESP_TEE_CFG_OFFS_S_INTR_HANDLER, "offset macro is wrong");
ESP_STATIC_ASSERT(offsetof(esp_tee_config_t, ns_entry_addr) == ESP_TEE_CFG_OFFS_NS_ENTRY_ADDR, "offset macro is wrong");
ESP_STATIC_ASSERT(offsetof(esp_tee_config_t, ns_int_handler) == ESP_TEE_CFG_OFFS_NS_INTR_HANDLER, "offset macro is wrong");
#endif // ifndef __ASSEMBLER__

#ifdef __cplusplus
}
#endif
