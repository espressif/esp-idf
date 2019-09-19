// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdarg.h>
#include "esp_attr.h"
#include "esp_spi_flash.h"   //for ``g_flash_guard_default_ops``
#include "esp_flash.h"
#include "esp_flash_partitions.h"


#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#else
#include "esp32s2beta/rom/ets_sys.h"
#endif

/*
 * OS functions providing delay service and arbitration among chips, and with the cache.
 *
 * The cache needs to be disabled when chips on the SPI1 bus is under operation, hence these functions need to be put
 * into the IRAM,and their data should be put into the DRAM.
 */

typedef struct {
    int host_id;
} app_func_arg_t;

typedef struct {
    int host_id;
    bool no_protect;    //to decide whether to check protected region (for the main chip) or not.
} spi1_app_func_arg_t;

// in the future we will have arbitration among devices, including flash on the same flash bus
static IRAM_ATTR esp_err_t spi_bus_acquire(int host_id)
{
    return ESP_OK;
}

static IRAM_ATTR esp_err_t spi_bus_release(int host_id)
{
    return ESP_OK;
}

//for SPI1, we have to disable the cache and interrupts before using the SPI bus
static IRAM_ATTR esp_err_t spi1_start(void *arg)
{
    g_flash_guard_default_ops.start();

    spi_bus_acquire(((spi1_app_func_arg_t *)arg)->host_id);

    return ESP_OK;
}
static IRAM_ATTR esp_err_t spi1_end(void *arg)
{
    g_flash_guard_default_ops.end();

    spi_bus_release(((spi1_app_func_arg_t *)arg)->host_id);

    return ESP_OK;
}

static esp_err_t spi23_start(void *arg)
{
    spi_bus_acquire(((app_func_arg_t *)arg)->host_id);
    return ESP_OK;
}

static esp_err_t spi23_end(void *arg)
{
    spi_bus_release(((app_func_arg_t *)arg)->host_id);
    return ESP_OK;
}

static IRAM_ATTR esp_err_t delay_ms(void *arg, unsigned ms)
{
    ets_delay_us(1000 * ms);
    return ESP_OK;
}

static IRAM_ATTR esp_err_t main_flash_region_protected(void* arg, size_t start_addr, size_t size)
{
    if (((spi1_app_func_arg_t*)arg)->no_protect || esp_partition_main_flash_region_safe(start_addr, size)) {
        //ESP_OK = 0, also means protected==0
        return ESP_OK;
    } else {
        return ESP_ERR_NOT_SUPPORTED;
    }
}

static DRAM_ATTR spi1_app_func_arg_t spi1_arg = {
    .host_id = 0,   //for SPI1,
    .no_protect = true,
};

static DRAM_ATTR spi1_app_func_arg_t main_flash_arg = {
    .host_id = 0,   //for SPI1,
    .no_protect = false,
};

static app_func_arg_t spi2_arg = {
    .host_id = 1,   //for SPI2,
};

static app_func_arg_t spi3_arg = {
    .host_id = 2,   //for SPI3,
};

//for SPI1, we have to disable the cache and interrupts before using the SPI bus
const DRAM_ATTR esp_flash_os_functions_t esp_flash_spi1_default_os_functions = {
    .start = spi1_start,
    .end = spi1_end,
    .delay_ms = delay_ms,
    .region_protected = main_flash_region_protected,
};

const esp_flash_os_functions_t esp_flash_spi23_default_os_functions = {
    .start = spi23_start,
    .end = spi23_end,
    .delay_ms = delay_ms,
};

esp_err_t esp_flash_init_os_functions(esp_flash_t *chip, int host_id)
{
    if (host_id == 0) {
        //SPI1
        chip->os_func = &esp_flash_spi1_default_os_functions;
        chip->os_func_data = &spi1_arg;
    } else if (host_id == 1 || host_id == 2) {
        //SPI2,3
        chip->os_func = &esp_flash_spi23_default_os_functions;
        chip->os_func_data = (host_id == 1) ? &spi2_arg : &spi3_arg;
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t esp_flash_app_init_os_functions(esp_flash_t* chip)
{
    chip->os_func = &esp_flash_spi1_default_os_functions;
    chip->os_func_data = &main_flash_arg;
    return ESP_OK;
}


