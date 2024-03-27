/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for SPI (common part, in iram)
// make these functions in a seperate file to make sure all LL functions are in the IRAM.

#include "hal/spi_hal.h"
#include "hal/assert.h"
#include "soc/ext_mem_defs.h"
#include "soc/soc_caps.h"

void spi_hal_setup_device(spi_hal_context_t *hal, const spi_hal_dev_config_t *dev)
{
    //Configure clock settings
    spi_dev_t *hw = hal->hw;
#if SOC_SPI_AS_CS_SUPPORTED
    spi_ll_master_set_cksel(hw, dev->cs_pin_id, dev->as_cs);
#endif
    spi_ll_master_set_pos_cs(hw, dev->cs_pin_id, dev->positive_cs);
    spi_ll_master_set_clock_by_reg(hw, &dev->timing_conf.clock_reg);
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

void spi_hal_enable_data_line(spi_dev_t *hw, bool mosi_ena, bool miso_ena)
{
    spi_ll_enable_mosi(hw, mosi_ena);
    spi_ll_enable_miso(hw, miso_ena);
}

void spi_hal_hw_prepare_rx(spi_dev_t *hw)
{
    spi_ll_dma_rx_fifo_reset(hw);
    spi_ll_infifo_full_clr(hw);
    spi_ll_dma_rx_enable(hw, 1);
}

void spi_hal_hw_prepare_tx(spi_dev_t *hw)
{
    spi_ll_dma_tx_fifo_reset(hw);
    spi_ll_outfifo_empty_clr(hw);
    spi_ll_dma_tx_enable(hw, 1);
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

void spi_hal_push_tx_buffer(const spi_hal_context_t *hal, const spi_hal_trans_config_t *hal_trans)
{
    if (hal_trans->send_buffer) {
        spi_ll_write_buffer(hal->hw, hal_trans->send_buffer, hal_trans->tx_bitlen);
    }
    //No need to setup anything for RX, we'll copy the result out of the work registers directly later.
}

void spi_hal_fetch_result(const spi_hal_context_t *hal)
{
    const spi_hal_trans_config_t *trans = &hal->trans_config;

    if (trans->rcv_buffer) {
        //Need to copy from SPI regs to result buffer.
        spi_ll_read_buffer(hal->hw, trans->rcv_buffer, trans->rx_bitlen);
    }
}

#if SOC_SPI_SCT_SUPPORTED
/*------------------------------------------------------------------------------
 * Segmented-Configure-Transfer
 *----------------------------------------------------------------------------*/
void spi_hal_clear_intr_mask(spi_hal_context_t *hal, uint32_t mask) {
    spi_ll_clear_intr(hal->hw, mask);
}

bool spi_hal_get_intr_mask(spi_hal_context_t *hal, uint32_t mask) {
    return spi_ll_get_intr(hal->hw, mask);
}

void spi_hal_sct_set_conf_bits_len(spi_hal_context_t *hal, uint32_t conf_len) {
    spi_ll_set_conf_phase_bits_len(hal->hw, conf_len);
}

void spi_hal_sct_init_conf_buffer(spi_hal_context_t *hal, uint32_t conf_buffer[SOC_SPI_SCT_BUFFER_NUM_MAX])
{
    spi_ll_init_conf_buffer(hal->hw, conf_buffer);
}

void spi_hal_sct_format_conf_buffer(spi_hal_context_t *hal, const spi_hal_seg_config_t *config, const spi_hal_dev_config_t *dev, uint32_t conf_buffer[SOC_SPI_SCT_BUFFER_NUM_MAX])
{
    spi_ll_format_line_mode_conf_buff(hal->hw, hal->trans_config.line_mode, conf_buffer);
    spi_ll_format_prep_phase_conf_buffer(hal->hw, config->cs_setup, conf_buffer);
    spi_ll_format_cmd_phase_conf_buffer(hal->hw, config->cmd, config->cmd_bits, dev->tx_lsbfirst, conf_buffer);
    spi_ll_format_addr_phase_conf_buffer(hal->hw, config->addr, config->addr_bits, dev->rx_lsbfirst, conf_buffer);
    spi_ll_format_dummy_phase_conf_buffer(hal->hw, config->dummy_bits, conf_buffer);
    spi_ll_format_dout_phase_conf_buffer(hal->hw, config->tx_bitlen, conf_buffer);
    spi_ll_format_din_phase_conf_buffer(hal->hw, config->rx_bitlen, conf_buffer);
    spi_ll_format_done_phase_conf_buffer(hal->hw, config->cs_hold, conf_buffer);
    spi_ll_format_conf_phase_conf_buffer(hal->hw, config->seg_end, conf_buffer);
#if CONFIG_IDF_TARGET_ESP32S2
    // only s2 support update seg_gap_len by conf_buffer
    spi_ll_format_conf_bitslen_buffer(hal->hw, config->seg_gap_len, conf_buffer);
#endif
}

#endif  //#if SOC_SPI_SCT_SUPPORTED
