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

// The HAL layer for SPI (common part, in iram)
// make these functions in a seperate file to make sure all LL functions are in the IRAM.

#include "hal/spi_hal.h"

void spi_hal_setup_device(const spi_hal_context_t *hal)
{
    //Configure clock settings
    spi_dev_t *hw = hal->hw;
#ifdef SOC_SPI_SUPPORT_AS_CS
    spi_ll_master_set_cksel(hw, hal->cs_pin_id, hal->as_cs);
#endif
    spi_ll_master_set_pos_cs(hw, hal->cs_pin_id, hal->positive_cs);
    spi_ll_master_set_clock_by_reg(hw, &hal->timing_conf->clock_reg);
    //Configure bit order
    spi_ll_set_rx_lsbfirst(hw, hal->rx_lsbfirst);
    spi_ll_set_tx_lsbfirst(hw, hal->tx_lsbfirst);
    spi_ll_master_set_mode(hw, hal->mode);
    //Configure misc stuff
    spi_ll_set_half_duplex(hw, hal->half_duplex);
    spi_ll_set_sio_mode(hw, hal->sio);
    //Configure CS pin and timing
    spi_ll_master_set_cs_setup(hw, hal->cs_setup);
    spi_ll_master_set_cs_hold(hw, hal->cs_hold);
    spi_ll_master_select_cs(hw, hal->cs_pin_id);
}

void spi_hal_setup_trans(const spi_hal_context_t *hal)
{
    spi_dev_t *hw = hal->hw;

    //clear int bit
    spi_ll_clear_int_stat(hal->hw);
    //We should be done with the transmission.
    assert(spi_ll_get_running_cmd(hw) == 0);

    spi_ll_master_set_io_mode(hw, hal->io_mode);

    int extra_dummy = 0;
    //when no_dummy is not set and in half-duplex mode, sets the dummy bit if RX phase exist
    if (hal->rcv_buffer && !hal->no_compensate && hal->half_duplex) {
        extra_dummy = hal->timing_conf->timing_dummy;
    }

    //SPI iface needs to be configured for a delay in some cases.
    //configure dummy bits
    spi_ll_set_dummy(hw, extra_dummy + hal->dummy_bits);

    uint32_t miso_delay_num = 0;
    uint32_t miso_delay_mode = 0;
    if (hal->timing_conf->timing_miso_delay < 0) {
        //if the data comes too late, delay half a SPI clock to improve reading
        switch (hal->mode) {
        case 0:
            miso_delay_mode = 2;
            break;
        case 1:
            miso_delay_mode = 1;
            break;
        case 2:
            miso_delay_mode = 1;
            break;
        case 3:
            miso_delay_mode = 2;
            break;
        }
        miso_delay_num = 0;
    } else {
        //if the data is so fast that dummy_bit is used, delay some apb clocks to meet the timing
        miso_delay_num = extra_dummy ? hal->timing_conf->timing_miso_delay : 0;
        miso_delay_mode = 0;
    }
    spi_ll_set_miso_delay(hw, miso_delay_mode, miso_delay_num);

    spi_ll_set_mosi_bitlen(hw, hal->tx_bitlen);

    if (hal->half_duplex) {
        spi_ll_set_miso_bitlen(hw, hal->rx_bitlen);
    } else {
        //rxlength is not used in full-duplex mode
        spi_ll_set_miso_bitlen(hw, hal->tx_bitlen);
    }

    //Configure bit sizes, load addr and command
    int cmdlen = hal->cmd_bits;
    int addrlen = hal->addr_bits;
    if (!hal->half_duplex && hal->cs_setup != 0) {
        /* The command and address phase is not compatible with cs_ena_pretrans
         * in full duplex mode.
         */
        cmdlen = 0;
        addrlen = 0;
    }

    spi_ll_set_addr_bitlen(hw, addrlen);
    spi_ll_set_command_bitlen(hw, cmdlen);

    spi_ll_set_command(hw, hal->cmd, cmdlen, hal->tx_lsbfirst);
    spi_ll_set_address(hw, hal->addr, addrlen, hal->tx_lsbfirst);
}

void spi_hal_prepare_data(const spi_hal_context_t *hal)
{
    spi_dev_t *hw = hal->hw;
    spi_ll_reset_dma(hw);
    //Fill DMA descriptors
    if (hal->rcv_buffer) {
        if (!hal->dma_enabled) {
            //No need to setup anything; we'll copy the result out of the work registers directly later.
        } else {
            lldesc_setup_link(hal->dmadesc_rx, hal->rcv_buffer, ((hal->rx_bitlen + 7) / 8), true);
            spi_ll_rxdma_start(hw, hal->dmadesc_rx);
        }
    } else {
        //DMA temporary workaround: let RX DMA work somehow to avoid the issue in ESP32 v0/v1 silicon
        if (hal->dma_enabled) {
            spi_ll_rxdma_start(hw, 0);
        }
    }

    if (hal->send_buffer) {
        if (!hal->dma_enabled) {
            //Need to copy data to registers manually
            spi_ll_write_buffer(hw, hal->send_buffer, hal->tx_bitlen);
        } else {
            lldesc_setup_link(hal->dmadesc_tx, hal->send_buffer, (hal->tx_bitlen + 7) / 8, false);
            spi_ll_txdma_start(hw, hal->dmadesc_tx);
        }
    }
    //in ESP32 these registers should be configured after the DMA is set
    if ((!hal->half_duplex && hal->rcv_buffer) || hal->send_buffer) {
        spi_ll_enable_mosi(hw, 1);
    } else {
        spi_ll_enable_mosi(hw, 0);
    }
    spi_ll_enable_miso(hw, (hal->rcv_buffer) ? 1 : 0);
}

void spi_hal_user_start(const spi_hal_context_t *hal)
{
    spi_ll_user_start(hal->hw);
}

bool spi_hal_usr_is_done(const spi_hal_context_t *hal)
{
    return spi_ll_usr_is_done(hal->hw);
}

void spi_hal_fetch_result(const spi_hal_context_t *hal)
{
    if (hal->rcv_buffer && !hal->dma_enabled) {
        //Need to copy from SPI regs to result buffer.
        spi_ll_read_buffer(hal->hw, hal->rcv_buffer, hal->rx_bitlen);
    }
}
