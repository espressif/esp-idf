/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for SPI (common part, in iram)
// make these functions in a seperate file to make sure all LL functions are in the IRAM.

#include "hal/spi_hal.h"
#include "hal/assert.h"
#include "soc/soc_caps.h"

//This GDMA related part will be introduced by GDMA dedicated APIs in the future. Here we temporarily use macros.
#if SOC_GDMA_SUPPORTED
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"

#define spi_dma_ll_rx_reset(dev, chan)                             gdma_ll_rx_reset_channel(&GDMA, chan)
#define spi_dma_ll_tx_reset(dev, chan)                             gdma_ll_tx_reset_channel(&GDMA, chan);
#define spi_dma_ll_rx_start(dev, chan, addr) do {\
            gdma_ll_rx_set_desc_addr(&GDMA, chan, (uint32_t)addr);\
            gdma_ll_rx_start(&GDMA, chan);\
        } while (0)
#define spi_dma_ll_tx_start(dev, chan, addr) do {\
            gdma_ll_tx_set_desc_addr(&GDMA, chan, (uint32_t)addr);\
            gdma_ll_tx_start(&GDMA, chan);\
        } while (0)
#endif

void spi_hal_setup_device(spi_hal_context_t *hal, const spi_hal_dev_config_t *dev)
{
    //Configure clock settings
    spi_dev_t *hw = hal->hw;
#if SOC_SPI_AS_CS_SUPPORTED
    spi_ll_master_set_cksel(hw, dev->cs_pin_id, dev->as_cs);
#endif
    spi_ll_master_set_pos_cs(hw, dev->cs_pin_id, dev->positive_cs);
    spi_ll_master_set_clock_by_reg(hw, &dev->timing_conf.clock_reg);
    spi_ll_master_set_rx_timing_mode(hw, dev->timing_conf.rx_sample_point);
    spi_ll_set_clk_source(hw, dev->timing_conf.clock_source);
    //Configure bit order
    spi_ll_set_rx_lsbfirst(hw, dev->rx_lsbfirst);
    spi_ll_set_tx_lsbfirst(hw, dev->tx_lsbfirst);
    spi_ll_master_set_mode(hw, dev->mode);
    //Configure misc stuff
    spi_ll_set_half_duplex(hw, dev->half_duplex);
    spi_ll_set_sio_mode(hw, dev->sio);
    //Configure CS pin and timing
    spi_ll_master_set_cs_setup(hw, dev->cs_setup);
    spi_ll_master_set_cs_hold(hw, dev->cs_hold);
    spi_ll_master_select_cs(hw, dev->cs_pin_id);
}

void spi_hal_setup_trans(spi_hal_context_t *hal, const spi_hal_dev_config_t *dev, const spi_hal_trans_config_t *trans)
{
    spi_dev_t *hw = hal->hw;

    //clear int bit
    spi_ll_clear_int_stat(hal->hw);
    //We should be done with the transmission.
    HAL_ASSERT(spi_ll_get_running_cmd(hw) == 0);
    //set transaction line mode
    spi_ll_master_set_line_mode(hw, trans->line_mode);

    int extra_dummy = 0;
    //when no_dummy is not set and in half-duplex mode, sets the dummy bit if RX phase exist
    if (trans->rcv_buffer && !dev->no_compensate && dev->half_duplex) {
        extra_dummy = dev->timing_conf.timing_dummy;
    }

    //SPI iface needs to be configured for a delay in some cases.
    //configure dummy bits
    spi_ll_set_dummy(hw, extra_dummy + trans->dummy_bits);

    uint32_t miso_delay_num = 0;
    uint32_t miso_delay_mode = 0;
    if (dev->timing_conf.timing_miso_delay < 0) {
        //if the data comes too late, delay half a SPI clock to improve reading
        switch (dev->mode) {
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
        miso_delay_num = extra_dummy ? dev->timing_conf.timing_miso_delay : 0;
        miso_delay_mode = 0;
    }
    spi_ll_set_miso_delay(hw, miso_delay_mode, miso_delay_num);

    spi_ll_set_mosi_bitlen(hw, trans->tx_bitlen);

    if (dev->half_duplex) {
        spi_ll_set_miso_bitlen(hw, trans->rx_bitlen);
    } else {
        //rxlength is not used in full-duplex mode
        spi_ll_set_miso_bitlen(hw, trans->tx_bitlen);
    }

    //Configure bit sizes, load addr and command
    int cmdlen = trans->cmd_bits;
    int addrlen = trans->addr_bits;
    if (!dev->half_duplex && dev->cs_setup != 0) {
        /* The command and address phase is not compatible with cs_ena_pretrans
         * in full duplex mode.
         */
        cmdlen = 0;
        addrlen = 0;
    }

    spi_ll_set_addr_bitlen(hw, addrlen);
    spi_ll_set_command_bitlen(hw, cmdlen);

    spi_ll_set_command(hw, trans->cmd, cmdlen, dev->tx_lsbfirst);
    spi_ll_set_address(hw, trans->addr, addrlen, dev->tx_lsbfirst);

    //Configure keep active CS
    spi_ll_master_keep_cs(hw, trans->cs_keep_active);

    //Save the transaction attributes for internal usage.
    memcpy(&hal->trans_config, trans, sizeof(spi_hal_trans_config_t));
}

void spi_hal_prepare_data(spi_hal_context_t *hal, const spi_hal_dev_config_t *dev, const spi_hal_trans_config_t *trans)
{
    spi_dev_t *hw = hal->hw;

    //Fill DMA descriptors
    if (trans->rcv_buffer) {
        if (!hal->dma_enabled) {
            //No need to setup anything; we'll copy the result out of the work registers directly later.
        } else {
            lldesc_setup_link(hal->dmadesc_rx, trans->rcv_buffer, ((trans->rx_bitlen + 7) / 8), true);

            spi_dma_ll_rx_reset(hal->dma_in, hal->rx_dma_chan);
            spi_ll_dma_rx_fifo_reset(hal->hw);
            spi_ll_infifo_full_clr(hal->hw);
            spi_ll_dma_rx_enable(hal->hw, 1);
            spi_dma_ll_rx_start(hal->dma_in, hal->rx_dma_chan, hal->dmadesc_rx);
        }

    }
#if CONFIG_IDF_TARGET_ESP32
    else {
        //DMA temporary workaround: let RX DMA work somehow to avoid the issue in ESP32 v0/v1 silicon
        if (hal->dma_enabled && !dev->half_duplex) {
            spi_ll_dma_rx_enable(hal->hw, 1);
            spi_dma_ll_rx_start(hal->dma_in, hal->rx_dma_chan, 0);
        }
    }
#endif

    if (trans->send_buffer) {
        if (!hal->dma_enabled) {
            //Need to copy data to registers manually
            spi_ll_write_buffer(hw, trans->send_buffer, trans->tx_bitlen);
        } else {
            lldesc_setup_link(hal->dmadesc_tx, trans->send_buffer, (trans->tx_bitlen + 7) / 8, false);

            spi_dma_ll_tx_reset(hal->dma_out, hal->tx_dma_chan);
            spi_ll_dma_tx_fifo_reset(hal->hw);
            spi_ll_outfifo_empty_clr(hal->hw);
            spi_ll_dma_tx_enable(hal->hw, 1);
            spi_dma_ll_tx_start(hal->dma_out, hal->tx_dma_chan, hal->dmadesc_tx);
        }
    }

    //in ESP32 these registers should be configured after the DMA is set
    if ((!dev->half_duplex && trans->rcv_buffer) || trans->send_buffer) {
        spi_ll_enable_mosi(hw, 1);
    } else {
        spi_ll_enable_mosi(hw, 0);
    }
    spi_ll_enable_miso(hw, (trans->rcv_buffer) ? 1 : 0);
}

void spi_hal_user_start(const spi_hal_context_t *hal)
{
    spi_ll_apply_config(hal->hw);
    spi_ll_user_start(hal->hw);
}

bool spi_hal_usr_is_done(const spi_hal_context_t *hal)
{
    return spi_ll_usr_is_done(hal->hw);
}

void spi_hal_fetch_result(const spi_hal_context_t *hal)
{
    const spi_hal_trans_config_t *trans = &hal->trans_config;

    if (trans->rcv_buffer && !hal->dma_enabled) {
        //Need to copy from SPI regs to result buffer.
        spi_ll_read_buffer(hal->hw, trans->rcv_buffer, trans->rx_bitlen);
    }
}
