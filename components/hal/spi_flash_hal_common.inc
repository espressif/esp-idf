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

#include <string.h>
#include <stdlib.h>
#include "hal/spi_flash_hal.h"
#include "hal/assert.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#define ADDRESS_MASK_24BIT 0xFFFFFF
#define COMPUTE_DUMMY_CYCLELEN(host, base)    ((base) + ((spi_flash_hal_context_t*)host)->extra_dummy)

static inline spi_dev_t *get_spi_dev(spi_flash_host_inst_t *host)
{
    return ((spi_flash_hal_context_t*)host)->spi;
}

static inline int get_host_id(spi_flash_host_inst_t* host)
{
    spi_dev_t *dev = get_spi_dev(host);
    return spi_flash_ll_hw_get_id(dev);
}

void spi_flash_hal_poll_cmd_done(spi_flash_host_inst_t *host)
{
    while (!spi_flash_ll_cmd_is_done(get_spi_dev(host))) {
        //nop
    }
}

esp_err_t spi_flash_hal_device_config(spi_flash_host_inst_t *host)
{
    spi_flash_hal_context_t* ctx = (spi_flash_hal_context_t*)host;
    spi_dev_t *dev = get_spi_dev(host);

    spi_flash_ll_reset(dev);
    spi_flash_ll_set_cs_pin(dev, ctx->cs_num);
    spi_flash_ll_set_clock(dev, &ctx->clock_conf);
    int cs_hold = ctx->cs_hold;
    spi_flash_ll_set_hold(dev, cs_hold);
    spi_flash_ll_set_cs_setup(dev, ctx->cs_setup);

#ifndef GPSPI_BUILD
#if SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
    if ((ctx->flags & SPI_FLASH_HOST_CONTEXT_FLAG_AUTO_SUSPEND) != 0) {
        spi_flash_hal_setup_auto_suspend_mode(host);
    } else {
        spi_flash_hal_disable_auto_suspend_mode(host);
    }
    if ((ctx->flags & SPI_FLASH_HOST_CONTEXT_FLAG_AUTO_RESUME) != 0) {
        spi_flash_hal_setup_auto_resume_mode(host);
    } else {
        spi_flash_hal_disable_auto_resume_mode(host);
    }
#endif //SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND
#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
    // Always keep the extra dummy on SPI1 is 0, add extra dummy to user dummy
    spimem_flash_ll_set_extra_dummy((spi_mem_dev_t*)dev, 0);
#endif
#else
    gpspi_flash_ll_set_hold_pol(dev, 1);
#endif //GPSPI_BUILD

    return ESP_OK;
}

esp_err_t spi_flash_hal_configure_host_io_mode(
    spi_flash_host_inst_t *host,
    uint32_t command,
    uint32_t addr_bitlen,
    int dummy_cyclelen_base,
    esp_flash_io_mode_t io_mode)
{
    spi_dev_t *dev = get_spi_dev(host);
    int host_id = spi_flash_ll_hw_get_id(dev);

    uint32_t extra_bits = io_mode & 0xFFFF0000;
    io_mode = io_mode & 0xFFFF;

    /*
     * Some flash chips, when working under some IO modes (DIO, QIO and OIO in the future), treat
     * the first 8 bits of the dummy bits as the bits. When the bits meet some pattern, the chip
     * will go into a "continuous (XIP)" mode, where the command field will be skipped in the next
     * transaction. We have to output all ones in these cycles because we don't need this feature.
     */
    bool conf_required = ((extra_bits & SPI_FLASH_CONFIG_CONF_BITS) != 0);

    if (!SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id) && io_mode > SPI_FLASH_FASTRD) {
        return ESP_ERR_NOT_SUPPORTED;
    }

#if SOC_SPI_PERIPH_SUPPORT_CONTROL_DUMMY_OUT
    // The CONTROL_DUMMY_OUTPUT feature is used to control M7-M0 bits.
    spi_flash_ll_set_dummy_out(dev, (conf_required? 1: 0), 1);
#else
    /**
     * - On current chips, addr phase can support 32 bits at most.
     * - Flash chip requires continuous mode bits
     *
     * We send continuous mode bits via the dummy output feature, so as to support
     * 32-bit address.
     *
     * On chips without dummy output feature (ESP32, ESP32C6), we fallback to use
     * addr phase to send the continuous mode bits:
     * - On ESP32 (QIO), qio_dummy: 6 - 4 / 4 = 5, addr_bitlen: 24 + 4 = 28. (This
     *   setting exists for long time, we keep this on ESP32)
     * - On ESP32C6 (QIO), qio_dummy: 6 - 8 / 4 = 4, addr_bitlen: 24 + 8 = 32
     * - On future chips without dummy output feature, we follow the ESP32C6 (QIO)
     *   way.
     * - Above two ways, the timings are same.
     * - DIO is similar.
     */
    if (conf_required) {
        int line_width = (io_mode == SPI_FLASH_DIO? 2: 4);
        dummy_cyclelen_base -= SPI_FLASH_LL_CONTINUOUS_MODE_BIT_NUMS / line_width;
        addr_bitlen += SPI_FLASH_LL_CONTINUOUS_MODE_BIT_NUMS;
        spi_flash_ll_set_extra_address(dev, 0);
    }
#endif

    if (command >= 0x100) {
        spi_flash_ll_set_command(dev, command, 16);
    } else {
        spi_flash_ll_set_command(dev, command, 8);
    }
    spi_flash_ll_set_addr_bitlen(dev, addr_bitlen);
    // Add dummy cycles to compensate for latency of GPIO matrix and external delay, if necessary...
    spi_flash_ll_set_dummy(dev, COMPUTE_DUMMY_CYCLELEN(host, dummy_cyclelen_base));
    //disable all data phases, enable them later if needed
    spi_flash_ll_set_miso_bitlen(dev, 0);
    spi_flash_ll_set_mosi_bitlen(dev, 0);
    spi_flash_ll_set_read_mode(dev, io_mode);
    return ESP_OK;
}

esp_err_t spi_flash_hal_common_command(spi_flash_host_inst_t *host, spi_flash_trans_t *trans)
{
    spi_dev_t *dev = get_spi_dev(host);
    esp_flash_io_mode_t io_mode = ((spi_flash_hal_context_t*)host)->base_io_mode;
    uint16_t command;
    uint8_t dummy_bitlen;
    bool pe_ops = false;

    command = trans->command;
    dummy_bitlen = trans->dummy_bitlen;
    if ((trans->flags & SPI_FLASH_TRANS_FLAG_IGNORE_BASEIO) != 0) {
        io_mode = trans->io_mode;
    }

    host->driver->configure_host_io_mode(host, command, trans->address_bitlen, dummy_bitlen, io_mode);

    spi_flash_ll_set_usr_address(dev, trans->address, trans->address_bitlen);
    //No extra dummy cycles for compensation if no input data
    if (trans->miso_len == 0) {
        spi_flash_ll_set_dummy(dev, dummy_bitlen);
    }

    spi_flash_ll_set_mosi_bitlen(dev, trans->mosi_len * 8);
    spi_flash_ll_set_buffer_data(dev, trans->mosi_data, trans->mosi_len);

    spi_flash_ll_set_miso_bitlen(dev, trans->miso_len * 8);
    if ((trans->flags & SPI_FLASH_TRANS_FLAG_PE_CMD) != 0) {
        pe_ops = true;
    }
    spi_flash_ll_user_start(dev, pe_ops);
    host->driver->poll_cmd_done(host);
    if (trans->miso_len > 0) {
        spi_flash_ll_get_buffer_data(dev, trans->miso_data, trans->miso_len);
    }
    return ESP_OK;
}

esp_err_t spi_flash_hal_read(spi_flash_host_inst_t *host, void *buffer, uint32_t address, uint32_t read_len)
{
    spi_dev_t *dev = get_spi_dev(host);
    int bitlen = spi_flash_ll_get_addr_bitlen(dev);
    //Only 24-bit and 32-bit address are supported. The extra length are for M7-M0, which should be
    //filled with ones by the function below
    spi_flash_ll_set_usr_address(dev, address, bitlen & (~7));
    spi_flash_ll_set_miso_bitlen(dev, read_len * 8);
    spi_flash_ll_user_start(dev, false);
    host->driver->poll_cmd_done(host);
    if (read_len > 0) {
        spi_flash_ll_get_buffer_data(dev, buffer, read_len);
    }
    return ESP_OK;
}
