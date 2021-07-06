// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "soc/rtc.h"
#include "driver/rtc_io.h"
#include "driver/dac.h"
#include "sys/lock.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "adc1_private.h"

#include "hal/adc_types.h"
#include "hal/adc_hal.h"
#include "hal/dac_hal.h"

#define ADC_CHECK_RET(fun_ret) ({                  \
    if (fun_ret != ESP_OK) {                                \
        ESP_LOGE(ADC_TAG,"%s:%d\n",__FUNCTION__,__LINE__);  \
        return ESP_FAIL;                                    \
    }                                                       \
})

static const char *ADC_TAG = "ADC";

#define ADC_CHECK(a, str, ret_val) ({                                               \
    if (!(a)) {                                                                     \
        ESP_LOGE(ADC_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str);   \
        return (ret_val);                                                           \
    }                                                                               \
})

#define ADC_GET_IO_NUM(periph, channel) (adc_channel_io_map[periph][channel])

#define ADC_CHANNEL_CHECK(periph, channel) ADC_CHECK(channel < SOC_ADC_CHANNEL_NUM(periph), "ADC"#periph" channel error", ESP_ERR_INVALID_ARG)

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define ADC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define ADC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

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

adc2_spinlock should be acquired first, then adc2_wifi_lock or rtc_spinlock.
*/
// This gets incremented when adc_power_acquire() is called, and decremented when
// adc_power_release() is called. ADC is powered down when the value reaches zero.
// Should be modified within critical section (ADC_ENTER/EXIT_CRITICAL).
static int s_adc_power_on_cnt;

static void adc_power_on_internal(void);
static void adc_power_off_internal(void);

#ifdef CONFIG_IDF_TARGET_ESP32
//prevent ADC2 being used by wifi and other tasks at the same time.
static _lock_t adc2_wifi_lock;
/** For ESP32S2 the ADC2 The right to use ADC2 is controlled by the arbiter, and there is no need to set a lock. */
#define ADC2_WIFI_LOCK_ACQUIRE()        _lock_acquire( &adc2_wifi_lock )
#define ADC2_WIFI_LOCK_RELEASE()        _lock_release( &adc2_wifi_lock )
#define ADC2_WIFI_LOCK_TRY_ACQUIRE()    _lock_try_acquire( &adc2_wifi_lock )
#define ADC2_WIFI_LOCK_CHECK()          ((uint32_t *)adc2_wifi_lock != NULL)

#elif defined CONFIG_IDF_TARGET_ESP32S2

#define ADC2_WIFI_LOCK_ACQUIRE()
#define ADC2_WIFI_LOCK_RELEASE()
#define ADC2_WIFI_LOCK_TRY_ACQUIRE()    (0)     //WIFI controller and rtc controller have independent parameter configuration.
#define ADC2_WIFI_LOCK_CHECK()          (true)

#endif

//prevent ADC2 being used by tasks (regardless of WIFI)
static portMUX_TYPE adc2_spinlock = portMUX_INITIALIZER_UNLOCKED;
#define ADC2_ENTER_CRITICAL()   portENTER_CRITICAL( &adc2_spinlock )
#define ADC2_EXIT_CRITICAL()    portEXIT_CRITICAL( &adc2_spinlock )

//prevent ADC1 being used by I2S dma and other tasks at the same time.
static _lock_t adc1_dma_lock;
#define ADC1_DMA_LOCK_ACQUIRE() _lock_acquire( &adc1_dma_lock )
#define ADC1_DMA_LOCK_RELEASE() _lock_release( &adc1_dma_lock )

#ifdef CONFIG_IDF_TARGET_ESP32S2
#ifdef CONFIG_PM_ENABLE
static esp_pm_lock_handle_t s_adc2_arbiter_lock;
#endif  //CONFIG_PM_ENABLE
#endif  //CONFIG_IDF_TARGET_ESP32S2

/*---------------------------------------------------------------
                    ADC Common
---------------------------------------------------------------*/

void adc_power_acquire(void)
{
    ADC_ENTER_CRITICAL();
    s_adc_power_on_cnt++;
    if (s_adc_power_on_cnt == 1) {
        adc_power_on_internal();
    }
    ADC_EXIT_CRITICAL();
}

void adc_power_release(void)
{
    ADC_ENTER_CRITICAL();
    s_adc_power_on_cnt--;
    /* Sanity check */
    if (s_adc_power_on_cnt < 0) {
        ADC_EXIT_CRITICAL();
        ESP_LOGE(ADC_TAG, "%s called, but s_adc_power_on_cnt == 0", __func__);
        abort();
    } else if (s_adc_power_on_cnt == 0) {
        adc_power_off_internal();
    }
    ADC_EXIT_CRITICAL();
}

static void adc_power_on_internal(void)
{
    ADC_ENTER_CRITICAL();
    /* Set the power always on to increase precision. */
    adc_hal_set_power_manage(ADC_POWER_SW_ON);
    ADC_EXIT_CRITICAL();
}

void adc_power_on(void) __attribute__((alias("adc_power_on_internal")));

static void adc_power_off_internal(void)
{
#if CONFIG_IDF_TARGET_ESP32
    adc_hal_set_power_manage(ADC_POWER_SW_OFF);
#else
    adc_hal_set_power_manage(ADC_POWER_BY_FSM);
#endif
}

void adc_power_off(void) __attribute__((alias("adc_power_off_internal")));

esp_err_t adc_set_clk_div(uint8_t clk_div)
{
    ADC_ENTER_CRITICAL();
    adc_hal_digi_set_clk_div(clk_div);
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

static void adc_rtc_chan_init(adc_unit_t adc_unit)
{
    if (adc_unit & ADC_UNIT_1) {
        /* Workaround: Disable the synchronization operation function of ADC1 and DAC.
           If enabled(default), ADC RTC controller sampling will cause the DAC channel output voltage. */
        dac_hal_rtc_sync_by_adc(false);
        adc_hal_rtc_output_invert(ADC_NUM_1, SOC_ADC1_DATA_INVERT_DEFAULT);
        adc_hal_set_sar_clk_div(ADC_NUM_1, SOC_ADC_SAR_CLK_DIV_DEFAULT(ADC_NUM_1));
#ifdef CONFIG_IDF_TARGET_ESP32
        adc_hal_hall_disable(); //Disable other peripherals.
        adc_hal_amp_disable();  //Currently the LNA is not open, close it by default.
#endif
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_hal_pwdet_set_cct(SOC_ADC_PWDET_CCT_DEFAULT);
        adc_hal_rtc_output_invert(ADC_NUM_2, SOC_ADC2_DATA_INVERT_DEFAULT);
        adc_hal_set_sar_clk_div(ADC_NUM_2, SOC_ADC_SAR_CLK_DIV_DEFAULT(ADC_NUM_2));
    }
}

esp_err_t adc_gpio_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    if (adc_unit & ADC_UNIT_1) {
        ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
        gpio_num = ADC_GET_IO_NUM(ADC_NUM_1, channel);
        ADC_CHECK_RET(rtc_gpio_init(gpio_num));
        ADC_CHECK_RET(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
        ADC_CHECK_RET(rtc_gpio_pulldown_dis(gpio_num));
        ADC_CHECK_RET(rtc_gpio_pullup_dis(gpio_num));
    }
    if (adc_unit & ADC_UNIT_2) {
        ADC_CHANNEL_CHECK(ADC_NUM_2, channel);
        gpio_num = ADC_GET_IO_NUM(ADC_NUM_2, channel);
        ADC_CHECK_RET(rtc_gpio_init(gpio_num));
        ADC_CHECK_RET(rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED));
        ADC_CHECK_RET(rtc_gpio_pulldown_dis(gpio_num));
        ADC_CHECK_RET(rtc_gpio_pullup_dis(gpio_num));
    }
    return ESP_OK;
}

esp_err_t adc_set_data_inv(adc_unit_t adc_unit, bool inv_en)
{
    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        adc_hal_rtc_output_invert(ADC_NUM_1, inv_en);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_hal_rtc_output_invert(ADC_NUM_1, inv_en);
    }
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t adc_set_data_width(adc_unit_t adc_unit, adc_bits_width_t bits)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    ADC_CHECK(bits < ADC_WIDTH_MAX, "WIDTH ERR: ESP32 support 9 ~ 12 bit width", ESP_ERR_INVALID_ARG);
#elif defined CONFIG_IDF_TARGET_ESP32S2
    ADC_CHECK(bits == ADC_WIDTH_BIT_13, "WIDTH ERR: ESP32S2 support 13 bit width", ESP_ERR_INVALID_ARG);
#endif

    ADC_ENTER_CRITICAL();
    if (adc_unit & ADC_UNIT_1) {
        adc_hal_rtc_set_output_format(ADC_NUM_1, bits);
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_hal_rtc_set_output_format(ADC_NUM_2, bits);
    }
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

/**
 * @brief Reset RTC controller FSM.
 *
 * @return
 *      - ESP_OK Success
 */
#ifdef CONFIG_IDF_TARGET_ESP32S2
esp_err_t adc_rtc_reset(void)
{
    ADC_ENTER_CRITICAL();
    adc_hal_rtc_reset();
    ADC_EXIT_CRITICAL();
    return ESP_OK;
}

static inline void adc_set_init_code(adc_ll_num_t adc_n, adc_channel_t channel)
{
    adc_atten_t atten = adc_hal_get_atten(adc_n, channel);
    uint32_t cal_val = adc_hal_calibration(adc_n, channel, atten, true, false);
    adc_hal_set_calibration_param(adc_n, cal_val);
    ESP_LOGD(ADC_TAG, "Set cal adc %d\n", cal_val);
}
#endif

/*-------------------------------------------------------------------------------------
 *                      ADC1
 *------------------------------------------------------------------------------------*/
esp_err_t adc1_pad_get_io_num(adc1_channel_t channel, gpio_num_t *gpio_num)
{
    ADC_CHANNEL_CHECK(ADC_NUM_1, channel);

    int io = ADC_GET_IO_NUM(ADC_NUM_1, channel);
    if (io < 0) {
        return ESP_ERR_INVALID_ARG;
    } else {
        *gpio_num = (gpio_num_t)io;
    }

    return ESP_OK;
}

esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)
{
    ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
    ADC_CHECK(atten < ADC_ATTEN_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);

    adc_gpio_init(ADC_UNIT_1, channel);
    ADC_ENTER_CRITICAL();
    adc_rtc_chan_init(ADC_UNIT_1);
    adc_hal_set_atten(ADC_NUM_1, channel, atten);
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t adc1_config_width(adc_bits_width_t width_bit)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    ADC_CHECK(width_bit < ADC_WIDTH_MAX, "WIDTH ERR: ESP32 support 9 ~ 12 bit width", ESP_ERR_INVALID_ARG);
#elif defined CONFIG_IDF_TARGET_ESP32S2
    ADC_CHECK(width_bit == ADC_WIDTH_BIT_13, "WIDTH ERR: ESP32S2 support 13 bit width", ESP_ERR_INVALID_ARG);
#endif

    ADC_ENTER_CRITICAL();
    adc_hal_rtc_set_output_format(ADC_NUM_1, width_bit);
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t adc1_dma_mode_acquire(void)
{
    /* Use locks to avoid digtal and RTC controller conflicts.
       for adc1, block until acquire the lock. */
    ADC1_DMA_LOCK_ACQUIRE();
    ESP_LOGD( ADC_TAG, "dma mode takes adc1 lock." );

    ADC_ENTER_CRITICAL();
    adc_hal_set_power_manage(ADC_POWER_SW_ON);
    /* switch SARADC into DIG channel */
    adc_hal_set_controller(ADC_NUM_1, ADC_CTRL_DIG);
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t adc1_rtc_mode_acquire(void)
{
    /* Use locks to avoid digtal and RTC controller conflicts.
       for adc1, block until acquire the lock. */
    ADC1_DMA_LOCK_ACQUIRE();

    ADC_ENTER_CRITICAL();
    /* switch SARADC into RTC channel. */
    adc_hal_set_controller(ADC_NUM_1, ADC_CTRL_RTC);
    ADC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t adc1_lock_release(void)
{
    ADC_CHECK((uint32_t *)adc1_dma_lock != NULL, "adc1 lock release called before acquire", ESP_ERR_INVALID_STATE );
    /* Use locks to avoid digtal and RTC controller conflicts. for adc1, block until acquire the lock. */
    ADC1_DMA_LOCK_RELEASE();
    return ESP_OK;
}

int adc1_get_raw(adc1_channel_t channel)
{
    int adc_value;
    ADC_CHANNEL_CHECK(ADC_NUM_1, channel);
    adc1_rtc_mode_acquire();
    adc_power_acquire();

    ADC_ENTER_CRITICAL();
#ifdef CONFIG_IDF_TARGET_ESP32S2
    adc_set_init_code(ADC_NUM_1, channel);             // calibration for adc
#endif
    adc_hal_set_controller(ADC_NUM_1, ADC_CTRL_RTC);    //Set controller
    adc_hal_convert(ADC_NUM_1, channel, &adc_value);   //Start conversion, For ADC1, the data always valid.
    ADC_EXIT_CRITICAL();
#ifdef CONFIG_IDF_TARGET_ESP32S2
    adc_hal_rtc_reset();    //Reset FSM of rtc controller
#endif

    adc_power_release();
    adc1_lock_release();
    return adc_value;
}

int adc1_get_voltage(adc1_channel_t channel)    //Deprecated. Use adc1_get_raw() instead
{
    return adc1_get_raw(channel);
}

void adc1_ulp_enable(void)
{
    adc_power_acquire();

    ADC_ENTER_CRITICAL();
    adc_hal_set_controller(ADC_NUM_1, ADC_CTRL_ULP);
    /* since most users do not need LNA and HALL with uLP, we disable them here
       open them in the uLP if needed. */
#ifdef CONFIG_IDF_TARGET_ESP32
    /* disable other peripherals. */
    adc_hal_hall_disable();
    adc_hal_amp_disable();
#endif
    ADC_EXIT_CRITICAL();
}

/*---------------------------------------------------------------
                    ADC2
---------------------------------------------------------------*/
esp_err_t adc2_pad_get_io_num(adc2_channel_t channel, gpio_num_t *gpio_num)
{
    ADC_CHANNEL_CHECK(ADC_NUM_2, channel);

    int io = ADC_GET_IO_NUM(ADC_NUM_2, channel);
    if (io < 0) {
        return ESP_ERR_INVALID_ARG;
    } else {
        *gpio_num = (gpio_num_t)io;
    }

    return ESP_OK;
}

/** For ESP32S2 the ADC2 The right to use ADC2 is controlled by the arbiter, and there is no need to set a lock.*/
esp_err_t adc2_wifi_acquire(void)
{
    /* Wi-Fi module will use adc2. Use locks to avoid conflicts. */
    ADC2_WIFI_LOCK_ACQUIRE();
    ESP_LOGD( ADC_TAG, "Wi-Fi takes adc2 lock." );
    return ESP_OK;
}

esp_err_t adc2_wifi_release(void)
{
    ADC_CHECK(ADC2_WIFI_LOCK_CHECK(), "wifi release called before acquire", ESP_ERR_INVALID_STATE );
    ADC2_WIFI_LOCK_RELEASE();
    ESP_LOGD( ADC_TAG, "Wi-Fi returns adc2 lock." );

    return ESP_OK;
}

esp_err_t adc2_config_channel_atten(adc2_channel_t channel, adc_atten_t atten)
{
    ADC_CHANNEL_CHECK(ADC_NUM_2, channel);
    ADC_CHECK(atten <= ADC_ATTEN_11db, "ADC2 Atten Err", ESP_ERR_INVALID_ARG);

    adc_gpio_init(ADC_UNIT_2, channel);
    ADC2_ENTER_CRITICAL();
    //avoid collision with other tasks
    if ( ADC2_WIFI_LOCK_TRY_ACQUIRE() == -1 ) {
        //try the lock, return if failed (wifi using).
        ADC2_EXIT_CRITICAL();
        adc_power_release();
        return ESP_ERR_TIMEOUT;
    }
    adc_rtc_chan_init(ADC_UNIT_2);
    adc_hal_set_atten(ADC_NUM_2, channel, atten);
    ADC2_WIFI_LOCK_RELEASE();
    ADC2_EXIT_CRITICAL();

    return ESP_OK;
}

static inline void adc2_config_width(adc_bits_width_t width_bit)
{
#ifdef CONFIG_IDF_TARGET_ESP32S2
#ifdef CONFIG_PM_ENABLE
    /* Lock APB clock. */
    if (s_adc2_arbiter_lock == NULL) {
        esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "adc2", &s_adc2_arbiter_lock);
    }
#endif  //CONFIG_PM_ENABLE
#endif  //CONFIG_IDF_TARGET_ESP32S2
    ADC_ENTER_CRITICAL();
    adc_hal_rtc_set_output_format(ADC_NUM_2, width_bit);
    ADC_EXIT_CRITICAL();
}

static inline void adc2_dac_disable( adc2_channel_t channel)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    if ( channel == ADC2_CHANNEL_8 ) { // the same as DAC channel 1
        dac_output_disable(DAC_CHANNEL_1);
    } else if ( channel == ADC2_CHANNEL_9 ) {
        dac_output_disable(DAC_CHANNEL_2);
    }
#elif defined CONFIG_IDF_TARGET_ESP32S2
    if ( channel == ADC2_CHANNEL_6 ) { // the same as DAC channel 1
        dac_output_disable(DAC_CHANNEL_1);
    } else if ( channel == ADC2_CHANNEL_7 ) {
        dac_output_disable(DAC_CHANNEL_2);
    }
#endif
}

/**
 * @note For ESP32S2:
 *       The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 *       Or, the RTC controller will fail when get raw data.
 *       This issue does not occur on digital controllers (DMA mode), and the hardware guarantees that there will be no errors.
 */
esp_err_t adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width_bit, int *raw_out)
{
    int adc_value = 0;

    ADC_CHECK(raw_out != NULL, "ADC out value err", ESP_ERR_INVALID_ARG);
    ADC_CHECK(channel < ADC2_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);
#ifdef CONFIG_IDF_TARGET_ESP32
    ADC_CHECK(width_bit < ADC_WIDTH_MAX, "WIDTH ERR: ESP32 support 9 ~ 12 bit width", ESP_ERR_INVALID_ARG);
#elif defined CONFIG_IDF_TARGET_ESP32S2
    ADC_CHECK(width_bit == ADC_WIDTH_BIT_13, "WIDTH ERR: ESP32S2 support 13 bit width", ESP_ERR_INVALID_ARG);
#endif

    adc_power_acquire();         //in critical section with whole rtc module

    ADC2_ENTER_CRITICAL();  //avoid collision with other tasks

    if ( ADC2_WIFI_LOCK_TRY_ACQUIRE() == -1 ) { //try the lock, return if failed (wifi using).
        ADC2_EXIT_CRITICAL();
        return ESP_ERR_TIMEOUT;
    }
#ifdef CONFIG_ADC_DISABLE_DAC
    adc2_dac_disable(channel);      //disable other peripherals
#endif
    adc2_config_width(width_bit);   // in critical section with whole rtc module. because the PWDET use the same registers, place it here.
#ifdef CONFIG_IDF_TARGET_ESP32S2
    adc_set_init_code(ADC_NUM_2, channel);  // calibration for adc
#endif
    adc_hal_set_controller(ADC_NUM_2, ADC_CTRL_RTC);// set controller

#ifdef CONFIG_IDF_TARGET_ESP32S2
#ifdef CONFIG_PM_ENABLE
    if (s_adc2_arbiter_lock) {
        esp_pm_lock_acquire(s_adc2_arbiter_lock);
    }
#endif //CONFIG_PM_ENABLE
#endif //CONFIG_IDF_TARGET_ESP32

    if (adc_hal_convert(ADC_NUM_2, channel, &adc_value)) {
        adc_value = -1;
    }

#ifdef CONFIG_IDF_TARGET_ESP32S2
#ifdef CONFIG_PM_ENABLE
    /* Release APB clock. */
    if (s_adc2_arbiter_lock) {
        esp_pm_lock_release(s_adc2_arbiter_lock);
    }
#endif //CONFIG_PM_ENABLE
#endif //CONFIG_IDF_TARGET_ESP32

    ADC2_WIFI_LOCK_RELEASE();
    ADC2_EXIT_CRITICAL();

#ifdef CONFIG_IDF_TARGET_ESP32S2
    adc_rtc_reset();
#endif

    if (adc_value < 0) {
        ESP_LOGD( ADC_TAG, "ADC2 ARB: Return data is invalid." );
        adc_power_release();
        return ESP_ERR_INVALID_STATE;
    }

    //in critical section with whole rtc module
    adc_power_release();
    *raw_out = adc_value;
    return ESP_OK;
}

esp_err_t adc2_vref_to_gpio(gpio_num_t gpio)
{
    return adc_vref_to_gpio(ADC_UNIT_2, gpio);
}

esp_err_t adc_vref_to_gpio(adc_unit_t adc_unit, gpio_num_t gpio)
{
    adc_power_acquire();
#ifdef CONFIG_IDF_TARGET_ESP32
    if (adc_unit & ADC_UNIT_1) {
        adc_power_release();
        return ESP_ERR_INVALID_ARG;
    }
#endif
    adc2_channel_t ch = ADC2_CHANNEL_MAX;
    /* Check if the GPIO supported. */
    for (int i = 0; i < ADC2_CHANNEL_MAX; i++) {
        if (gpio == ADC_GET_IO_NUM(ADC_NUM_2, i)) {
            ch = i;
            break;
        }
    }
    if (ch == ADC2_CHANNEL_MAX) {
        adc_power_release();
        return ESP_ERR_INVALID_ARG;
    }

    ADC_ENTER_CRITICAL();
    adc_hal_set_power_manage(ADC_POWER_SW_ON);
    if (adc_unit & ADC_UNIT_1) {
        adc_hal_vref_output(ADC_NUM_1, ch, true);
    } else if (adc_unit & ADC_UNIT_2) {
        adc_hal_vref_output(ADC_NUM_2, ch, true);
    }
    ADC_EXIT_CRITICAL();

    //Configure RTC gpio, Only ADC2's channels IO are supported to output reference voltage.
    adc_gpio_init(ADC_UNIT_2, ch);
    return ESP_OK;
}
