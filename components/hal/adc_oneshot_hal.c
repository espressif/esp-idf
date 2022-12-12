/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/adc_oneshot_hal.h"
#include "hal/adc_hal_common.h"
#include "hal/adc_hal_conf.h"
#include "hal/adc_ll.h"
#include "hal/assert.h"

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
    adc_ll_digi_clk_sel(0);
#else
    adc_ll_set_sar_clk_div(unit, ADC_HAL_SAR_CLK_DIV_DEFAULT(unit));
    if (unit == ADC_UNIT_2) {
        adc_ll_pwdet_set_cct(ADC_HAL_PWDET_CCT_DEFAULT);
    }
#endif

    adc_oneshot_ll_output_invert(unit, ADC_HAL_DATA_INVERT_DEFAULT(unit));
    adc_oneshot_ll_set_atten(unit, chan, hal->chan_configs[chan].atten);
    adc_oneshot_ll_set_output_bits(unit, hal->chan_configs[chan].bitwidth);
    adc_oneshot_ll_set_channel(unit, chan);
    adc_hal_set_controller(unit, hal->work_mode);

#if SOC_ADC_ARBITER_SUPPORTED
    adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
    adc_hal_arbiter_config(&config);
#endif //#if SOC_ADC_ARBITER_SUPPORTED
}

static void adc_hal_onetime_start(adc_unit_t unit)
{
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    (void)unit;
    /**
     * There is a hardware limitation. If the APB clock frequency is high, the step of this reg signal: ``onetime_start`` may not be captured by the
     * ADC digital controller (when its clock frequency is too slow). A rough estimate for this step should be at least 3 ADC digital controller
     * clock cycle.
     *
     * This limitation will be removed in hardware future versions.
     *
     */
    uint32_t digi_clk = APB_CLK_FREQ / (ADC_LL_CLKM_DIV_NUM_DEFAULT + ADC_LL_CLKM_DIV_A_DEFAULT / ADC_LL_CLKM_DIV_B_DEFAULT + 1);
    //Convert frequency to time (us). Since decimals are removed by this division operation. Add 1 here in case of the fact that delay is not enough.
    uint32_t delay = (1000 * 1000) / digi_clk + 1;
    //3 ADC digital controller clock cycle
    delay = delay * 3;
    //This coefficient (8) is got from test. When digi_clk is not smaller than ``APB_CLK_FREQ/8``, no delay is needed.
    if (digi_clk >= APB_CLK_FREQ/8) {
        delay = 0;
    }

    adc_oneshot_ll_start(false);
    esp_rom_delay_us(delay);
    adc_oneshot_ll_start(true);

    //No need to delay here. Becuase if the start signal is not seen, there won't be a done intr.
#else
    adc_oneshot_ll_start(unit);
#endif
}

bool adc_oneshot_hal_convert(adc_oneshot_hal_ctx_t *hal, int *out_raw)
{
    bool valid = true;
    uint32_t event = 0;
    if (hal->unit == ADC_UNIT_1) {
        event = ADC_LL_EVENT_ADC1_ONESHOT_DONE;
    } else {
        event = ADC_LL_EVENT_ADC2_ONESHOT_DONE;
    }

    adc_oneshot_ll_clear_event(event);
    adc_oneshot_ll_disable_all_unit();
    adc_oneshot_ll_enable(hal->unit);

    adc_hal_onetime_start(hal->unit);
    while (!adc_oneshot_ll_get_event(event)) {
        ;
    }
    *out_raw = adc_oneshot_ll_get_raw_result(hal->unit);
#if (SOC_ADC_PERIPH_NUM == 2)
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
