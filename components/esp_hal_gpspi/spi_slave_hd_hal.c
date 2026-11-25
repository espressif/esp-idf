/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for SPI Slave HD

#include <string.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "soc/spi_periph.h"
#include "hal/spi_slave_hd_hal.h"
#include "hal/assert.h"

void spi_slave_hd_hal_init(spi_slave_hd_hal_context_t *hal, const spi_slave_hd_hal_config_t *hal_config)
{
    spi_dev_t *hw = spi_periph_signal[hal_config->host_id].hw;
    hal->dev = hw;

    spi_ll_slave_hd_init(hw);
    spi_ll_set_addr_bitlen(hw, hal_config->address_bits);
    spi_ll_set_command_bitlen(hw, hal_config->command_bits);
    spi_ll_set_dummy(hw, hal_config->dummy_bits);
    spi_ll_set_rx_lsbfirst(hw, hal_config->rx_lsbfirst);
    spi_ll_set_tx_lsbfirst(hw, hal_config->tx_lsbfirst);
    spi_ll_set_sio_mode(hw, hal_config->three_wire_mode);
    spi_ll_slave_set_mode(hw, hal_config->mode, (hal_config->dma_enabled));

    spi_ll_disable_intr(hw, UINT32_MAX);
    spi_ll_clear_intr(hw, UINT32_MAX);
    if (!hal_config->append_mode) {
        spi_ll_set_intr(hw, SPI_LL_INTR_CMD7 | SPI_LL_INTR_CMD8);

        bool workaround_required = false;
        if (!spi_ll_get_intr(hw, SPI_LL_INTR_CMD7)) {
            hal->intr_not_triggered |= SPI_EV_RECV;
            workaround_required = true;
        }
        if (!spi_ll_get_intr(hw, SPI_LL_INTR_CMD8)) {
            hal->intr_not_triggered |= SPI_EV_SEND;
            workaround_required = true;
        }

        if (workaround_required) {
            //Workaround if the previous interrupts are not writable
            spi_ll_set_intr(hw, SPI_LL_INTR_TRANS_DONE);
        }
    } else {
#if !SOC_GDMA_SUPPORTED
        spi_ll_enable_intr(hw, SPI_LL_INTR_CMD7 | SPI_LL_INTR_CMD8);
#endif //SOC_GDMA_SUPPORTED
    }

    spi_ll_slave_hd_set_len_cond(hw,    SPI_LL_TRANS_LEN_COND_WRBUF |
                                 SPI_LL_TRANS_LEN_COND_WRDMA |
                                 SPI_LL_TRANS_LEN_COND_RDBUF |
                                 SPI_LL_TRANS_LEN_COND_RDDMA);

    spi_ll_slave_set_seg_mode(hal->dev, true);
}

void spi_slave_hd_hal_read_buffer(spi_slave_hd_hal_context_t *hal, int addr, uint8_t *out_data, size_t len)
{
    spi_ll_read_buffer_byte(hal->dev, addr, out_data, len);
}

void spi_slave_hd_hal_write_buffer(spi_slave_hd_hal_context_t *hal, int addr, uint8_t *data, size_t len)
{
    spi_ll_write_buffer_byte(hal->dev, addr, data, len);
}

int spi_slave_hd_hal_get_last_addr(spi_slave_hd_hal_context_t *hal)
{
    return spi_ll_slave_hd_get_last_addr(hal->dev);
}

int spi_slave_hd_hal_get_rxlen(spi_slave_hd_hal_context_t *hal)
{
    //this is by -byte
    return spi_ll_slave_get_rx_byte_len(hal->dev);
}
