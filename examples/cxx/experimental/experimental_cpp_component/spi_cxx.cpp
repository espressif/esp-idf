/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if __cpp_exceptions

#include "driver/spi_common.h"
#include "esp_exception.hpp"
#include "spi_cxx.hpp"

namespace idf {

esp_err_t check_spi_num(uint32_t spi_num) noexcept {
    if (spi_num >= static_cast<uint32_t>(SPI_HOST_MAX)) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

SPI_DMAConfig SPI_DMAConfig::DISABLED() {
    return SPI_DMAConfig(static_cast<uint32_t>(spi_common_dma_t::SPI_DMA_DISABLED));
}

SPI_DMAConfig SPI_DMAConfig::AUTO() {
    return SPI_DMAConfig(static_cast<uint32_t>(spi_common_dma_t::SPI_DMA_CH_AUTO));
}

}

#endif
