/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for SPI (common part)

#include "hal/spi_hal.h"
#include "hal/log.h"
#include "hal/assert.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

/* The tag may be unused if log level is set to NONE  */
static const __attribute__((unused)) char SPI_HAL_TAG[] = "spi_hal";

#define SPI_HAL_CHECK(a, str, ret_val, ...) \
    if (!(a)) { \
        HAL_LOGE(SPI_HAL_TAG,"%s(%d): "str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

void spi_hal_init(spi_hal_context_t *hal, uint32_t host_id)
{
    memset(hal, 0, sizeof(spi_hal_context_t));
    spi_dev_t *hw = SPI_LL_GET_HW(host_id);
    hal->hw = hw;

#if SPI_LL_MOSI_FREE_LEVEL
    // Change default data line level to low which same as esp32
    spi_ll_set_mosi_free_level(hw, 0);
#endif
    spi_ll_master_init(hw);

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

#if SOC_SPI_SCT_SUPPORTED
void spi_hal_sct_init(spi_hal_context_t *hal)
{
    spi_ll_conf_state_enable(hal->hw, true);
    spi_ll_set_magic_number(hal->hw, SPI_LL_SCT_MAGIC_NUMBER);
    spi_ll_disable_int(hal->hw);    //trans_done intr enabled in `add device` phase, sct mode shoud use sct_trans_done only
    spi_ll_enable_intr(hal->hw, SPI_LL_INTR_SEG_DONE);
    spi_ll_set_intr(hal->hw, SPI_LL_INTR_SEG_DONE);
}

void spi_hal_sct_deinit(spi_hal_context_t *hal)
{
    spi_ll_conf_state_enable(hal->hw, false);
    spi_ll_disable_intr(hal->hw, SPI_LL_INTR_SEG_DONE);
    spi_ll_clear_intr(hal->hw, SPI_LL_INTR_SEG_DONE);
    spi_ll_clear_int_stat(hal->hw);
    spi_ll_enable_int(hal->hw); //recover trans_done intr
}
#endif  //#if SOC_SPI_SCT_SUPPORTED

esp_err_t spi_hal_cal_clock_conf(const spi_hal_timing_param_t *timing_param, spi_hal_timing_conf_t *timing_conf)
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

    temp_conf.real_freq = eff_clk_n;
    if (timing_conf) {
        *timing_conf = temp_conf;
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
