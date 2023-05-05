/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/adc_hal_common.h"
#include "hal/adc_ll.h"
#include "hal/assert.h"

/*---------------------------------------------------------------
                    Controller Setting
---------------------------------------------------------------*/
static adc_ll_controller_t get_controller(adc_unit_t unit, adc_hal_work_mode_t work_mode)
{
    if (unit == ADC_UNIT_1) {
        switch (work_mode) {
#if SOC_ULP_HAS_ADC
            case ADC_HAL_ULP_FSM_MODE:
                return ADC_LL_CTRL_ULP;
#endif
            case ADC_HAL_SINGLE_READ_MODE:
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
                return ADC_LL_CTRL_DIG;
#elif SOC_ADC_RTC_CTRL_SUPPORTED
                return ADC_LL_CTRL_RTC;
#endif
            case ADC_HAL_CONTINUOUS_READ_MODE:
                return ADC_LL_CTRL_DIG;
            default:
                abort();
        }
    } else {
        switch (work_mode) {
#if SOC_ULP_HAS_ADC
            case ADC_HAL_ULP_FSM_MODE:
                return ADC_LL_CTRL_ULP;
#endif
#if !SOC_ADC_ARBITER_SUPPORTED                  //No ADC2 arbiter on ESP32
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
            default:
                return ADC_LL_CTRL_DIG;
#else
            case ADC_HAL_SINGLE_READ_MODE:
                return ADC_LL_CTRL_RTC;
            case ADC_HAL_CONTINUOUS_READ_MODE:
                return ADC_LL_CTRL_DIG;
            case ADC_HAL_PWDET_MODE:
                return ADC_LL_CTRL_PWDET;
            default:
                abort();
#endif  //#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
#else
            default:
                return ADC_LL_CTRL_ARB;
#endif
        }
    }
}

void adc_hal_set_controller(adc_unit_t unit, adc_hal_work_mode_t work_mode)
{
    adc_ll_controller_t ctrlr = get_controller(unit, work_mode);
    adc_ll_set_controller(unit, ctrlr);
}


/*---------------------------------------------------------------
                    Arbiter
---------------------------------------------------------------*/
#if SOC_ADC_ARBITER_SUPPORTED
void adc_hal_arbiter_config(adc_arbiter_t *config)
{
    adc_ll_set_arbiter_work_mode(config->mode);
    adc_ll_set_arbiter_priority(config->rtc_pri, config->dig_pri, config->pwdet_pri);
}
#endif  // #if SOC_ADC_ARBITER_SUPPORTED


/*---------------------------------------------------------------
                    ADC calibration setting
---------------------------------------------------------------*/
#if SOC_ADC_CALIBRATION_V1_SUPPORTED
//For chips without RTC controller, Digital controller is used to trigger an ADC single read.
#include "esp_rom_sys.h"

void adc_hal_calibration_init(adc_unit_t adc_n)
{
    adc_ll_calibration_init(adc_n);
}

static uint32_t s_previous_init_code[SOC_ADC_PERIPH_NUM] = {
    [0 ... (SOC_ADC_PERIPH_NUM - 1)] = -1,
};

void adc_hal_set_calibration_param(adc_unit_t adc_n, uint32_t param)
{
    if (param != s_previous_init_code[adc_n]) {
        adc_ll_set_calibration_param(adc_n, param);
        s_previous_init_code[adc_n] = param;
    }
}

#if SOC_ADC_SELF_HW_CALI_SUPPORTED
static void cal_setup(adc_unit_t adc_n, adc_atten_t atten)
{
    adc_hal_set_controller(adc_n, ADC_HAL_SINGLE_READ_MODE);
    adc_oneshot_ll_disable_all_unit();
    // Enableinternal connect GND (for calibration).
    adc_oneshot_ll_disable_channel(adc_n);
    /**
     * Note:
     * When controlled by RTC controller, when all channels are disabled, HW auto selects channel0 atten param.
     * When controlled by DIG controller, unit and channel are not related to attenuation
     */
    adc_oneshot_ll_set_atten(adc_n, 0, atten);
    adc_oneshot_ll_enable(adc_n);
}

static uint32_t read_cal_channel(adc_unit_t adc_n)
{
    uint32_t event = (adc_n == ADC_UNIT_1) ? ADC_LL_EVENT_ADC1_ONESHOT_DONE : ADC_LL_EVENT_ADC2_ONESHOT_DONE;
    adc_oneshot_ll_clear_event(event);

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    adc_oneshot_ll_start(false);
    esp_rom_delay_us(5);
    adc_oneshot_ll_start(true);
#else
    adc_oneshot_ll_start(adc_n);
#endif

    while(!adc_oneshot_ll_get_event(event));

    uint32_t read_val = -1;
    read_val = adc_oneshot_ll_get_raw_result(adc_n);
    if (adc_oneshot_ll_raw_check_valid(adc_n, read_val) == false) {
        return -1;
    }
    return read_val;
}

#define ADC_HAL_CAL_TIMES        (10)
#define ADC_HAL_CAL_OFFSET_RANGE (4096)

uint32_t adc_hal_self_calibration(adc_unit_t adc_n, adc_atten_t atten, bool internal_gnd)
{
#if SOC_ADC_ARBITER_SUPPORTED
    if (adc_n == ADC_UNIT_2) {
        adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
        adc_hal_arbiter_config(&config);
    }
#endif // #if SOC_ADC_ARBITER_SUPPORTED

    cal_setup(adc_n, atten);

    adc_ll_calibration_prepare(adc_n, internal_gnd);

    uint32_t code_list[ADC_HAL_CAL_TIMES] = {0};
    uint32_t code_sum = 0;
    uint32_t code_h = 0;
    uint32_t code_l = 0;
    uint32_t chk_code = 0;

    for (uint8_t rpt = 0 ; rpt < ADC_HAL_CAL_TIMES ; rpt ++) {
        code_h = ADC_HAL_CAL_OFFSET_RANGE;
        code_l = 0;
        chk_code = (code_h + code_l) / 2;
        adc_ll_set_calibration_param(adc_n, chk_code);
        uint32_t self_cal = read_cal_channel(adc_n);
        while (code_h - code_l > 1) {
            if (self_cal == 0) {
                code_h = chk_code;
            } else {
                code_l = chk_code;
            }
            chk_code = (code_h + code_l) / 2;
            adc_ll_set_calibration_param(adc_n, chk_code);
            self_cal = read_cal_channel(adc_n);
            if ((code_h - code_l == 1)) {
                chk_code += 1;
                adc_ll_set_calibration_param(adc_n, chk_code);
                self_cal = read_cal_channel(adc_n);
            }
        }
        code_list[rpt] = chk_code;
        code_sum += chk_code;
    }

    code_l = code_list[0];
    code_h = code_list[0];
    for (uint8_t i = 0 ; i < ADC_HAL_CAL_TIMES ; i++) {
        code_l = MIN(code_l, code_list[i]);
        code_h = MAX(code_h, code_list[i]);
    }

    chk_code = code_h + code_l;
    uint32_t ret = ((code_sum - chk_code) % (ADC_HAL_CAL_TIMES - 2) < 4)
           ? (code_sum - chk_code) / (ADC_HAL_CAL_TIMES - 2)
           : (code_sum - chk_code) / (ADC_HAL_CAL_TIMES - 2) + 1;

    adc_ll_calibration_finish(adc_n);
    return ret;
}
#endif  //#if SOC_ADC_SELF_HW_CALI_SUPPORTED
#endif //SOC_ADC_CALIBRATION_V1_SUPPORTED
