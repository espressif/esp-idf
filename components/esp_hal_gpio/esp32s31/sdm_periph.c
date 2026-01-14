/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/sdm_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_ext_reg.h"

const soc_sdm_signal_desc_t soc_sdm_signals[1] = {
    [0] = {
        .module_name = "SDM0",
        .channels = {
            [0] = {
                .sig_id_matrix = GPIO_SD0_OUT_IDX
            },
            [1] = {
                .sig_id_matrix = GPIO_SD1_OUT_IDX
            },
            [2] = {
                .sig_id_matrix = GPIO_SD2_OUT_IDX
            },
            [3] = {
                .sig_id_matrix = GPIO_SD3_OUT_IDX
            },
            [4] = {
                .sig_id_matrix = GPIO_SD4_OUT_IDX
            },
            [5] = {
                .sig_id_matrix = GPIO_SD5_OUT_IDX
            },
            [6] = {
                .sig_id_matrix = GPIO_SD6_OUT_IDX
            },
            [7] = {
                .sig_id_matrix = GPIO_SD7_OUT_IDX
            }
        }
    }
};

#if SOC_PAU_SUPPORTED
/**
 * @brief Registers in retention context:
 *        GPIO_EXT_SIGMADELTA[x]_REG
 *        GPIO_EXT_SIGMADELTA_MISC_REG
 */
#define GPIO_EXT_RETENTION_REGS_CNT 9
#define GPIO_EXT_RETENTION_REGS_BASE (DR_REG_GPIO_EXT_BASE + 0x4)
static const uint32_t gpio_ext_regs_map[4] = {0x1ff, 0x0, 0x0, 0x0};
static const regdma_entries_config_t gpio_ext_regdma_entries[] = {
    // backup stage: save configuration and status registers
    // restore stage: restore the configuration and status registers
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GPIO_EXT_LINK(0x00),
                                            GPIO_EXT_RETENTION_REGS_BASE, GPIO_EXT_RETENTION_REGS_BASE,
                                            GPIO_EXT_RETENTION_REGS_CNT, 0, 0,
                                            gpio_ext_regs_map[0], gpio_ext_regs_map[1],
                                            gpio_ext_regs_map[2], gpio_ext_regs_map[3]),
        .owner = ENTRY(0) | ENTRY(2),
    },
};

const soc_sdm_retention_desc_t soc_sdm_retention_infos[1] = {
    [0] = {
        .module = SLEEP_RETENTION_MODULE_SDM0,
        .regdma_entry_array = gpio_ext_regdma_entries,
        .array_size = ARRAY_SIZE(gpio_ext_regdma_entries)
    }
};
#endif // SOC_PAU_SUPPORTED
