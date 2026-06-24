/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_private/spi_share_hw_ctrl.h"

#include <stdatomic.h>

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/spi_ll.h"
#include "esp_log.h"

ESP_LOG_ATTR_TAG(SPI_TAG, "spi_share_hw_ctrl");

//Periph 1 is 'claimed' by SPI flash code.
static atomic_bool spi_periph_claimed[SOC_SPI_PERIPH_NUM] = { ATOMIC_VAR_INIT(true), ATOMIC_VAR_INIT(false),
#if (SOC_SPI_PERIPH_NUM >= 3)
                                                              ATOMIC_VAR_INIT(false),
#endif
#if (SOC_SPI_PERIPH_NUM >= 4)
                                                              ATOMIC_VAR_INIT(false),
#endif
                                                            };

static const char* spi_claiming_func[3] = {NULL, NULL, NULL};

//----------------------------------------------------------alloc spi periph-------------------------------------------------------//
//Returns true if this peripheral is successfully claimed, false if otherwise.
bool spicommon_periph_claim(spi_host_device_t host, const char* source)
{
    bool false_var = false;
    bool ret = atomic_compare_exchange_strong(&spi_periph_claimed[host], &false_var, true);
    if (ret) {
        spi_claiming_func[host] = source;
        PERIPH_RCC_ATOMIC() {
            spi_ll_enable_bus_clock(host, true);
            spi_ll_reset_register(host);
        }
    } else {
        ESP_EARLY_LOGE(SPI_TAG, "SPI%d already claimed by %s.", host + 1, spi_claiming_func[host]);
    }
    return ret;
}

bool spicommon_periph_in_use(spi_host_device_t host)
{
    return atomic_load(&spi_periph_claimed[host]);
}

//Returns true if this peripheral is successfully freed, false if otherwise.
bool spicommon_periph_free(spi_host_device_t host)
{
    bool true_var = true;
    bool ret = atomic_compare_exchange_strong(&spi_periph_claimed[host], &true_var, false);
    if (ret) {
        PERIPH_RCC_ATOMIC() {
            spi_ll_enable_bus_clock(host, false);
        }
    }
    return ret;
}
