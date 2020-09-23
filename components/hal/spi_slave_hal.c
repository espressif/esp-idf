#include "hal/spi_slave_hal.h"
#include "hal/spi_ll.h"
#include "soc/soc_caps.h"

#if SOC_GDMA_SUPPORTED
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"

#define spi_dma_ll_rx_reset(dev)                             gdma_ll_rx_reset_channel(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL)
#define spi_dma_ll_tx_reset(dev)                             gdma_ll_tx_reset_channel(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL);
#define spi_dma_ll_rx_enable_burst_data(dev, enable)         gdma_ll_rx_enable_data_burst(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_ll_tx_enable_burst_data(dev, enable)         gdma_ll_tx_enable_data_burst(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_ll_rx_enable_burst_desc(dev, enable)         gdma_ll_rx_enable_descriptor_burst(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_ll_tx_enable_burst_desc(dev, enable)         gdma_ll_tx_enable_descriptor_burst(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_set_rx_channel_priority(dev, priority)       gdma_ll_rx_set_priority(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, priority);
#define spi_dma_set_tx_channel_priority(dev, priority)       gdma_ll_tx_set_priority(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, priority);
#define spi_dma_enable_out_auto_wrback(dev, enable)          gdma_ll_tx_enable_auto_write_back(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_set_out_eof_generation(dev, enable)          gdma_ll_tx_set_eof_mode(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, enable);
#define spi_dma_connect_rx_channel_to_periph(dev, periph_id) gdma_ll_rx_connect_to_periph(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, periph_id);
#define spi_dma_connect_tx_channel_to_periph(dev, periph_id) gdma_ll_tx_connect_to_periph(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, periph_id);
#define spi_dma_ll_rx_start(dev, addr) do {\
            gdma_ll_rx_set_desc_addr(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, (uint32_t)addr);\
            gdma_ll_rx_start(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL);\
        } while (0)
#define spi_dma_ll_tx_start(dev, addr) do {\
            gdma_ll_tx_set_desc_addr(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL, (uint32_t)addr);\
            gdma_ll_tx_start(&GDMA, SOC_GDMA_SPI3_DMA_CHANNEL);\
        } while (0)

#endif

static void s_spi_slave_hal_dma_init_config(const spi_slave_hal_context_t *hal)
{
    spi_dma_ll_rx_enable_burst_data(hal->dma_in, 1);
    spi_dma_ll_tx_enable_burst_data(hal->dma_out, 1);
    spi_dma_ll_rx_enable_burst_desc(hal->dma_in, 1);
    spi_dma_ll_tx_enable_burst_desc(hal->dma_out, 1);
}

void spi_slave_hal_init(spi_slave_hal_context_t *hal, const spi_slave_hal_config_t *hal_config)
{
    memset(hal, 0, sizeof(spi_slave_hal_context_t));
    spi_dev_t *hw = SPI_LL_GET_HW(hal_config->host_id);
    hal->hw = hw;
    hal->dma_in = hal_config->dma_in;
    hal->dma_out = hal_config->dma_out;

    s_spi_slave_hal_dma_init_config(hal);
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