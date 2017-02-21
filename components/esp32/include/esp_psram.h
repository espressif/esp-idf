#ifndef _PSRAM_H
#define _PSRAM_H
#include "soc/spi_reg.h"
#include "esp_err.h"

typedef enum {
    PSRAM_CACHE_F80M_S40M = 0, //DO NOT USE FOR NOW
    PSRAM_CACHE_F40M_S40M,     //SUPPORTED
    PSRAM_CACHE_F80M_S80M,     //DO NOT USE FOR NOW
    PSRAM_CACHE_MAX,
} psram_cache_mode_t;

typedef enum {
    PSRAM_VADDR_MODE_NORMAL=0,
    PSRAM_VADDR_MODE_LOWHIGH,
    PSRAM_VADDR_MODE_EVENODD,
} psram_vaddr_mode_t;

esp_err_t psram_enable(psram_cache_mode_t mode, psram_vaddr_mode_t vaddrmode);

#endif
