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

// HAL for
//  - GPSPI (SP2, SPI3) on ESP32-S2 and later
// The common part is in spi_flash_hal_common.inc

#define GPSPI_BUILD

#define spi_flash_hal_common_command            spi_flash_hal_gpspi_common_command
#define spi_flash_hal_poll_cmd_done             spi_flash_hal_gpspi_poll_cmd_done
#define spi_flash_hal_device_config             spi_flash_hal_gpspi_device_config
#define spi_flash_hal_configure_host_io_mode    spi_flash_hal_gpspi_configure_host_io_mode
#define spi_flash_hal_read                      spi_flash_hal_gpspi_read

#include "spi_flash_hal_common.inc"

bool spi_flash_hal_gpspi_supports_direct_write(spi_flash_host_inst_t *host, const void *p)
{
    return true;
}

bool spi_flash_hal_gpspi_supports_direct_read(spi_flash_host_inst_t *host, const void *p)
{
    return true;
}

uint32_t spi_flash_hal_gpspi_check_status(spi_flash_host_inst_t *host)
{
    spi_dev_t *dev = get_spi_dev(host);
    return spi_flash_ll_host_idle(dev);
}
