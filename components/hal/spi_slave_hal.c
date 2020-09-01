#include "hal/spi_slave_hal.h"
#include "hal/spi_ll.h"

void spi_slave_hal_init(spi_slave_hal_context_t *hal, int host_id)
{
    memset(hal, 0, sizeof(spi_slave_hal_context_t));
    spi_dev_t *hw = spi_periph_signal[host_id].hw;
    hal->hw = hw;

    spi_ll_slave_init(hal->hw);

    //Force a transaction done interrupt. This interrupt won't fire yet because we initialized the SPI interrupt as
    //disabled.  This way, we can just enable the SPI interrupt and the interrupt handler will kick in, handling
    //any transactions that are queued.
    spi_ll_set_int_stat(hal->hw);
    spi_ll_slave_set_int_type(hal->hw, SPI_LL_INT_TYPE_NORMAL);
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