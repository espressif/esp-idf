/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdm_priv.h"
#include "soc/gpio_ext_reg.h"

/**
 * @brief Registers in retention context:
 *        GPIO_EXT_SIGMADELTA[x]_REG
 *        GPIO_EXT_SIGMADELTA_MISC_REG
 */
#define GPIO_EXT_RETENTION_REGS_CNT 9
#define GPIO_EXT_RETENTION_REGS_BASE (DR_REG_GPIO_EXT_BASE + 0x0)
static const uint32_t gpio_ext_regs_map[4] = {0x2ff, 0x0, 0x0, 0x0};
static const regdma_entries_config_t gpio_ext_regdma_entries[] = {
    // backup stage: save configuration and status registers
    // restore stage: restore the configuration and status registers
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_SDM_LINK(0x00),
                                            GPIO_EXT_RETENTION_REGS_BASE, GPIO_EXT_RETENTION_REGS_BASE,
                                            GPIO_EXT_RETENTION_REGS_CNT, 0, 0,
                                            gpio_ext_regs_map[0], gpio_ext_regs_map[1],
                                            gpio_ext_regs_map[2], gpio_ext_regs_map[3]),
        .owner = ENTRY(0),
    },
};

const sdm_reg_retention_info_t sdm_reg_retention_infos[1] = {
    [0] = {
        .module = SLEEP_RETENTION_MODULE_SDM0,
        .regdma_entry_array = gpio_ext_regdma_entries,
        .array_size = ARRAY_SIZE(gpio_ext_regdma_entries)
    }
};
