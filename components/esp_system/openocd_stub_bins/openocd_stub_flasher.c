/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>

#include "stub_flash_idf_image.h"

/* Copied from openocd-esp32/contrib/loaders/flash/espressif/${target}/inc/stub_flash_idf_binary_code.inc */
static const uint8_t s_flasher_stub_code[]  __attribute__((used, section(".ocd_stub.code"))) = {
#include "stub_flash_idf_binary_code.inc"
};

/* Copied from openocd-esp32/src/target/espressif/esp_riscv_stub_tramp.inc */
static const uint8_t s_flasher_stub_tramp[] __attribute__((used, section(".ocd_stub.tramp"))) = {
#include "esp_riscv_stub_tramp.inc"
};

/* Copied from openocd-esp32/contrib/loaders/flash/espressif/${target}/inc/stub_flash_idf_binary_data.inc */
static const uint8_t s_flasher_stub_data[] __attribute__((used, section(".ocd_stub.data"))) = {
#include "stub_flash_idf_binary_data.inc"
};

static uint8_t s_flasher_stub_bss[OPENOCD_STUB_BSS_SIZE] __attribute__((used, section(".ocd_stub.bss")));
static uint8_t s_flasher_stub_stack[OPENOCD_STUB_STACK_SIZE] __attribute__((used, section(".ocd_stub.stack")));
static uint8_t s_flasher_stub_params[OPENOCD_STUB_PARAM_SIZE] __attribute__((used, section(".ocd_stub.params")));
static uint8_t s_flasher_stub_bp_sectors[OPENOCD_STUB_BP_SECTOR_SIZE] __attribute__((used, section(".ocd_stub.scratchmem")));

// Hook to force the linker to include this file
void esp_system_include_openocd_stub_binaries(void)
{
}
