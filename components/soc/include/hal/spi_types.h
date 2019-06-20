#pragma once

#include "soc/spi_caps.h"
#include "sdkconfig.h"

/**
 * @brief Enum with the three SPI peripherals that are software-accessible in it
 */
typedef enum {
    SPI1_HOST=0,    ///< SPI1
    SPI2_HOST=1,    ///< SPI2
    SPI3_HOST=2,    ///< SPI3
} spi_host_device_t;

//alias for different chips
#define SPI_HOST    SPI1_HOST
#define HSPI_HOST   SPI2_HOST
#define VSPI_HOST   SPI3_HOST
