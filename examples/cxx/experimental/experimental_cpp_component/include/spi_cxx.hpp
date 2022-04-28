/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#if __cpp_exceptions

#include "esp_exception.hpp"
#include "gpio_cxx.hpp"
#include "system_cxx.hpp"

namespace idf {

/**
 * @brief Exception which is thrown in the context of SPI C++ classes.
 */
struct SPIException : public ESPException {
    SPIException(esp_err_t error);
};

/**
 * @brief The maximum SPI transfer size in bytes.
 */
class SPITransferSize : public StrongValueOrdered<size_t> {
public:
    /**
     * @brief Create a valid SPI transfer size.
     *
     * @param transfer_size The raw transfer size in bytes.
     */
    explicit SPITransferSize(size_t transfer_size) noexcept : StrongValueOrdered<size_t>(transfer_size) { }

    static SPITransferSize default_size() {
        return SPITransferSize(0);
    }
};

/**
 * @brief Check if the raw uint32_t spi number is in the range according to the hardware.
 */
esp_err_t check_spi_num(uint32_t spi_num) noexcept;

/**
 * @brief Represents a valid SPI host number.
 *
 * ESP chips may have different independent SPI peripherals. This SPI number distinguishes between them.
 */
class SPINum : public StrongValueComparable<uint32_t> {
public:
    /**
     * @brief Create a valid SPI host number.
     *
     * @param host_id_raw The raw SPI host number.
     *
     * @throw SPIException if the passed SPI host number is incorrect.
     */
    explicit SPINum(uint32_t host_id_raw) : StrongValueComparable<uint32_t>(host_id_raw)
    {
        esp_err_t spi_num_check_result = check_spi_num(host_id_raw);
        if (spi_num_check_result != ESP_OK) {
            throw SPIException(spi_num_check_result);
        }
    }

    /**
     * @brief Return the raw value of the SPI host.
     *
     * This should only be used when calling driver and other interfaces which don't support the C++ class.
     *
     * @return the raw value of the SPI host.
     */
    uint32_t get_spi_num() const
    {
        return get_value();
    }
};

/**
 * @brief Represents a valid MOSI signal pin number.
 */
class MOSI_type;
using MOSI = GPIONumBase<class MOSI_type>;

/**
 * @brief Represents a valid MISO signal pin number.
 */
class MISO_type;
using MISO = GPIONumBase<class MISO_type>;

/**
 * @brief Represents a valid SCLK signal pin number.
 */
class SCLK_type;
using SCLK = GPIONumBase<class SCLK_type>;

/**
 * @brief Represents a valid CS (chip select) signal pin number.
 */
class CS_type;
using CS = GPIONumBase<class CS_type>;

/**
 * @brief Represents a valid QSPIWP signal pin number.
 */
class QSPIWP_type;
using QSPIWP = GPIONumBase<class QSPIWP_type>;

/**
 * @brief Represents a valid QSPIHD signal pin number.
 */
class QSPIHD_type;
using QSPIHD = GPIONumBase<class QSPIHD_type>;

/**
 * @brief Represents a valid SPI DMA configuration. Use it similar to an enum.
 */
class SPI_DMAConfig : public StrongValueComparable<uint32_t> {
    /**
     * Constructor is hidden to enforce object invariants.
     * Use the static creation methods to create instances.
     */
    explicit SPI_DMAConfig(uint32_t channel_num) : StrongValueComparable<uint32_t>(channel_num) { }

public:
    /**
     * @brief Create a configuration with DMA disabled.
     */
    static SPI_DMAConfig DISABLED();

    /**
     * @brief Create a configuration where the driver allocates DMA.
     */
    static SPI_DMAConfig AUTO();

    /**
     * @brief Return the raw value of the DMA configuration.
     *
     * This should only be used when calling driver and other interfaces which don't support the C++ class.
     *
     * @return the raw value of the DMA configuration.
     */
    uint32_t get_num() const {
        return get_value();
    }
};

}

#endif
