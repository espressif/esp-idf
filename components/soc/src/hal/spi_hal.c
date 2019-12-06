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

// The HAL layer for SPI (common part)

#include "hal/spi_hal.h"


static const char SPI_HAL_TAG[] = "spi_hal";
#define SPI_HAL_CHECK(a, str, ret_val, ...) \
    if (!(a)) { \
        HAL_LOGE(SPI_HAL_TAG,"%s(%d): "str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

void spi_hal_init(spi_hal_context_t *hal, int host_id)
{
    memset(hal, 0, sizeof(spi_hal_context_t));
    spi_dev_t *hw = spi_periph_signal[host_id].hw;
    hal->hw = hw;
    spi_ll_master_init(hw);

    //Force a transaction done interrupt. This interrupt won't fire yet because
    //we initialized the SPI interrupt as disabled. This way, we can just
    //enable the SPI interrupt and the interrupt handler will kick in, handling
    //any transactions that are queued.
    spi_ll_enable_int(hw);
    spi_ll_set_int_stat(hw);
    spi_ll_set_mosi_delay(hw, 0, 0);
}

void spi_hal_deinit(spi_hal_context_t *hal)
{
    spi_dev_t *hw = hal->hw;
    if (hw) {
        spi_ll_disable_int(hw);
        spi_ll_clear_int_stat(hw);
    }
}

esp_err_t spi_hal_get_clock_conf(const spi_hal_context_t *hal, int speed_hz, int duty_cycle, bool use_gpio, int input_delay_ns, int *out_freq, spi_hal_timing_conf_t *timing_conf)
{
    spi_hal_timing_conf_t temp_conf;

    int eff_clk_n = spi_ll_master_cal_clock(APB_CLK_FREQ, speed_hz, duty_cycle, &temp_conf.clock_reg);

    //When the speed is too fast, we may need to use dummy cycles to compensate the reading.
    //But these don't work for full-duplex connections.
    spi_hal_cal_timing(eff_clk_n, use_gpio, input_delay_ns, &temp_conf.timing_dummy, &temp_conf.timing_miso_delay);

#ifdef CONFIG_IDF_TARGET_ESP32
    const int freq_limit = spi_hal_get_freq_limit(use_gpio, input_delay_ns);

    SPI_HAL_CHECK(hal->half_duplex || temp_conf.timing_dummy == 0 || hal->no_compensate,
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

void spi_hal_cal_timing(int eff_clk, bool gpio_is_used, int input_delay_ns, int *dummy_n, int *miso_delay_n)
{
    const int apbclk_kHz = APB_CLK_FREQ / 1000;
    //calculate how many apb clocks a period has
    const int apbclk_n = APB_CLK_FREQ / eff_clk;
    const int gpio_delay_ns = gpio_is_used ? GPIO_MATRIX_DELAY_NS : 0;

    //calculate how many apb clocks the delay is, the 1 is to compensate in case ``input_delay_ns`` is rounded off.
    int apb_period_n = (1 + input_delay_ns + gpio_delay_ns) * apbclk_kHz / 1000 / 1000;
    if (apb_period_n < 0) {
        apb_period_n = 0;
    }

    int dummy_required = apb_period_n / apbclk_n;

    int miso_delay = 0;
    if (dummy_required > 0) {
        //due to the clock delay between master and slave, there's a range in which data is random
        //give MISO a delay if needed to make sure we sample at the time MISO is stable
        miso_delay = (dummy_required + 1) * apbclk_n - apb_period_n - 1;
    } else {
        //if the dummy is not required, maybe we should also delay half a SPI clock if the data comes too early
        if (apb_period_n * 4 <= apbclk_n) {
            miso_delay = -1;
        }
    }
    *dummy_n = dummy_required;
    *miso_delay_n = miso_delay;
    HAL_LOGD(SPI_HAL_TAG, "eff: %d, limit: %dk(/%d), %d dummy, %d delay", eff_clk / 1000, apbclk_kHz / (apb_period_n + 1), apb_period_n, dummy_required, miso_delay);
}

int spi_hal_get_freq_limit(bool gpio_is_used, int input_delay_ns)
{
    const int apbclk_kHz = APB_CLK_FREQ / 1000;
    const int gpio_delay_ns = gpio_is_used ? GPIO_MATRIX_DELAY_NS : 0;

    //calculate how many apb clocks the delay is, the 1 is to compensate in case ``input_delay_ns`` is rounded off.
    int apb_period_n = (1 + input_delay_ns + gpio_delay_ns) * apbclk_kHz / 1000 / 1000;
    if (apb_period_n < 0) {
        apb_period_n = 0;
    }

    return APB_CLK_FREQ / (apb_period_n + 1);
}