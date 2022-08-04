/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "soc/rtc.h"
#include "driver/rtc_io.h"
#include "sys/lock.h"
#include "driver/gpio.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "adc1_private.h"
#include "hal/adc_types.h"
#include "hal/adc_hal.h"
#include "hal/adc_hal_common.h"
#include "hal/adc_hal_conf.h"
#include "esp_private/periph_ctrl.h"
#include "driver/adc_types_legacy.h"

#if SOC_DAC_SUPPORTED
#include "driver/dac.h"
#include "hal/dac_hal.h"
#endif

#if CONFIG_IDF_TARGET_ESP32S3
#include "esp_efuse_rtc_calib.h"
#endif

#define ADC_CHECK_RET(fun_ret) ({                  \
    if (fun_ret != ESP_OK) {                                \
        ESP_LOGE(ADC_TAG,"%s:%d\n",__FUNCTION__,__LINE__);  \
        return ESP_FAIL;                                    \
    }                                                       \
})

static const char *ADC_TAG = "ADC";

#define ADC_CHECK(a, str, ret_val) ({                                               \
    if (!(a)) {                                                                     \
        ESP_LOGE(ADC_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str);                \
        return (ret_val);                                                           \
    }                                                                               \
})

#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])

#define ADC_CHANNEL_CHECK(periph, channel) ADC_CHECK(channel < SOC_ADC_CHANNEL_NUM(periph), "ADC"#periph" channel error", ESP_ERR_INVALID_ARG)

//////////////////////// Locks ///////////////////////////////////////////
extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.

#define RTC_ENTER_CRITICAL()    portENTER_CRITICAL(&rtc_spinlock)
#define RTC_EXIT_CRITICAL()     portEXIT_CRITICAL(&rtc_spinlock)
#define DIGI_ENTER_CRITICAL()
#define DIGI_EXIT_CRITICAL()

#define ADC_POWER_ENTER()       RTC_ENTER_CRITICAL()
#define ADC_POWER_EXIT()        RTC_EXIT_CRITICAL()

#define DIGI_CONTROLLER_ENTER() DIGI_ENTER_CRITICAL()
#define DIGI_CONTROLLER_EXIT()  DIGI_EXIT_CRITICAL()

#define SARADC1_ENTER()         RTC_ENTER_CRITICAL()
#define SARADC1_EXIT()          RTC_EXIT_CRITICAL()

#define SARADC2_ENTER()         RTC_ENTER_CRITICAL()
#define SARADC2_EXIT()          RTC_EXIT_CRITICAL()

//n stands for ADC unit: 1 for ADC1 and 2 for ADC2. Currently both unit touches the same registers
#define VREF_ENTER(n)           RTC_ENTER_CRITICAL()
#define VREF_EXIT(n)            RTC_EXIT_CRITICAL()

#define FSM_ENTER()             RTC_ENTER_CRITICAL()
#define FSM_EXIT()              RTC_EXIT_CRITICAL()

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
//prevent ADC1 being used by I2S dma and other tasks at the same time.
static _lock_t adc1_dma_lock;
#define SARADC1_ACQUIRE() _lock_acquire( &adc1_dma_lock )
#define SARADC1_RELEASE() _lock_release( &adc1_dma_lock )
#endif


/*
In ADC2, there're two locks used for different cases:
1. lock shared with app and Wi-Fi:
   ESP32:
        When Wi-Fi using the ADC2, we assume it will never stop, so app checks the lock and returns immediately if failed.
   ESP32S2:
        The controller's control over the ADC is determined by the arbiter. There is no need to control by lock.

2. lock shared between tasks:
   when several tasks sharing the ADC2, we want to guarantee
   all the requests will be handled.
   Since conversions are short (about 31us), app returns the lock very soon,
   we use a spinlock to stand there waiting to do conversions one by one.

adc2_spinlock should be acquired first, then call `adc_lock_release(ADC_UNIT_2)` or rtc_spinlock.
*/

#if CONFIG_IDF_TARGET_ESP32S2
#ifdef CONFIG_PM_ENABLE
static esp_pm_lock_handle_t s_adc2_arbiter_lock;
#endif  //CONFIG_PM_ENABLE
#endif  // !CONFIG_IDF_TARGET_ESP32

static esp_err_t adc_hal_convert(adc_unit_t adc_n, int channel, int *out_raw);

/*---------------------------------------------------------------
                    ADC Common
---------------------------------------------------------------*/
esp_err_t adc1_pad_get_io_num(adc1_channel_t channel, gpio_num_t *gpio_num)
{
    ADC_CHANNEL_CHECK(ADC_UNIT_1, channel);

    int io = ADC_GET_IO_NUM(ADC_UNIT_1, channel);
    if (io < 0) {
        return ESP_ERR_INVALID_ARG;
    } else {
        *gpio_num = (gpio_num_t)io;
    }

    return ESP_OK;
}

#if (SOC_ADC_PERIPH_NUM >= 2)
esp_err_t adc2_pad_get_io_num(adc2_channel_t channel, gpio_num_t *gpio_num)
{
    ADC_CHANNEL_CHECK(ADC_UNIT_2, channel);

    int io = ADC_GET_IO_NUM(ADC_UNIT_2, channel);
    if (io < 0) {
        return ESP_ERR_INVALID_ARG;
    } else {
        *gpio_num = (gpio_num_t)io;
    }

    return ESP_OK;
}
#endif

//------------------------------------------------------------RTC Single Read----------------------------------------------//
#if SOC_ADC_RTC_CTRL_SUPPORTED
esp_err_t adc_set_clk_div(uint8_t clk_div)
{
    DIGI_CONTROLLER_ENTER();
    adc_ll_digi_set_clk_div(clk_div);
    DIGI_CONTROLLER_EXIT();
    return ESP_OK;
}

static void adc_rtc_chan_init(adc_unit_t adc_unit)
{
    if (adc_unit == ADC_UNIT_1) {
        /* Workaround: Disable the synchronization operation function of ADC1 and DAC.
           If enabled(default), ADC RTC controller sampling will cause the DAC channel output voltage. */
#if SOC_DAC_SUPPORTED
        dac_hal_rtc_sync_by_adc(false);
#endif
        adc_oneshot_ll_output_invert(ADC_UNIT_1, ADC_HAL_DATA_INVERT_DEFAULT(ADC_UNIT_1));
        adc_ll_set_sar_clk_div(ADC_UNIT_1, ADC_HAL_SAR_CLK_DIV_DEFAULT(ADC_UNIT_1));
#ifdef CONFIG_IDF_TARGET_ESP32
        adc_ll_hall_disable(); //Disable other peripherals.
        adc_ll_amp_disable();  //Currently the LNA is not open, close it by default.
#endif
    }
    if (adc_unit == ADC_UNIT_2) {
        adc_hal_pwdet_set_cct(ADC_HAL_PWDET_CCT_DEFAULT);
        adc_oneshot_ll_output_invert(ADC_UNIT_2, ADC_HAL_DATA_INVERT_DEFAULT(ADC_UNIT_2));
        adc_ll_set_sar_clk_div(ADC_UNIT_2, ADC_HAL_SAR_CLK_DIV_DEFAULT(ADC_UNIT_2));
    }
}

esp_err_t adc_common_gpio_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    //If called with `ADC_UNIT_BOTH (ADC_UNIT_1 | ADC_UNIT_2)`, both if blocks will be run
    if (adc_unit == ADC_UNIT_1) {
        ADC_CHANNEL_CHECK(ADC_UNIT_1, channel);
        gpio_num = ADC_GET_IO_NUM(ADC_UNIT_1, channel);
        ADC_CHECK_RET(rtc_gpio_init(gpio_num));
        ADC_CHECK_RET(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
        ADC_CHECK_RET(rtc_gpio_pulldown_dis(gpio_num));
        ADC_CHECK_RET(rtc_gpio_pullup_dis(gpio_num));
    }
    if (adc_unit == ADC_UNIT_2) {
        ADC_CHANNEL_CHECK(ADC_UNIT_2, channel);
        gpio_num = ADC_GET_IO_NUM(ADC_UNIT_2, channel);
        ADC_CHECK_RET(rtc_gpio_init(gpio_num));
        ADC_CHECK_RET(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
        ADC_CHECK_RET(rtc_gpio_pulldown_dis(gpio_num));
        ADC_CHECK_RET(rtc_gpio_pullup_dis(gpio_num));
    }

    return ESP_OK;
}

esp_err_t adc_set_data_inv(adc_unit_t adc_unit, bool inv_en)
{
    if (adc_unit == ADC_UNIT_1) {
        SARADC1_ENTER();
        adc_oneshot_ll_output_invert(ADC_UNIT_1, inv_en);
        SARADC1_EXIT();
    }
    if (adc_unit == ADC_UNIT_2) {
        SARADC2_ENTER();
        adc_oneshot_ll_output_invert(ADC_UNIT_2, inv_en);
        SARADC2_EXIT();
    }

    return ESP_OK;
}

esp_err_t adc_set_data_width(adc_unit_t adc_unit, adc_bits_width_t width_bit)
{
    ADC_CHECK(width_bit < ADC_WIDTH_MAX, "unsupported bit width", ESP_ERR_INVALID_ARG);
    adc_bitwidth_t bitwidth = 0;
#if CONFIG_IDF_TARGET_ESP32
    if ((uint32_t)width_bit == (uint32_t)ADC_BITWIDTH_DEFAULT) {
        bitwidth = SOC_ADC_RTC_MAX_BITWIDTH;
    } else {
        switch(width_bit) {
            case ADC_WIDTH_BIT_9:
                bitwidth = ADC_BITWIDTH_9;
                break;
            case ADC_WIDTH_BIT_10:
                bitwidth = ADC_BITWIDTH_10;
                break;
            case ADC_WIDTH_BIT_11:
                bitwidth = ADC_BITWIDTH_11;
                break;
            case ADC_WIDTH_BIT_12:
                bitwidth = ADC_BITWIDTH_12;
                break;
            default:
                return ESP_ERR_INVALID_ARG;
        }
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    bitwidth = ADC_BITWIDTH_13;
#else   //esp32s3
    bitwidth = ADC_BITWIDTH_12;
#endif

    if (adc_unit == ADC_UNIT_1) {
        SARADC1_ENTER();
        adc_oneshot_ll_set_output_bits(ADC_UNIT_1, bitwidth);
        SARADC1_EXIT();
    }
    if (adc_unit == ADC_UNIT_2) {
        SARADC2_ENTER();
        adc_oneshot_ll_set_output_bits(ADC_UNIT_2, bitwidth);
        SARADC2_EXIT();
    }

    return ESP_OK;
}

/**
 * @brief Reset RTC controller FSM.
 *
 * @return
 *      - ESP_OK Success
 */
#if !CONFIG_IDF_TARGET_ESP32
esp_err_t adc_rtc_reset(void)
{
    FSM_ENTER();
    adc_ll_rtc_reset();
    FSM_EXIT();
    return ESP_OK;
}
#endif

/*-------------------------------------------------------------------------------------
 *                      ADC1
 *------------------------------------------------------------------------------------*/
esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)
{
    ADC_CHANNEL_CHECK(ADC_UNIT_1, channel);
    ADC_CHECK(atten < SOC_ADC_ATTEN_NUM, "ADC Atten Err", ESP_ERR_INVALID_ARG);

    adc_common_gpio_init(ADC_UNIT_1, channel);
    SARADC1_ENTER();
    adc_rtc_chan_init(ADC_UNIT_1);
    adc_oneshot_ll_set_atten(ADC_UNIT_1, channel, atten);
    SARADC1_EXIT();

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_hal_calibration_init(ADC_UNIT_1);
#endif

    return ESP_OK;
}

esp_err_t adc1_config_width(adc_bits_width_t width_bit)
{
    ADC_CHECK(width_bit < ADC_WIDTH_MAX, "unsupported bit width", ESP_ERR_INVALID_ARG);
    adc_bitwidth_t bitwidth = 0;
#if CONFIG_IDF_TARGET_ESP32
    if ((uint32_t)width_bit == (uint32_t)ADC_BITWIDTH_DEFAULT) {
        bitwidth = SOC_ADC_RTC_MAX_BITWIDTH;
    } else {
        switch(width_bit) {
            case ADC_WIDTH_BIT_9:
                bitwidth = ADC_BITWIDTH_9;
                break;
            case ADC_WIDTH_BIT_10:
                bitwidth = ADC_BITWIDTH_10;
                break;
            case ADC_WIDTH_BIT_11:
                bitwidth = ADC_BITWIDTH_11;
                break;
            case ADC_WIDTH_BIT_12:
                bitwidth = ADC_BITWIDTH_12;
                break;
            default:
                return ESP_ERR_INVALID_ARG;
        }
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    bitwidth = ADC_BITWIDTH_13;
#else   //esp32s3
    bitwidth = ADC_BITWIDTH_12;
#endif

    SARADC1_ENTER();
    adc_oneshot_ll_set_output_bits(ADC_UNIT_1, bitwidth);
    SARADC1_EXIT();

    return ESP_OK;
}

esp_err_t adc1_dma_mode_acquire(void)
{
    /* Use locks to avoid digtal and RTC controller conflicts.
       for adc1, block until acquire the lock. */
    SARADC1_ACQUIRE();
    ESP_LOGD( ADC_TAG, "dma mode takes adc1 lock." );

    adc_power_acquire();

    SARADC1_ENTER();
    /* switch SARADC into DIG channel */
    adc_ll_set_controller(ADC_UNIT_1, ADC_LL_CTRL_DIG);
    SARADC1_EXIT();

    return ESP_OK;
}

esp_err_t adc1_rtc_mode_acquire(void)
{
    /* Use locks to avoid digtal and RTC controller conflicts.
       for adc1, block until acquire the lock. */
    SARADC1_ACQUIRE();
    adc_power_acquire();

    SARADC1_ENTER();
    /* switch SARADC into RTC channel. */
    adc_ll_set_controller(ADC_UNIT_1, ADC_LL_CTRL_RTC);
    SARADC1_EXIT();

    return ESP_OK;
}

esp_err_t adc1_lock_release(void)
{
    ADC_CHECK((uint32_t *)adc1_dma_lock != NULL, "adc1 lock release called before acquire", ESP_ERR_INVALID_STATE );
    /* Use locks to avoid digtal and RTC controller conflicts. for adc1, block until acquire the lock. */

    adc_power_release();
    SARADC1_RELEASE();
    return ESP_OK;
}

int adc1_get_raw(adc1_channel_t channel)
{
    int adc_value;
    ADC_CHANNEL_CHECK(ADC_UNIT_1, channel);
    adc1_rtc_mode_acquire();

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_atten_t atten = adc_ll_get_atten(ADC_UNIT_1, channel);
    adc_set_hw_calibration_code(ADC_UNIT_1, atten);
#endif  //SOC_ADC_CALIBRATION_V1_SUPPORTED

    SARADC1_ENTER();
#ifdef CONFIG_IDF_TARGET_ESP32
    adc_ll_hall_disable(); //Disable other peripherals.
    adc_ll_amp_disable();  //Currently the LNA is not open, close it by default.
#endif
    adc_ll_set_controller(ADC_UNIT_1, ADC_LL_CTRL_RTC);    //Set controller
    adc_oneshot_ll_set_channel(ADC_UNIT_1, channel);
    adc_hal_convert(ADC_UNIT_1, channel, &adc_value);   //Start conversion, For ADC1, the data always valid.
#if !CONFIG_IDF_TARGET_ESP32
    adc_ll_rtc_reset();    //Reset FSM of rtc controller
#endif
    SARADC1_EXIT();

    adc1_lock_release();
    return adc_value;
}

int adc1_get_voltage(adc1_channel_t channel)    //Deprecated. Use adc1_get_raw() instead
{
    return adc1_get_raw(channel);
}

#if SOC_ULP_SUPPORTED
void adc1_ulp_enable(void)
{
    adc_power_acquire();

    SARADC1_ENTER();
    adc_ll_set_controller(ADC_UNIT_1, ADC_LL_CTRL_ULP);
    /* since most users do not need LNA and HALL with uLP, we disable them here
       open them in the uLP if needed. */
#ifdef CONFIG_IDF_TARGET_ESP32
    /* disable other peripherals. */
    adc_ll_hall_disable();
    adc_ll_amp_disable();
#endif
    SARADC1_EXIT();
}
#endif

#if (SOC_ADC_PERIPH_NUM >= 2)
/*---------------------------------------------------------------
                    ADC2
---------------------------------------------------------------*/
esp_err_t adc2_config_channel_atten(adc2_channel_t channel, adc_atten_t atten)
{
    ADC_CHANNEL_CHECK(ADC_UNIT_2, channel);
    ADC_CHECK(atten <= SOC_ADC_ATTEN_NUM, "ADC2 Atten Err", ESP_ERR_INVALID_ARG);

    adc_common_gpio_init(ADC_UNIT_2, channel);

#if CONFIG_IDF_TARGET_ESP32
    /** For ESP32S2 and S3, the right to use ADC2 is controlled by the arbiter, and there is no need to set a lock.*/
    if (adc_lock_try_acquire(ADC_UNIT_2) != ESP_OK) {
        //try the lock, return if failed (wifi using).
        return ESP_ERR_TIMEOUT;
    }
#endif

    //avoid collision with other tasks
    SARADC2_ENTER();
    adc_rtc_chan_init(ADC_UNIT_2);
    adc_oneshot_ll_set_atten(ADC_UNIT_2, channel, atten);
    SARADC2_EXIT();

#if CONFIG_IDF_TARGET_ESP32
    adc_lock_release(ADC_UNIT_2);
#endif

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_hal_calibration_init(ADC_UNIT_2);
#endif

    return ESP_OK;
}

static inline void adc2_init(void)
{
#if CONFIG_IDF_TARGET_ESP32S2
#ifdef CONFIG_PM_ENABLE
    /* Lock APB clock. */
    if (s_adc2_arbiter_lock == NULL) {
        esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "adc2", &s_adc2_arbiter_lock);
    }
#endif  //CONFIG_PM_ENABLE
#endif  //CONFIG_IDF_TARGET_ESP32S2
}

static inline void adc2_dac_disable( adc2_channel_t channel)
{
#if SOC_DAC_SUPPORTED
#ifdef CONFIG_IDF_TARGET_ESP32
    if ( channel == ADC2_CHANNEL_8 ) { // the same as DAC channel 1
        dac_output_disable(DAC_CHANNEL_1);
    } else if ( channel == ADC2_CHANNEL_9 ) {
        dac_output_disable(DAC_CHANNEL_2);
    }
#else
    if ( channel == ADC2_CHANNEL_6 ) { // the same as DAC channel 1
        dac_output_disable(DAC_CHANNEL_1);
    } else if ( channel == ADC2_CHANNEL_7 ) {
        dac_output_disable(DAC_CHANNEL_2);
    }
#endif
#endif // SOC_DAC_SUPPORTED
}

/**
 * @note For ESP32S2:
 *       The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 *       Or, the RTC controller will fail when get raw data.
 *       This issue does not occur on digital controllers (DMA mode), and the hardware guarantees that there will be no errors.
 */
esp_err_t adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width_bit, int *raw_out)
{
    esp_err_t ret = ESP_OK;
    int adc_value = 0;
    adc_bitwidth_t bitwidth = 0;

    ADC_CHECK(raw_out != NULL, "ADC out value err", ESP_ERR_INVALID_ARG);
    ADC_CHECK(channel < ADC2_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);
    ADC_CHECK(width_bit < ADC_WIDTH_MAX, "unsupported bit width", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    if ((uint32_t)width_bit == (uint32_t)ADC_BITWIDTH_DEFAULT) {
        bitwidth = SOC_ADC_RTC_MAX_BITWIDTH;
    } else {
        switch(width_bit) {
            case ADC_WIDTH_BIT_9:
                bitwidth = ADC_BITWIDTH_9;
                break;
            case ADC_WIDTH_BIT_10:
                bitwidth = ADC_BITWIDTH_10;
                break;
            case ADC_WIDTH_BIT_11:
                bitwidth = ADC_BITWIDTH_11;
                break;
            case ADC_WIDTH_BIT_12:
                bitwidth = ADC_BITWIDTH_12;
                break;
            default:
                return ESP_ERR_INVALID_ARG;
        }
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    bitwidth = ADC_BITWIDTH_13;
#else   //esp32s3
    bitwidth = ADC_BITWIDTH_12;
#endif

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_atten_t atten = adc_ll_get_atten(ADC_UNIT_2, channel);
    adc_set_hw_calibration_code(ADC_UNIT_2, atten);
#endif  //SOC_ADC_CALIBRATION_V1_SUPPORTED

#if CONFIG_IDF_TARGET_ESP32
    /** For ESP32S2 and S3, the right to use ADC2 is controlled by the arbiter, and there is no need to set a lock.*/
    if (adc_lock_try_acquire(ADC_UNIT_2) != ESP_OK) {
        //try the lock, return if failed (wifi using).
        return ESP_ERR_TIMEOUT;
    }
#endif
    adc_power_acquire();         //in critical section with whole rtc module

    //avoid collision with other tasks
    adc2_init();   // in critical section with whole rtc module. because the PWDET use the same registers, place it here.
    SARADC2_ENTER();

#if SOC_ADC_ARBITER_SUPPORTED
    adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
    adc_hal_arbiter_config(&config);
#endif

#ifdef CONFIG_ADC_DISABLE_DAC
    adc2_dac_disable(channel);      //disable other peripherals
#endif
    adc_oneshot_ll_set_output_bits(ADC_UNIT_2, bitwidth);

#if CONFIG_IDF_TARGET_ESP32
    adc_ll_set_controller(ADC_UNIT_2, ADC_LL_CTRL_RTC);// set controller
#else
    adc_ll_set_controller(ADC_UNIT_2, ADC_LL_CTRL_ARB);// set controller
#endif

#if CONFIG_IDF_TARGET_ESP32S2
#ifdef CONFIG_PM_ENABLE
    if (s_adc2_arbiter_lock) {
        esp_pm_lock_acquire(s_adc2_arbiter_lock);
    }
#endif //CONFIG_PM_ENABLE
#endif //CONFIG_IDF_TARGET_ESP32

    adc_oneshot_ll_set_channel(ADC_UNIT_2, channel);
    ret = adc_hal_convert(ADC_UNIT_2, channel, &adc_value);
    if (ret != ESP_OK) {
        adc_value = -1;
    }

#if CONFIG_IDF_TARGET_ESP32S2
#ifdef CONFIG_PM_ENABLE
    /* Release APB clock. */
    if (s_adc2_arbiter_lock) {
        esp_pm_lock_release(s_adc2_arbiter_lock);
    }
#endif //CONFIG_PM_ENABLE
#endif //CONFIG_IDF_TARGET_ESP32
    SARADC2_EXIT();

    adc_power_release();
#if CONFIG_IDF_TARGET_ESP32
    adc_lock_release(ADC_UNIT_2);
#endif

    *raw_out = adc_value;
    return ret;
}

esp_err_t adc_vref_to_gpio(adc_unit_t adc_unit, gpio_num_t gpio)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    if (adc_unit == ADC_UNIT_1) {
        return ESP_ERR_INVALID_ARG;
    }
#endif
    adc2_channel_t ch = ADC2_CHANNEL_MAX;
    /* Check if the GPIO supported. */
    for (int i = 0; i < ADC2_CHANNEL_MAX; i++) {
        if (gpio == ADC_GET_IO_NUM(ADC_UNIT_2, i)) {
            ch = i;
            break;
        }
    }
    if (ch == ADC2_CHANNEL_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    adc_power_acquire();
    if (adc_unit == ADC_UNIT_1) {
        VREF_ENTER(1);
        adc_hal_vref_output(ADC_UNIT_1, ch, true);
        VREF_EXIT(1);
    } else if (adc_unit == ADC_UNIT_2) {
        VREF_ENTER(2);
        adc_hal_vref_output(ADC_UNIT_2, ch, true);
        VREF_EXIT(2);
    }

    //Configure RTC gpio, Only ADC2's channels IO are supported to output reference voltage.
    adc_common_gpio_init(ADC_UNIT_2, ch);
    return ESP_OK;
}

#endif //SOC_ADC_RTC_CTRL_SUPPORTED
#endif  //#if (SOC_ADC_PERIPH_NUM >= 2)


#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
/*---------------------------------------------------------------
            Legacy ADC Single Read Mode
            when RTC controller isn't supported
---------------------------------------------------------------*/
#include "esp_check.h"

portMUX_TYPE adc_reg_lock = portMUX_INITIALIZER_UNLOCKED;
#define ADC_REG_LOCK_ENTER()       portENTER_CRITICAL(&adc_reg_lock)
#define ADC_REG_LOCK_EXIT()        portEXIT_CRITICAL(&adc_reg_lock)

static adc_atten_t s_atten1_single[ADC1_CHANNEL_MAX];    //Array saving attenuate of each channel of ADC1, used by single read API
#if (SOC_ADC_PERIPH_NUM >= 2)
static adc_atten_t s_atten2_single[ADC2_CHANNEL_MAX];    //Array saving attenuate of each channel of ADC2, used by single read API
#endif


static int8_t adc_digi_get_io_num(adc_unit_t adc_unit, uint8_t adc_channel)
{
    assert(adc_unit <= SOC_ADC_PERIPH_NUM);
    uint8_t adc_n = (adc_unit == ADC_UNIT_1) ? 0 : 1;
    return adc_channel_io_map[adc_n][adc_channel];
}

static esp_err_t adc_digi_gpio_init(adc_unit_t adc_unit, uint16_t channel_mask)
{
    esp_err_t ret = ESP_OK;
    uint64_t gpio_mask = 0;
    uint32_t n = 0;
    int8_t io = 0;

    while (channel_mask) {
        if (channel_mask & 0x1) {
            io = adc_digi_get_io_num(adc_unit, n);
            if (io < 0) {
                return ESP_ERR_INVALID_ARG;
            }
            gpio_mask |= BIT64(io);
        }
        channel_mask = channel_mask >> 1;
        n++;
    }

    gpio_config_t cfg = {
        .pin_bit_mask = gpio_mask,
        .mode = GPIO_MODE_DISABLE,
    };
    ret = gpio_config(&cfg);

    return ret;
}

#if CONFIG_IDF_TARGET_ESP32C3
esp_err_t adc_vref_to_gpio(adc_unit_t adc_unit, gpio_num_t gpio)
{
    esp_err_t ret;
    uint32_t channel = ADC2_CHANNEL_MAX;
    if (adc_unit == ADC_UNIT_2) {
        for (int i = 0; i < ADC2_CHANNEL_MAX; i++) {
            if (gpio == ADC_GET_IO_NUM(ADC_UNIT_2, i)) {
                channel = i;
                break;
            }
        }
        if (channel == ADC2_CHANNEL_MAX) {
            return ESP_ERR_INVALID_ARG;
        }
    }

    adc_power_acquire();
    if (adc_unit == ADC_UNIT_1) {
        RTC_ENTER_CRITICAL();
        adc_hal_vref_output(ADC_UNIT_1, channel, true);
        RTC_EXIT_CRITICAL();
    } else {    //ADC_UNIT_2
        RTC_ENTER_CRITICAL();
        adc_hal_vref_output(ADC_UNIT_2, channel, true);
        RTC_EXIT_CRITICAL();
    }

    ret = adc_digi_gpio_init(ADC_UNIT_2, BIT(channel));

    return ret;
}
#endif

esp_err_t adc1_config_width(adc_bits_width_t width_bit)
{
    //On ESP32C3, the data width is always 12-bits.
    if (width_bit != ADC_WIDTH_BIT_12) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)
{
    ESP_RETURN_ON_FALSE(channel < SOC_ADC_CHANNEL_NUM(ADC_UNIT_1), ESP_ERR_INVALID_ARG, ADC_TAG, "ADC1 channel error");
    ESP_RETURN_ON_FALSE((atten < SOC_ADC_ATTEN_NUM), ESP_ERR_INVALID_ARG, ADC_TAG, "ADC Atten Err");

    esp_err_t ret = ESP_OK;
    s_atten1_single[channel] = atten;
    ret = adc_digi_gpio_init(ADC_UNIT_1, BIT(channel));

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_hal_calibration_init(ADC_UNIT_1);
#endif

    return ret;
}

int adc1_get_raw(adc1_channel_t channel)
{
    int raw_out = 0;

    if (adc_lock_try_acquire(ADC_UNIT_1) != ESP_OK) {
        return ESP_ERR_TIMEOUT;
    }

    periph_module_enable(PERIPH_SARADC_MODULE);
    adc_power_acquire();
    adc_ll_digi_clk_sel(0);

    adc_atten_t atten = s_atten1_single[channel];
#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_set_hw_calibration_code(ADC_UNIT_1, atten);
#endif

    ADC_REG_LOCK_ENTER();
    adc_oneshot_ll_set_atten(ADC_UNIT_2, channel, atten);
    adc_hal_convert(ADC_UNIT_1, channel, &raw_out);
    ADC_REG_LOCK_EXIT();

    adc_power_release();
    periph_module_disable(PERIPH_SARADC_MODULE);
    adc_lock_release(ADC_UNIT_1);

    return raw_out;
}

#if (SOC_ADC_PERIPH_NUM >= 2)
esp_err_t adc2_config_channel_atten(adc2_channel_t channel, adc_atten_t atten)
{
    ESP_RETURN_ON_FALSE(channel < SOC_ADC_CHANNEL_NUM(ADC_UNIT_2), ESP_ERR_INVALID_ARG, ADC_TAG, "ADC2 channel error");
    ESP_RETURN_ON_FALSE((atten <= ADC_ATTEN_DB_11), ESP_ERR_INVALID_ARG, ADC_TAG, "ADC2 Atten Err");

    esp_err_t ret = ESP_OK;
    s_atten2_single[channel] = atten;
    ret = adc_digi_gpio_init(ADC_UNIT_2, BIT(channel));

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_hal_calibration_init(ADC_UNIT_2);
#endif

    return ret;
}

esp_err_t adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width_bit, int *raw_out)
{
    //On ESP32C3, the data width is always 12-bits.
    if (width_bit != ADC_WIDTH_BIT_12) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    if (adc_lock_try_acquire(ADC_UNIT_2) != ESP_OK) {
        return ESP_ERR_TIMEOUT;
    }

    periph_module_enable(PERIPH_SARADC_MODULE);
    adc_power_acquire();
    adc_ll_digi_clk_sel(0);

    adc_arbiter_t config = ADC_ARBITER_CONFIG_DEFAULT();
    adc_hal_arbiter_config(&config);

    adc_atten_t atten = s_atten2_single[channel];
#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_set_hw_calibration_code(ADC_UNIT_2, atten);
#endif

    ADC_REG_LOCK_ENTER();
    adc_oneshot_ll_set_atten(ADC_UNIT_2, channel, atten);
    ret = adc_hal_convert(ADC_UNIT_2, channel, raw_out);
    ADC_REG_LOCK_EXIT();

    adc_power_release();
    periph_module_disable(PERIPH_SARADC_MODULE);
    adc_lock_release(ADC_UNIT_2);

    return ret;
}
#endif //#if (SOC_ADC_PERIPH_NUM >= 2)
#endif  //#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED


static void adc_hal_onetime_start(adc_unit_t adc_n)
{
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    (void)adc_n;
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
    adc_oneshot_ll_start(adc_n);
#endif
}

static esp_err_t adc_hal_convert(adc_unit_t adc_n, int channel, int *out_raw)
{
    uint32_t event = (adc_n == ADC_UNIT_1) ? ADC_LL_EVENT_ADC1_ONESHOT_DONE : ADC_LL_EVENT_ADC2_ONESHOT_DONE;
    adc_oneshot_ll_clear_event(event);
    adc_oneshot_ll_disable_all_unit();
    adc_oneshot_ll_enable(adc_n);
    adc_oneshot_ll_set_channel(adc_n, channel);

    adc_hal_onetime_start(adc_n);

    while (adc_oneshot_ll_get_event(event) != true) {
        ;
    }

    *out_raw = adc_oneshot_ll_get_raw_result(adc_n);
    if (adc_oneshot_ll_raw_check_valid(adc_n, *out_raw) == false) {
        return ESP_ERR_INVALID_STATE;
    }

    //HW workaround: when enabling periph clock, this should be false
    adc_oneshot_ll_disable_all_unit();

    return ESP_OK;
}

/**
 * @brief This function will be called during start up, to check that adc_oneshot driver is not running along with the legacy adc oneshot driver
 */
__attribute__((constructor))
static void check_adc_oneshot_driver_conflict(void)
{
    // This function was declared as weak here. adc_oneshot driver has one implementation.
    // So if adc_oneshot driver is not linked in, then `adc_oneshot_new_unit` should be NULL at runtime.
    extern __attribute__((weak)) esp_err_t adc_oneshot_new_unit(const void *init_config, void **ret_unit);
    if ((void *)adc_oneshot_new_unit != NULL) {
        ESP_EARLY_LOGE(ADC_TAG, "CONFLICT! driver_ng is not allowed to be used with the legacy driver");
        abort();
    }
    ESP_EARLY_LOGW(ADC_TAG, "legacy driver is deprecated, please migrate to `esp_adc/adc_oneshot.h`");
}

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
/*---------------------------------------------------------------
            ADC Hardware Calibration
---------------------------------------------------------------*/
static __attribute__((constructor)) void adc_hw_calibration(void)
{
    //Calculate all ICode
    for (int i = 0; i < SOC_ADC_PERIPH_NUM; i++) {
        adc_hal_calibration_init(i);
        for (int j = 0; j < SOC_ADC_ATTEN_NUM; j++) {
            /**
             * This may get wrong when attenuations are NOT consecutive on some chips,
             * update this when bringing up the calibration on that chip
             */
            adc_calc_hw_calibration_code(i, j);
        }
    }
}
#endif  //#if SOC_ADC_CALIBRATION_V1_SUPPORTED
