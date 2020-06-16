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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/spi_types.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/ets_sys.h"
#endif

#include "driver/spi_common_internal.h"


/*
 * OS functions providing delay service and arbitration among chips, and with the cache.
 *
 * The cache needs to be disabled when chips on the SPI1 bus is under operation, hence these functions need to be put
 * into the IRAM,and their data should be put into the DRAM.
 */

typedef struct {
    spi_bus_lock_dev_handle_t dev_lock;
} app_func_arg_t;

typedef struct {
    app_func_arg_t common_arg; //shared args, must be the first item
    bool no_protect;    //to decide whether to check protected region (for the main chip) or not.
} spi1_app_func_arg_t;

IRAM_ATTR static void cache_enable(void* arg)
{
    g_flash_guard_default_ops.end();
}

IRAM_ATTR static void cache_disable(void* arg)
{
    g_flash_guard_default_ops.start();
}

static IRAM_ATTR esp_err_t spi_start(void *arg)
{
    spi_bus_lock_dev_handle_t dev_lock = ((app_func_arg_t *)arg)->dev_lock;

    // wait for other devices (or cache) to finish their operation
    esp_err_t ret = spi_bus_lock_acquire_start(dev_lock, portMAX_DELAY);
    if (ret != ESP_OK) {
        return ret;
    }
    spi_bus_lock_touch(dev_lock);
    return ESP_OK;
}

static IRAM_ATTR esp_err_t spi_end(void *arg)
{
    return spi_bus_lock_acquire_end(((app_func_arg_t *)arg)->dev_lock);
}

static IRAM_ATTR esp_err_t spi1_start(void *arg)
{
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
    //use the lock to disable the cache and interrupts before using the SPI bus
    return spi_start(arg);
#else
    //directly disable the cache and interrupts when lock is not used
    cache_disable(NULL);
#endif
    return ESP_OK;
}

static IRAM_ATTR esp_err_t spi1_end(void *arg)
{
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
    return spi_end(arg);
#else
    cache_enable(NULL);
    return ESP_OK;
#endif
}

static IRAM_ATTR esp_err_t delay_us(void *arg, unsigned us)
{
    ets_delay_us(us);
    return ESP_OK;
}

static IRAM_ATTR esp_err_t spi_flash_os_yield(void *arg)
{
#ifdef CONFIG_SPI_FLASH_YIELD_DURING_ERASE
    vTaskDelay(CONFIG_SPI_FLASH_ERASE_YIELD_TICKS);
#endif
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

static DRAM_ATTR spi1_app_func_arg_t main_flash_arg = {};

//for SPI1, we have to disable the cache and interrupts before using the SPI bus
static const DRAM_ATTR esp_flash_os_functions_t esp_flash_spi1_default_os_functions = {
    .start = spi1_start,
    .end = spi1_end,
    .region_protected = main_flash_region_protected,
    .delay_us = delay_us,
    .yield = spi_flash_os_yield,
};

static const esp_flash_os_functions_t esp_flash_spi23_default_os_functions = {
    .start = spi_start,
    .end = spi_end,
    .delay_us = delay_us,
    .yield = spi_flash_os_yield
};

static spi_bus_lock_dev_handle_t register_dev(int host_id)
{
    spi_bus_lock_handle_t lock = spi_bus_lock_get_by_id(host_id);
    spi_bus_lock_dev_handle_t dev_handle;
    spi_bus_lock_dev_config_t config = {.flags = SPI_BUS_LOCK_DEV_FLAG_CS_REQUIRED};
    esp_err_t err = spi_bus_lock_register_dev(lock, &config, &dev_handle);
    if (err != ESP_OK) {
        return NULL;
    }
    return dev_handle;
}

esp_err_t esp_flash_init_os_functions(esp_flash_t *chip, int host_id, int* out_dev_id)
{
    spi_bus_lock_dev_handle_t dev_handle = NULL;

    // Skip initializing the bus lock when the bus is SPI1 and the bus is not shared with SPI Master
    // driver, leaving dev_handle = NULL
    bool skip_register_dev = (host_id == SPI_HOST);
#if CONFIG_SPI_FLASH_SHARE_SPI1_BUS
    skip_register_dev = false;
#endif
    if (!skip_register_dev) {
        dev_handle = register_dev(host_id);
    }

    if (host_id == SPI1_HOST) {
        //SPI1
        chip->os_func = &esp_flash_spi1_default_os_functions;
        chip->os_func_data = heap_caps_malloc(sizeof(spi1_app_func_arg_t),
                                         MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (chip->os_func_data == NULL) {
            return ESP_ERR_NO_MEM;
        }
        *(spi1_app_func_arg_t*) chip->os_func_data = (spi1_app_func_arg_t) {
            .common_arg = {
                .dev_lock = dev_handle,
            },
            .no_protect = true,
        };
    } else if (host_id == SPI2_HOST || host_id == SPI3_HOST) {
        //SPI2, SPI3
        chip->os_func = &esp_flash_spi23_default_os_functions;
        chip->os_func_data = heap_caps_malloc(sizeof(app_func_arg_t),
                                         MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (chip->os_func_data == NULL) {
            return ESP_ERR_NO_MEM;
        }
        *(app_func_arg_t*) chip->os_func_data = (app_func_arg_t) {
                .dev_lock = dev_handle,
        };
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    // Bus lock not initialized, the device ID should be directly given by application.
    if (dev_handle) {
        *out_dev_id = spi_bus_lock_get_dev_id(dev_handle);
    }

    return ESP_OK;
}

esp_err_t esp_flash_deinit_os_functions(esp_flash_t* chip)
{
    if (chip->os_func_data) {
        spi_bus_lock_dev_handle_t dev_lock = ((app_func_arg_t*)chip->os_func_data)->dev_lock;
        // SPI bus lock is possible not used on SPI1 bus
        if (dev_lock) {
            spi_bus_lock_unregister_dev(dev_lock);
        }
        free(chip->os_func_data);
    }
    chip->os_func = NULL;
    chip->os_func_data = NULL;
    return ESP_OK;
}

esp_err_t esp_flash_init_main_bus_lock(void)
{
    spi_bus_lock_init_main_bus();
    spi_bus_lock_set_bg_control(g_main_spi_bus_lock, cache_enable, cache_disable, NULL);

    esp_err_t err = spi_bus_lock_init_main_dev();
    if (err != ESP_OK) {
        return err;
    }
    return ESP_OK;
}

esp_err_t esp_flash_app_enable_os_functions(esp_flash_t* chip)
{
    main_flash_arg = (spi1_app_func_arg_t) {
        .common_arg = {
            .dev_lock = g_spi_lock_main_flash_dev,   //for SPI1,
        },
        .no_protect = false,
    };
    chip->os_func = &esp_flash_spi1_default_os_functions;
    chip->os_func_data = &main_flash_arg;
    return ESP_OK;
}
