#include "hal/spi_slave_hal.h"
#include "hal/spi_ll.h"
#include "soc/soc_caps.h"

//This GDMA related part will be introduced by GDMA dedicated APIs in the future. Here we temporarily use macros.
#if SOC_GDMA_SUPPORTED
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"

#define spi_dma_ll_rx_enable_burst_data(dev, chan, enable)         gdma_ll_rx_enable_data_burst(&GDMA, chan, enable);
#define spi_dma_ll_tx_enable_burst_data(dev, chan, enable)         gdma_ll_tx_enable_data_burst(&GDMA, chan, enable);
#define spi_dma_ll_rx_enable_burst_desc(dev, chan, enable)         gdma_ll_rx_enable_descriptor_burst(&GDMA, chan, enable);
#define spi_dma_ll_tx_enable_burst_desc(dev, chan, enable)         gdma_ll_tx_enable_descriptor_burst(&GDMA, chan, enable);
#define spi_dma_ll_enable_out_auto_wrback(dev, chan, enable)          gdma_ll_tx_enable_auto_write_back(&GDMA, chan, enable);
#define spi_dma_ll_set_out_eof_generation(dev, chan, enable)          gdma_ll_tx_set_eof_mode(&GDMA, chan, enable);
#endif

static void s_spi_slave_hal_dma_init_config(const spi_slave_hal_context_t *hal)
{
    spi_dma_ll_rx_enable_burst_data(hal->dma_in, hal->rx_dma_chan, 1);
    spi_dma_ll_tx_enable_burst_data(hal->dma_out, hal->tx_dma_chan, 1);
    spi_dma_ll_rx_enable_burst_desc(hal->dma_in, hal->rx_dma_chan, 1);
    spi_dma_ll_tx_enable_burst_desc(hal->dma_out, hal->tx_dma_chan, 1);
}

void spi_slave_hal_init(spi_slave_hal_context_t *hal, const spi_slave_hal_config_t *hal_config)
{
    memset(hal, 0, sizeof(spi_slave_hal_context_t));
    spi_dev_t *hw = SPI_LL_GET_HW(hal_config->host_id);
    hal->hw = hw;
    hal->dma_in = hal_config->dma_in;
    hal->dma_out = hal_config->dma_out;

    if (hal->use_dma) {
        s_spi_slave_hal_dma_init_config(hal);
    }
    spi_ll_slave_init(hal->hw);

    //Force a transaction done interrupt. This interrupt won't fire yet because we initialized the SPI interrupt as
    //disabled.  This way, we can just enable the SPI interrupt and the interrupt handler will kick in, handling
    //any transactions that are queued.
    spi_ll_set_int_stat(hal->hw);
    spi_ll_enable_int(hal->hw);
}

void spi_slave_hal_setup_device(const spi_slave_hal_context_t *hal)
{
    spi_ll_set_rx_lsbfirst(hal->hw, hal->rx_lsbfirst);
    spi_ll_set_tx_lsbfirst(hal->hw, hal->tx_lsbfirst);
    spi_ll_slave_set_mode(hal->hw, hal->mode, hal->use_dma);
}

void spi_slave_hal_deinit(spi_slave_hal_context_t *hal)
{

}
