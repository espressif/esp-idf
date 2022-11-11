/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure for flash dummy bits.
 *        For some flash chips, dummy bits are configurable under different conditions.
 */
typedef struct {
    uint8_t dio_dummy;
    uint8_t dout_dummy;
    uint8_t qio_dummy;
    uint8_t qout_dummy;
    uint8_t fastrd_dummy;
} spi_flash_hpm_dummy_conf_t;

typedef enum {
    SPI_FLASH_HPM_CMD_NEEDED,      // Means that in the certain condition, flash needs to enter the high performance mode by command.
    SPI_FLASH_HPM_DUMMY_NEEDED,    // Means that in the certain condition, flash needs to enter the high performance mode by adjusting dummy.
    SPI_FLASH_HPM_WRITE_SR_NEEDED, // Means that in the certain condition, flash needs to enter the high performance mode by writing status register.
    SPI_FLASH_HPM_UNNEEDED,           // Means that flash doesn't need to enter the high performance mode.
    SPI_FLASH_HPM_BEYOND_LIMIT,       // Means that flash has no capability to meet that condition.
} spi_flash_requirement_t;

typedef void (*spi_flash_hpm_enable_fn_t)(void);
typedef esp_err_t (*spi_flash_hpf_check_fn_t)(void);
typedef void (*spi_flash_get_chip_dummy_fn_t)(spi_flash_hpm_dummy_conf_t *dummy_conf);
typedef esp_err_t (*spi_flash_hpm_probe_fn_t)(uint32_t flash_id);
typedef spi_flash_requirement_t (*spi_flash_hpm_chip_requirement_check_t)(uint32_t flash_id, uint32_t freq_mhz, int voltage_mv, int temperature);

typedef struct __attribute__((packed))
{
    const char *method;      /* Flash HPM method */
    spi_flash_hpm_probe_fn_t probe;
    spi_flash_hpm_chip_requirement_check_t chip_hpm_requirement_check;
    spi_flash_hpm_enable_fn_t flash_hpm_enable;
    spi_flash_hpf_check_fn_t flash_hpf_check;
    spi_flash_get_chip_dummy_fn_t flash_get_dummy;
} spi_flash_hpm_info_t;

/**
 * Array of known flash chips and method to enable flash high performance mode.
 *
 * Users can override this array.
 */
extern const spi_flash_hpm_info_t __attribute__((weak)) spi_flash_hpm_enable_list[];

#ifdef __cplusplus
}
#endif
