/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for SPI (common part)

#include "hal/spi_hal.h"
#include "hal/log.h"
#include "hal/assert.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

//This GDMA related part will be introduced by GDMA dedicated APIs in the future. Here we temporarily use macros.
#if SOC_GDMA_SUPPORTED
#if (SOC_GDMA_TRIG_PERIPH_SPI2_BUS == SOC_GDMA_BUS_AHB) && (SOC_AHB_GDMA_VERSION == 1)
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"
#define spi_dma_ll_rx_enable_burst_data(dev, chan, enable)         gdma_ll_rx_enable_data_burst(&GDMA, chan, enable);
#define spi_dma_ll_tx_enable_burst_data(dev, chan, enable)         gdma_ll_tx_enable_data_burst(&GDMA, chan, enable);
#define spi_dma_ll_rx_enable_burst_desc(dev, chan, enable)         gdma_ll_rx_enable_descriptor_burst(&GDMA, chan, enable);
#define spi_dma_ll_tx_enable_burst_desc(dev, chan, enable)         gdma_ll_tx_enable_descriptor_burst(&GDMA, chan, enable);
#define spi_dma_ll_enable_out_auto_wrback(dev, chan, enable)          gdma_ll_tx_enable_auto_write_back(&GDMA, chan, enable);
#define spi_dma_ll_set_out_eof_generation(dev, chan, enable)          gdma_ll_tx_set_eof_mode(&GDMA, chan, enable);

#elif (SOC_GDMA_TRIG_PERIPH_SPI2_BUS == SOC_GDMA_BUS_AXI)   //TODO: IDF-6152, refactor spi hal layer
#include "hal/axi_dma_ll.h"
#define spi_dma_ll_rx_enable_burst_data(dev, chan, enable)         axi_dma_ll_rx_enable_data_burst(&AXI_DMA, chan, enable);
#define spi_dma_ll_tx_enable_burst_data(dev, chan, enable)         axi_dma_ll_tx_enable_data_burst(&AXI_DMA, chan, enable);
#define spi_dma_ll_rx_enable_burst_desc(dev, chan, enable)         axi_dma_ll_rx_enable_descriptor_burst(&AXI_DMA, chan, enable);
#define spi_dma_ll_tx_enable_burst_desc(dev, chan, enable)         axi_dma_ll_tx_enable_descriptor_burst(&AXI_DMA, chan, enable);
#define spi_dma_ll_enable_out_auto_wrback(dev, chan, enable)          axi_dma_ll_tx_enable_auto_write_back(&AXI_DMA, chan, enable);
#define spi_dma_ll_set_out_eof_generation(dev, chan, enable)          axi_dma_ll_tx_set_eof_mode(&AXI_DMA, chan, enable);
#endif
#endif  //SOC_GDMA_SUPPORTED

/* The tag may be unused if log level is set to NONE  */
static const __attribute__((unused)) char SPI_HAL_TAG[] = "spi_hal";

#define SPI_HAL_CHECK(a, str, ret_val, ...) \
    if (!(a)) { \
        HAL_LOGE(SPI_HAL_TAG,"%s(%d): "str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

static void s_spi_hal_dma_init_config(const spi_hal_context_t *hal)
{
    spi_dma_ll_rx_enable_burst_data(hal->dma_in, hal->rx_dma_chan, 1);
    spi_dma_ll_tx_enable_burst_data(hal->dma_out, hal->tx_dma_chan, 1);
    spi_dma_ll_rx_enable_burst_desc(hal->dma_in, hal->rx_dma_chan, 1);
    spi_dma_ll_tx_enable_burst_desc(hal->dma_out, hal->tx_dma_chan, 1);
}

void spi_hal_init(spi_hal_context_t *hal, uint32_t host_id, const spi_hal_config_t *config)
{
    memset(hal, 0, sizeof(spi_hal_context_t));
    spi_dev_t *hw = SPI_LL_GET_HW(host_id);
    hal->hw = hw;
    hal->dma_in = config->dma_in;
    hal->dma_out = config->dma_out;
    hal->dma_enabled = config->dma_enabled;
    hal->dmadesc_tx = config->dmadesc_tx;
    hal->dmadesc_rx = config->dmadesc_rx;
    hal->tx_dma_chan = config->tx_dma_chan;
    hal->rx_dma_chan = config->rx_dma_chan;
    hal->dmadesc_n = config->dmadesc_n;

#if SPI_LL_MOSI_FREE_LEVEL
    // Change default data line level to low which same as esp32
    spi_ll_set_mosi_free_level(hw, 0);
#endif
    spi_ll_master_init(hw);
    if (config->dma_enabled) {
        s_spi_hal_dma_init_config(hal);
    }

    //Force a transaction done interrupt. This interrupt won't fire yet because
    //we initialized the SPI interrupt as disabled. This way, we can just
    //enable the SPI interrupt and the interrupt handler will kick in, handling
    //any transactions that are queued.
    spi_ll_enable_int(hw);
    spi_ll_set_int_stat(hw);
    spi_ll_set_mosi_delay(hw, 0, 0);
    spi_ll_apply_config(hw);
}

void spi_hal_deinit(spi_hal_context_t *hal)
{
    spi_dev_t *hw = hal->hw;
    if (hw) {
        spi_ll_disable_int(hw);
        spi_ll_clear_int_stat(hw);
    }
}

esp_err_t spi_hal_cal_clock_conf(const spi_hal_timing_param_t *timing_param, int *out_freq, spi_hal_timing_conf_t *timing_conf)
{
    spi_hal_timing_conf_t temp_conf = {};

    int eff_clk_n = spi_ll_master_cal_clock(timing_param->clk_src_hz, timing_param->expected_freq, timing_param->duty_cycle, &temp_conf.clock_reg);

    //When the speed is too fast, we may need to use dummy cycles to compensate the reading.
    //But these don't work for full-duplex connections.
    spi_hal_cal_timing(timing_param->clk_src_hz, eff_clk_n, timing_param->use_gpio, timing_param->input_delay_ns, &temp_conf.timing_dummy, &temp_conf.timing_miso_delay);

#ifdef CONFIG_IDF_TARGET_ESP32
    const int freq_limit = spi_hal_get_freq_limit(timing_param->use_gpio, timing_param->input_delay_ns);

    SPI_HAL_CHECK(timing_param->half_duplex || temp_conf.timing_dummy == 0 || timing_param->no_compensate,
                  "When work in full-duplex mode at frequency > %.1fMHz, device cannot read correct data.\n\
Try to use IOMUX pins to increase the frequency limit, or use the half duplex mode.\n\
Please note the SPI master can only work at divisors of 80MHz, and the driver always tries to find the closest frequency to your configuration.\n\
Specify ``SPI_DEVICE_NO_DUMMY`` to ignore this checking. Then you can output data at higher speed, or read data at your own risk.",
                  ESP_ERR_NOT_SUPPORTED, freq_limit / 1000. / 1000 );
#endif

    if (timing_conf) {
        *timing_conf = temp_conf;
    }
    if (out_freq) {
        *out_freq = eff_clk_n;
    }
    return ESP_OK;
}

int spi_hal_master_cal_clock(int fapb, int hz, int duty_cycle)
{
    return spi_ll_master_cal_clock(fapb, hz, duty_cycle, NULL);
}


void spi_hal_cal_timing(int source_freq_hz, int eff_clk, bool gpio_is_used, int input_delay_ns, int *dummy_n, int *miso_delay_n)
{
    const int apbclk_kHz = source_freq_hz / 1000;
    //how many apb clocks a period has
    const int spiclk_apb_n = source_freq_hz / eff_clk;
    const int gpio_delay_ns = gpio_is_used ? GPIO_MATRIX_DELAY_NS : 0;

    //how many apb clocks the delay is, the 1 is to compensate in case ``input_delay_ns`` is rounded off.
    int delay_apb_n = (1 + input_delay_ns + gpio_delay_ns) * apbclk_kHz / 1000 / 1000;
    if (delay_apb_n < 0) {
        delay_apb_n = 0;
    }

    int dummy_required = delay_apb_n / spiclk_apb_n;

    int miso_delay = 0;
    if (dummy_required > 0) {
        //due to the clock delay between master and slave, there's a range in which data is random
        //give MISO a delay if needed to make sure we sample at the time MISO is stable
        miso_delay = (dummy_required + 1) * spiclk_apb_n - delay_apb_n - 1;
    } else {
        //if the dummy is not required, maybe we should also delay half a SPI clock if the data comes too early
        if (delay_apb_n * 4 <= spiclk_apb_n) {
            miso_delay = -1;
        }
    }
    *dummy_n = dummy_required;
    *miso_delay_n = miso_delay;
    HAL_LOGD(SPI_HAL_TAG, "eff: %d, limit: %dk(/%d), %d dummy, %d delay", eff_clk / 1000, apbclk_kHz / (delay_apb_n + 1), delay_apb_n, dummy_required, miso_delay);
}

#ifdef CONFIG_IDF_TARGET_ESP32
//TODO: IDF-6578
int spi_hal_get_freq_limit(bool gpio_is_used, int input_delay_ns)
{
    const int apbclk_kHz = APB_CLK_FREQ / 1000;
    const int gpio_delay_ns = gpio_is_used ? GPIO_MATRIX_DELAY_NS : 0;

    //how many apb clocks the delay is, the 1 is to compensate in case ``input_delay_ns`` is rounded off.
    int delay_apb_n = (1 + input_delay_ns + gpio_delay_ns) * apbclk_kHz / 1000 / 1000;
    if (delay_apb_n < 0) {
        delay_apb_n = 0;
    }

    return APB_CLK_FREQ / (delay_apb_n + 1);
}
#endif
