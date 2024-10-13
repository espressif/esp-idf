/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/adc_oneshot_hal.h"
#include "hal/adc_hal_common.h"
#include "hal/adc_ll.h"
#include "hal/assert.h"
#include "hal/log.h"

#if SOC_DAC_SUPPORTED
#include "hal/dac_ll.h"
#endif

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
/**
 * For chips without RTC controller, Digital controller is used to trigger an ADC single read.
 */
#include "esp_rom_sys.h"
#endif


#if CONFIG_ADC_DISABLE_DAC_OUTPUT
// To disable DAC, workarounds, see this function body to know more
static void s_disable_dac(adc_oneshot_hal_ctx_t *hal, adc_channel_t channel);
#endif


void adc_oneshot_hal_init(adc_oneshot_hal_ctx_t *hal, const adc_oneshot_hal_cfg_t *config)
{
    hal->unit = config->unit;
    hal->work_mode = config->work_mode;
    hal->clk_src = config->clk_src;
    hal->clk_src_freq_hz = config->clk_src_freq_hz;
}

void adc_oneshot_hal_channel_config(adc_oneshot_hal_ctx_t *hal, const adc_oneshot_hal_chan_cfg_t *config, adc_channel_t chan)
{
    hal->chan_configs[chan].atten = config->atten;
    hal->chan_configs[chan].bitwidth = config->bitwidth;
}

void adc_oneshot_hal_setup(adc_oneshot_hal_ctx_t *hal, adc_channel_t chan)
{
    adc_unit_t unit = hal->unit;

#ifdef CONFIG_IDF_TARGET_ESP32
    adc_ll_hall_disable(); //Disable other peripherals.
    adc_ll_amp_disable();  //Currently the LNA is not open, close it by default.
#endif

#if CONFIG_ADC_DISABLE_DAC_OUTPUT
    s_disable_dac(hal, chan);
#endif

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    adc_ll_digi_clk_sel(hal->clk_src);
    adc_ll_digi_controller_clk_div(ADC_LL_CLKM_DIV_NUM_DEFAULT, ADC_LL_CLKM_DIV_A_DEFAULT, ADC_LL_CLKM_DIV_B_DEFAULT);
    adc_ll_digi_set_clk_div(ADC_LL_DIGI_SAR_CLK_DIV_DEFAULT);
#else
#if SOC_LP_ADC_SUPPORTED
    if (hal->work_mode == ADC_HAL_LP_MODE) {
        adc_ll_set_sar_clk_div(unit, LP_ADC_LL_SAR_CLK_DIV_DEFAULT(unit));
    } else {
        adc_ll_set_sar_clk_div(unit, ADC_LL_SAR_CLK_DIV_DEFAULT(unit));
    }
#else
    adc_ll_set_sar_clk_div(unit, ADC_LL_SAR_CLK_DIV_DEFAULT(unit));
#endif //SOC_LP_ADC_SUPPORTED
    if (unit == ADC_UNIT_2) {
        adc_ll_pwdet_set_cct(ADC_LL_PWDET_CCT_DEFAULT);
    }
#endif

    adc_oneshot_ll_output_invert(unit, ADC_LL_DATA_INVERT_DEFAULT(unit));
    adc_oneshot_ll_set_atten(unit, chan, hal->chan_configs[chan].atten);
    adc_oneshot_ll_set_output_bits(unit, hal->chan_configs[chan].bitwidth);
    adc_oneshot_ll_set_channel(unit, chan);
    adc_hal_set_controller(unit, hal->work_mode);

#if SOC_ADC_ARBITER_SUPPORTED
    adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
    adc_hal_arbiter_config(&config);
#endif //#if SOC_ADC_ARBITER_SUPPORTED
}

static void adc_hal_onetime_start(adc_unit_t unit, uint32_t clk_src_freq_hz, uint32_t *read_delay_us)
{
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    (void)unit;
    /**
     * There is a hardware limitation. If the APB clock frequency is high, the step of this reg signal: ``onetime_start`` may not be captured by the
     * ADC digital controller (when its clock frequency is too slow). A rough estimate for this step should be at least 3 ADC digital controller
     * clock cycle.
     */
    uint32_t adc_ctrl_clk = clk_src_freq_hz / (ADC_LL_CLKM_DIV_NUM_DEFAULT + ADC_LL_CLKM_DIV_A_DEFAULT / ADC_LL_CLKM_DIV_B_DEFAULT + 1);
    //Convert frequency to time (us). Since decimals are removed by this division operation. Add 1 here in case of the fact that delay is not enough.
    uint32_t sample_delay_us = ((1000 * 1000) / adc_ctrl_clk + 1) * 3;
    HAL_EARLY_LOGD("adc_hal", "clk_src_freq_hz: %"PRIu32", adc_ctrl_clk: %"PRIu32", sample_delay_us: %"PRIu32"", clk_src_freq_hz, adc_ctrl_clk, sample_delay_us);

    //This coefficient (8) is got from test, and verified from DT. When digi_clk is not smaller than ``APB_CLK_FREQ/8``, no delay is needed.
    if (adc_ctrl_clk >= APB_CLK_FREQ/8) {
        sample_delay_us = 0;
    }

    HAL_EARLY_LOGD("adc_hal", "delay for `onetime_start` signal captured: %"PRIu32"", sample_delay_us);
    adc_oneshot_ll_start(false);
    esp_rom_delay_us(sample_delay_us);
    adc_oneshot_ll_start(true);

#if ADC_LL_DELAY_CYCLE_AFTER_DONE_SIGNAL
    /**
     * There is a hardware limitation.
     * After ADC get DONE signal, it still need a delay to synchronize ADC raw data or it may get zero.
     * A rough estimate for this step should be at least ADC_LL_DELAY_CYCLE_AFTER_DONE_SIGNAL ADC sar clock cycle.
     */
    uint32_t sar_clk = adc_ctrl_clk / ADC_LL_DIGI_SAR_CLK_DIV_DEFAULT;
    *read_delay_us = ((1000 * 1000) / sar_clk + 1) * ADC_LL_DELAY_CYCLE_AFTER_DONE_SIGNAL;
    HAL_EARLY_LOGD("adc_hal", "clk_src_freq_hz: %"PRIu32", sar_clk: %"PRIu32", read_delay_us: %"PRIu32"", clk_src_freq_hz, sar_clk, read_delay_us);
#endif //ADC_LL_DELAY_CYCLE_AFTER_DONE_SIGNAL

#else
    adc_oneshot_ll_start(unit);
#endif // SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
}

bool adc_oneshot_hal_convert(adc_oneshot_hal_ctx_t *hal, int *out_raw)
{
    bool valid = true;
    uint32_t event = 0;
    uint32_t read_delay_us = 0;
    if (hal->unit == ADC_UNIT_1) {
        event = ADC_LL_EVENT_ADC1_ONESHOT_DONE;
    } else {
        event = ADC_LL_EVENT_ADC2_ONESHOT_DONE;
    }

    adc_oneshot_ll_clear_event(event);
    adc_oneshot_ll_disable_all_unit();
    adc_oneshot_ll_enable(hal->unit);

    adc_hal_onetime_start(hal->unit, hal->clk_src_freq_hz, &read_delay_us);
    while (!adc_oneshot_ll_get_event(event)) {
        ;
    }
    esp_rom_delay_us(read_delay_us);
    *out_raw = adc_oneshot_ll_get_raw_result(hal->unit);
#if SOC_ADC_ARBITER_SUPPORTED
    if (hal->unit == ADC_UNIT_2) {
        valid = adc_oneshot_ll_raw_check_valid(ADC_UNIT_2, *out_raw);
        if (!valid) {
            *out_raw = -1;
        }
    }
#endif

    adc_oneshot_ll_disable_all_unit();
    return valid;
}

/*---------------------------------------------------------------
                    Workarounds
---------------------------------------------------------------*/
#if CONFIG_ADC_DISABLE_DAC_OUTPUT
static void s_disable_dac(adc_oneshot_hal_ctx_t *hal, adc_channel_t channel)
{
    /**
     * Workaround: Disable the synchronization operation function of ADC1 and DAC.
     * If enabled(default), ADC RTC controller sampling will cause the DAC channel output voltage.
     */
    if (hal->unit == ADC_UNIT_1) {
        dac_ll_rtc_sync_by_adc(false);
    }

#if CONFIG_IDF_TARGET_ESP32
    if (hal->unit == ADC_UNIT_2) {
        if (channel == ADC_CHANNEL_8) {
            dac_ll_power_down(DAC_CHAN_0);  // the same as DAC channel 0
        }
        if (channel == ADC_CHANNEL_9) {
            dac_ll_power_down(DAC_CHAN_1);
        }
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    if (hal->unit == ADC_UNIT_2) {
        if (channel == ADC_CHANNEL_6) {
            dac_ll_power_down(DAC_CHAN_0);  // the same as DAC channel 0
        }
        if (channel == ADC_CHANNEL_7) {
            dac_ll_power_down(DAC_CHAN_1);
        }
    }
#else
    //Nothing needed (DAC is only supported on ESP32 and ESP32S2), add this if future chips needs
#endif
}
#endif
