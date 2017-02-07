#ifndef _PSRAM_H
#define _PSRAM_H
#include "soc/spi_reg.h"
#include "esp_err.h"
#define PSRAM_READ              0x03
#define PSRAM_FAST_READ          0x0B
#define PSRAM_FAST_READ_QUAD      0xEB
#define PSRAM_WRITE             0x02
#define PSRAM_QUAD_WRITE         0x38
#define PSRAM_ENTER_QMODE     0x35
#define PSRAM_EXIT_QMODE      0xF5
#define PSRAM_RESET_EN           0x66
#define PSRAM_RESET             0x99
#define PSRAM_SET_BURST_LEN       0xC0
#define PSRAM_DEVICE_ID          0x9F

typedef enum {
    PSRAM_SPI_1  = 0x1,
    PSRAM_SPI_2,
    PSRAM_SPI_3,
    PSRAM_SPI_MAX ,
} psram_spi_num_t;

typedef enum {
	PSRAM_CACHE_F80M_S40M = 0,//DO NOT USE FOR NOW
	PSRAM_CACHE_F40M_S40M, //SUPPORTED
	PSRAM_CACHE_F80M_S80M, //DO NOT USE FOR NOW
	PSRAM_CACHE_MAX,
} psram_cache_mode_t;

esp_err_t psram_enable(psram_cache_mode_t mode);

#endif
