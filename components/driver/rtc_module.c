// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_log.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "soc/syscon_periph.h"
#include "soc/rtc.h"
#include "soc/periph_defs.h"
#include "rtc_io.h"
#include "touch_pad.h"
#include "adc.h"
#include "dac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_intr_alloc.h"
#include "sys/lock.h"
#include "driver/rtc_cntl.h"
#include "driver/gpio.h"
#include "adc1_i2s_private.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/ets_sys.h"
#endif

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "sys/queue.h"


#define ADC_FSM_RSTB_WAIT_DEFAULT     (8)
#define ADC_FSM_START_WAIT_DEFAULT    (5)
#define ADC_FSM_STANDBY_WAIT_DEFAULT  (100)
#define ADC_FSM_TIME_KEEP             (-1)
#define ADC_MAX_MEAS_NUM_DEFAULT      (255)
#define ADC_MEAS_NUM_LIM_DEFAULT      (1)
#define SAR_ADC_CLK_DIV_DEFUALT       (2)
#define ADC_PATT_LEN_MAX              (16)
#define TOUCH_PAD_FILTER_FACTOR_DEFAULT   (4)   // IIR filter coefficient.
#define TOUCH_PAD_SHIFT_DEFAULT           (4)   // Increase computing accuracy.
#define TOUCH_PAD_SHIFT_ROUND_DEFAULT     (8)   // ROUND = 2^(n-1); rounding off for fractional.
#define DAC_ERR_STR_CHANNEL_ERROR   "DAC channel error"

static const char *RTC_MODULE_TAG = "RTC_MODULE";

#define RTC_MODULE_CHECK(a, str, ret_val) if (!(a)) {                                \
    ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str); \
    return (ret_val);                                                              \
}

#define RTC_RES_CHECK(res, ret_val) if ( (a) != ESP_OK) {                           \
        ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s)", __FILE__, __LINE__, __FUNCTION__); \
        return (ret_val);                                                              \
}

#define ADC_CHECK_UNIT(unit) RTC_MODULE_CHECK(adc_unit < ADC_UNIT_2, "ADC unit error, only support ADC1 for now", ESP_ERR_INVALID_ARG)

#define ADC1_CHECK_FUNCTION_RET(fun_ret) if(fun_ret!=ESP_OK){\
    ESP_LOGE(RTC_MODULE_TAG,"%s:%d\n",__FUNCTION__,__LINE__);\
    return ESP_FAIL;\
}

#define ADC2_CHECK_FUNCTION_RET(fun_ret) do { if(fun_ret!=ESP_OK){\
    ESP_LOGE(RTC_MODULE_TAG,"%s:%d\n",__FUNCTION__,__LINE__);\
    return ESP_FAIL;\
} }while (0)

portMUX_TYPE rtc_spinlock = portMUX_INITIALIZER_UNLOCKED;
#if CONFIG_IDF_TARGET_ESP32
static SemaphoreHandle_t rtc_touch_mux = NULL;
#endif
/*
In ADC2, there're two locks used for different cases:
1. lock shared with app and WIFI:
   when wifi using the ADC2, we assume it will never stop,
   so app checks the lock and returns immediately if failed.

2. lock shared between tasks:
   when several tasks sharing the ADC2, we want to guarantee
   all the requests will be handled.
   Since conversions are short (about 31us), app returns the lock very soon,
   we use a spinlock to stand there waiting to do conversions one by one.

adc2_spinlock should be acquired first, then adc2_wifi_lock or rtc_spinlock.
*/
//prevent ADC2 being used by wifi and other tasks at the same time.
static _lock_t adc2_wifi_lock;
//prevent ADC2 being used by tasks (regardless of WIFI)
portMUX_TYPE adc2_spinlock = portMUX_INITIALIZER_UNLOCKED;

//prevent ADC1 being used by I2S dma and other tasks at the same time.
static _lock_t adc1_i2s_lock;

#if CONFIG_IDF_TARGET_ESP32
typedef struct {
    TimerHandle_t timer;
    uint16_t filtered_val[TOUCH_PAD_MAX];
    uint16_t raw_val[TOUCH_PAD_MAX];
    uint32_t filter_period;
    uint32_t period;
    bool enable;
} touch_pad_filter_t;
static touch_pad_filter_t *s_touch_pad_filter = NULL;
// check if touch pad be inited.
static uint16_t s_touch_pad_init_bit = 0x0000;
static filter_cb_t s_filter_cb = NULL;
#endif

typedef enum {
    ADC_CTRL_RTC = 0,
    ADC_CTRL_ULP = 1,
    ADC_CTRL_DIG = 2,
    ADC2_CTRL_PWDET = 3,
} adc_controller_t ;

static const char TAG[] = "adc";

static inline void dac_output_set_enable(dac_channel_t channel, bool enable);
static inline void adc1_hall_enable(bool enable);


/*---------------------------------------------------------------
                        RTC IO
---------------------------------------------------------------*/
esp_err_t rtc_gpio_init(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    // 0: GPIO connected to digital GPIO module. 1: GPIO connected to analog RTC module.
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, (rtc_gpio_desc[gpio_num].mux));
    //0:RTC FUNCIOTN 1,2,3:Reserved
    SET_PERI_REG_BITS(rtc_gpio_desc[gpio_num].reg, RTC_IO_TOUCH_PAD1_FUN_SEL_V, 0x0, rtc_gpio_desc[gpio_num].func);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    rtc_gpio_reg[gpio_num]->mux_sel = 0x1;
    rtc_gpio_reg[gpio_num]->fun_sel = 0x0;
#endif
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t rtc_gpio_deinit(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    //Select Gpio as Digital Gpio
#if CONFIG_IDF_TARGET_ESP32
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, (rtc_gpio_desc[gpio_num].mux));
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    rtc_gpio_reg[gpio_num]->mux_sel = 0x0;
#endif
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

static esp_err_t rtc_gpio_output_enable(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32
    int rtc_gpio_num = rtc_gpio_desc[gpio_num].rtc_num;
    RTC_MODULE_CHECK(rtc_gpio_num != -1, "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    SET_PERI_REG_MASK(RTC_GPIO_ENABLE_W1TS_REG, (1 << (rtc_gpio_num + RTC_GPIO_ENABLE_W1TS_S)));
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    SET_PERI_REG_MASK(RTC_GPIO_ENABLE_W1TS_REG, (1 << ( gpio_num + RTC_GPIO_ENABLE_W1TS_S)));
#endif
    return ESP_OK;
}

static esp_err_t rtc_gpio_output_disable(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32
    int rtc_gpio_num = rtc_gpio_desc[gpio_num].rtc_num;
    RTC_MODULE_CHECK(rtc_gpio_num != -1, "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    SET_PERI_REG_MASK(RTC_GPIO_ENABLE_W1TC_REG, (1 << ( rtc_gpio_num + RTC_GPIO_ENABLE_W1TC_S)));
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    SET_PERI_REG_MASK(RTC_GPIO_ENABLE_W1TC_REG, (1 << ( gpio_num + RTC_GPIO_ENABLE_W1TC_S)));
#endif
    return ESP_OK;
}

static esp_err_t rtc_gpio_input_enable(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].ie);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    rtc_gpio_reg[gpio_num]->fun_ie = 1;
#endif
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

static esp_err_t rtc_gpio_input_disable(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].ie);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    rtc_gpio_reg[gpio_num]->fun_ie = 0;
#endif
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

#if CONFIG_IDF_TARGET_ESP32S2BETA
esp_err_t rtc_gpio_sleep_output_enable(gpio_num_t gpio_num, bool output)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    rtc_gpio_reg[gpio_num]->slp_sel = 1;
    rtc_gpio_reg[gpio_num]->slp_oe = output;
    return ESP_OK;
}

esp_err_t rtc_gpio_sleep_input_enable(gpio_num_t gpio_num, bool input)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    rtc_gpio_reg[gpio_num]->slp_sel = 1;
    rtc_gpio_reg[gpio_num]->slp_ie = input;
    return ESP_OK;
}

esp_err_t rtc_gpio_sleep_mode_disable(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    rtc_gpio_reg[gpio_num]->slp_sel = 0;
    return ESP_OK;
}
#endif

esp_err_t rtc_gpio_set_level(gpio_num_t gpio_num, uint32_t level)
{
#if CONFIG_IDF_TARGET_ESP32
    int rtc_gpio_num = rtc_gpio_num = rtc_gpio_desc[gpio_num].rtc_num;;
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);

    if (level) {
        WRITE_PERI_REG(RTC_GPIO_OUT_W1TS_REG, (1 << (rtc_gpio_num + RTC_GPIO_OUT_DATA_W1TS_S)));
    } else {
        WRITE_PERI_REG(RTC_GPIO_OUT_W1TC_REG, (1 << (rtc_gpio_num + RTC_GPIO_OUT_DATA_W1TC_S)));
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    if (level) {
        WRITE_PERI_REG(RTC_GPIO_OUT_W1TS_REG, (1 << (gpio_num + RTC_GPIO_OUT_DATA_W1TS_S)));
    } else {
        WRITE_PERI_REG(RTC_GPIO_OUT_W1TC_REG, (1 << (gpio_num + RTC_GPIO_OUT_DATA_W1TC_S)));
    }
#endif
    return ESP_OK;
}

uint32_t rtc_gpio_get_level(gpio_num_t gpio_num)
{
    uint32_t level = 0;
#if CONFIG_IDF_TARGET_ESP32
    int rtc_gpio_num = rtc_gpio_desc[gpio_num].rtc_num;
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    level = READ_PERI_REG(RTC_GPIO_IN_REG);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ((level >> (RTC_GPIO_IN_NEXT_S + rtc_gpio_num)) & 0x01);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    level = RTCIO.in_val.in;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ((level >> gpio_num) & 0x1);
#endif
}

esp_err_t rtc_gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Output pad only", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(strength < GPIO_DRIVE_CAP_MAX, "GPIO drive capability error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    SET_PERI_REG_BITS(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].drv_v, strength, rtc_gpio_desc[gpio_num].drv_s);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    rtc_gpio_reg[gpio_num]->drv = strength;
#endif
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t rtc_gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t* strength)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Output pad only", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(strength != NULL, "GPIO drive pointer error", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    *strength = GET_PERI_REG_BITS2(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].drv_v, rtc_gpio_desc[gpio_num].drv_s);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    *strength = rtc_gpio_reg[gpio_num]->drv;
#endif
    return ESP_OK;
}

esp_err_t rtc_gpio_set_direction(gpio_num_t gpio_num, rtc_gpio_mode_t mode)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);

    switch (mode) {
    case RTC_GPIO_MODE_INPUT_ONLY:
        rtc_gpio_output_disable(gpio_num);
        rtc_gpio_input_enable(gpio_num);
        break;
    case RTC_GPIO_MODE_OUTPUT_ONLY:
        rtc_gpio_output_enable(gpio_num);
        rtc_gpio_input_disable(gpio_num);
        break;
    case RTC_GPIO_MODE_INPUT_OUTPUT:
        rtc_gpio_output_enable(gpio_num);
        rtc_gpio_input_enable(gpio_num);
        break;
    case RTC_GPIO_MODE_DISABLED:
        rtc_gpio_output_disable(gpio_num);
        rtc_gpio_input_disable(gpio_num);
        break;
    }

    return ESP_OK;
}

esp_err_t rtc_gpio_pullup_en(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32
    //this is a digital pad
    if (rtc_gpio_desc[gpio_num].pullup == 0) {
        return ESP_ERR_INVALID_ARG;
    }
#endif
    //this is a rtc pad
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].pullup);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    rtc_gpio_reg[gpio_num]->rue = 0x1;
#endif
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

#if CONFIG_IDF_TARGET_ESP32S2BETA
esp_err_t rtc_gpio_set_output_mode(gpio_num_t gpio_num, rtc_io_out_mode_t mode)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    RTCIO.pin[gpio_num].pad_driver = mode;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t rtc_gpio_get_output_mode(gpio_num_t gpio_num, rtc_io_out_mode_t *mode)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    *mode = RTCIO.pin[gpio_num].pad_driver;
    return ESP_OK;
}
#endif

esp_err_t rtc_gpio_pulldown_en(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32
    //this is a digital pad
    if (rtc_gpio_desc[gpio_num].pulldown == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    //this is a rtc pad
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].pulldown);
    portEXIT_CRITICAL(&rtc_spinlock);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    portENTER_CRITICAL(&rtc_spinlock);
    rtc_gpio_reg[gpio_num]->rde = 0x1;
    portEXIT_CRITICAL(&rtc_spinlock);
#endif
    return ESP_OK;
}

esp_err_t rtc_gpio_pullup_dis(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32
    //this is a digital pad
    if ( rtc_gpio_desc[gpio_num].pullup == 0 ) {
        return ESP_ERR_INVALID_ARG;
    }

    //this is a rtc pad
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].pullup);
    portEXIT_CRITICAL(&rtc_spinlock);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    portENTER_CRITICAL(&rtc_spinlock);
    rtc_gpio_reg[gpio_num]->rue = 0x0;
    portEXIT_CRITICAL(&rtc_spinlock);
#endif
    return ESP_OK;
}

esp_err_t rtc_gpio_pulldown_dis(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32
    //this is a digital pad
    if (rtc_gpio_desc[gpio_num].pulldown == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    //this is a rtc pad
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].pulldown);
    portEXIT_CRITICAL(&rtc_spinlock);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    portENTER_CRITICAL(&rtc_spinlock);
    rtc_gpio_reg[gpio_num]->rde = 0x0;
    portEXIT_CRITICAL(&rtc_spinlock);
#endif
    return ESP_OK;
}

esp_err_t rtc_gpio_hold_en(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32
    // check if an RTC IO
    if (rtc_gpio_desc[gpio_num].pullup == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].hold);
    portEXIT_CRITICAL(&rtc_spinlock);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.pad_hold.val |= BIT(gpio_num);
    portEXIT_CRITICAL(&rtc_spinlock);
#endif
    return ESP_OK;
}

esp_err_t rtc_gpio_hold_dis(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32
    // check if an RTC IO
    if (rtc_gpio_desc[gpio_num].pullup == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].hold);
    portEXIT_CRITICAL(&rtc_spinlock);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.pad_hold.val &= ~(BIT(gpio_num));
    portEXIT_CRITICAL(&rtc_spinlock);
#endif
    return ESP_OK;
}

esp_err_t rtc_gpio_isolate(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32
    if (rtc_gpio_desc[gpio_num].reg == 0) {
        return ESP_ERR_INVALID_ARG;
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
#endif
    rtc_gpio_pullup_dis(gpio_num);
    rtc_gpio_pulldown_dis(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_hold_en(gpio_num);

    return ESP_OK;
}

void rtc_gpio_force_hold_dis_all(void)
{
#if CONFIG_IDF_TARGET_ESP32
    for (int gpio = 0; gpio < GPIO_PIN_COUNT; ++gpio) {
        const rtc_gpio_desc_t* desc = &rtc_gpio_desc[gpio];
        if (desc->hold_force != 0) {
            REG_CLR_BIT(RTC_CNTL_HOLD_FORCE_REG, desc->hold_force);
        }
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.rtc_pwc.rtc_pad_force_hold = 0;
    portEXIT_CRITICAL(&rtc_spinlock);
#endif
}

esp_err_t rtc_gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
#if CONFIG_IDF_TARGET_ESP32
    int rtc_num = rtc_gpio_desc[gpio_num].rtc_num;
    if (rtc_num < 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (( intr_type != GPIO_INTR_LOW_LEVEL ) && ( intr_type != GPIO_INTR_HIGH_LEVEL )) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t reg = RTC_GPIO_PIN0_REG + rtc_num * sizeof(uint32_t);
    /* each pin has its own register, spinlock not needed */
    REG_SET_BIT(reg, RTC_GPIO_PIN0_WAKEUP_ENABLE);
    REG_SET_FIELD(reg, RTC_GPIO_PIN0_INT_TYPE, intr_type);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    if (( intr_type != GPIO_INTR_LOW_LEVEL ) && ( intr_type != GPIO_INTR_HIGH_LEVEL )) {
        return ESP_ERR_INVALID_ARG;
    }
    /* each pin has its own register, spinlock not needed */
    RTCIO.pin[gpio_num].wakeup_enable = 1;
    RTCIO.pin[gpio_num].int_type = intr_type;
#endif
    return ESP_OK;
}

esp_err_t rtc_gpio_wakeup_disable(gpio_num_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32
    int rtc_num = rtc_gpio_desc[gpio_num].rtc_num;
    if (rtc_num < 0) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t reg = RTC_GPIO_PIN0_REG + rtc_num * sizeof(uint32_t);
    /* each pin has its own register, spinlock not needed */
    REG_CLR_BIT(reg, RTC_GPIO_PIN0_WAKEUP_ENABLE);
    REG_SET_FIELD(reg, RTC_GPIO_PIN0_INT_TYPE, 0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    /* each pin has its own register, spinlock not needed */
    RTCIO.pin[gpio_num].wakeup_enable = 0;
    RTCIO.pin[gpio_num].int_type = 0;
#endif
    return ESP_OK;
}

#if CONFIG_IDF_TARGET_ESP32S2BETA
esp_err_t rtc_gpio_force_hold_all()
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.rtc_pwc.rtc_pad_force_hold = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}
#endif
#if CONFIG_IDF_TARGET_ESP32
/*---------------------------------------------------------------
                    Touch Pad
---------------------------------------------------------------*/
//Some register bits of touch sensor 8 and 9 are mismatched, we need to swap the bits.
#define BITSWAP(data, n, m)   (((data >> n) &  0x1)  == ((data >> m) & 0x1) ? (data) : ((data) ^ ((0x1 <<n) | (0x1 << m))))
#define TOUCH_BITS_SWAP(v)  BITSWAP(v, TOUCH_PAD_NUM8, TOUCH_PAD_NUM9)
static esp_err_t _touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value, touch_fsm_mode_t mode);

//Some registers of touch sensor 8 and 9 are mismatched, we need to swap register index
inline static touch_pad_t touch_pad_num_wrap(touch_pad_t touch_num)
{
    if (touch_num == TOUCH_PAD_NUM8) {
        return TOUCH_PAD_NUM9;
    } else if (touch_num == TOUCH_PAD_NUM9) {
        return TOUCH_PAD_NUM8;
    }
    return touch_num;
}

esp_err_t touch_pad_isr_handler_register(void (*fn)(void *), void *arg, int no_use, intr_handle_t *handle_no_use)
{
    RTC_MODULE_CHECK(fn, "Touch_Pad ISR null", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    return rtc_isr_register(fn, arg, RTC_CNTL_TOUCH_INT_ST_M);
#else
    return ESP_FAIL;
#endif
}

esp_err_t touch_pad_isr_register(intr_handler_t fn, void* arg)
{
    RTC_MODULE_CHECK(fn, "Touch_Pad ISR null", ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    return rtc_isr_register(fn, arg, RTC_CNTL_TOUCH_INT_ST_M);
#else
    return ESP_FAIL;
#endif
}

esp_err_t touch_pad_isr_deregister(intr_handler_t fn, void *arg)
{
    return rtc_isr_deregister(fn, arg);
}

static esp_err_t touch_pad_get_io_num(touch_pad_t touch_num, gpio_num_t *gpio_num)
{
    switch (touch_num) {
    case TOUCH_PAD_NUM0:
        *gpio_num = TOUCH_PAD_NUM0_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM1:
        *gpio_num = TOUCH_PAD_NUM1_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM2:
        *gpio_num = TOUCH_PAD_NUM2_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM3:
        *gpio_num = TOUCH_PAD_NUM3_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM4:
        *gpio_num = TOUCH_PAD_NUM4_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM5:
        *gpio_num = TOUCH_PAD_NUM5_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM6:
        *gpio_num = TOUCH_PAD_NUM6_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM7:
        *gpio_num = TOUCH_PAD_NUM7_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM8:
        *gpio_num = TOUCH_PAD_NUM8_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM9:
        *gpio_num = TOUCH_PAD_NUM9_GPIO_NUM;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static uint32_t _touch_filter_iir(uint32_t in_now, uint32_t out_last, uint32_t k)
{
    if (k == 0) {
        return in_now;
    } else {
        uint32_t out_now = (in_now + (k - 1) * out_last) / k;
        return out_now;
    }
}

esp_err_t touch_pad_set_filter_read_cb(filter_cb_t read_cb)
{
    s_filter_cb = read_cb;
    return ESP_OK;
}

static void touch_pad_filter_cb(void *arg)
{
    static uint32_t s_filtered_temp[TOUCH_PAD_MAX] = {0};

    if (s_touch_pad_filter == NULL || rtc_touch_mux == NULL) {
        return;
    }
    uint16_t val = 0;
    touch_fsm_mode_t mode;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    touch_pad_get_fsm_mode(&mode);
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        if ((s_touch_pad_init_bit >> i) & 0x1) {
            _touch_pad_read(i, &val, mode);
            s_touch_pad_filter->raw_val[i] = val;
            s_filtered_temp[i] = s_filtered_temp[i] == 0 ? ((uint32_t)val << TOUCH_PAD_SHIFT_DEFAULT) : s_filtered_temp[i];
            s_filtered_temp[i] = _touch_filter_iir((val << TOUCH_PAD_SHIFT_DEFAULT),
                    s_filtered_temp[i], TOUCH_PAD_FILTER_FACTOR_DEFAULT);
            s_touch_pad_filter->filtered_val[i] = (s_filtered_temp[i] + TOUCH_PAD_SHIFT_ROUND_DEFAULT) >> TOUCH_PAD_SHIFT_DEFAULT;
        }
    }
    xTimerReset(s_touch_pad_filter->timer, portMAX_DELAY);
    xSemaphoreGive(rtc_touch_mux);
    if(s_filter_cb != NULL) {
        //return the raw data and filtered data.
        s_filter_cb(s_touch_pad_filter->raw_val, s_touch_pad_filter->filtered_val);
    }
}

esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_cycle)
{
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    portENTER_CRITICAL(&rtc_spinlock);
    //touch sensor sleep cycle Time = sleep_cycle / RTC_SLOW_CLK( can be 150k or 32k depending on the options)
    SENS.sar_touch_ctrl2.touch_sleep_cycles = sleep_cycle;
    //touch sensor measure time= meas_cycle / 8Mhz
    SENS.sar_touch_ctrl1.touch_meas_delay = meas_cycle;
    //the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD
    SENS.sar_touch_ctrl1.touch_xpd_wait = TOUCH_PAD_MEASURE_WAIT_DEFAULT;
    portEXIT_CRITICAL(&rtc_spinlock);
    xSemaphoreGive(rtc_touch_mux);
    return ESP_OK;
}

esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_cycle)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (sleep_cycle) {
        *sleep_cycle = SENS.sar_touch_ctrl2.touch_sleep_cycles;
    }
    if (meas_cycle) {
        *meas_cycle = SENS.sar_touch_ctrl1.touch_meas_delay;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_set_voltage(touch_high_volt_t refh, touch_low_volt_t refl, touch_volt_atten_t atten)
{
    RTC_MODULE_CHECK(((refh < TOUCH_HVOLT_MAX) && (refh >= (int )TOUCH_HVOLT_KEEP)), "touch refh error",
            ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(((refl < TOUCH_LVOLT_MAX) && (refh >= (int )TOUCH_LVOLT_KEEP)), "touch refl error",
            ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(((atten < TOUCH_HVOLT_ATTEN_MAX) && (refh >= (int )TOUCH_HVOLT_ATTEN_KEEP)), "touch atten error",
            ESP_ERR_INVALID_ARG);
#if CONFIG_IDF_TARGET_ESP32
    portENTER_CRITICAL(&rtc_spinlock);
    if (refh > TOUCH_HVOLT_KEEP) {
        RTCIO.touch_cfg.drefh = refh;
    }
    if (refl > TOUCH_LVOLT_KEEP) {
        RTCIO.touch_cfg.drefl = refl;
    }
    if (atten > TOUCH_HVOLT_ATTEN_KEEP) {
        RTCIO.touch_cfg.drange = atten;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
#endif
    return ESP_OK;
}

esp_err_t touch_pad_get_voltage(touch_high_volt_t *refh, touch_low_volt_t *refl, touch_volt_atten_t *atten)
{
#if CONFIG_IDF_TARGET_ESP32
    portENTER_CRITICAL(&rtc_spinlock);
    if (refh) {
        *refh = RTCIO.touch_cfg.drefh;
    }
    if (refl) {
        *refl = RTCIO.touch_cfg.drefl;
    }
    if (atten) {
        *atten = RTCIO.touch_cfg.drange;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
#endif
    return ESP_OK;
}

esp_err_t touch_pad_set_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t slope, touch_tie_opt_t opt)
{
    RTC_MODULE_CHECK((slope < TOUCH_PAD_SLOPE_MAX), "touch slope error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((opt < TOUCH_PAD_TIE_OPT_MAX), "touch opt error", ESP_ERR_INVALID_ARG);

    touch_pad_t touch_pad_wrap = touch_pad_num_wrap(touch_num);
    portENTER_CRITICAL(&rtc_spinlock);
    RTCIO.touch_pad[touch_pad_wrap].tie_opt = opt;
    RTCIO.touch_pad[touch_num].dac = slope;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t *slope, touch_tie_opt_t *opt)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);

    touch_pad_t touch_pad_wrap = touch_pad_num_wrap(touch_num);
    portENTER_CRITICAL(&rtc_spinlock);
    if(opt) {
        *opt = RTCIO.touch_pad[touch_pad_wrap].tie_opt;
    }
    if(slope) {
        *slope = RTCIO.touch_pad[touch_num].dac;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_io_init(touch_pad_t touch_num)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    gpio_num_t gpio_num = GPIO_NUM_0;
    touch_pad_get_io_num(touch_num, &gpio_num);
    rtc_gpio_init(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_pulldown_dis(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
    return ESP_OK;
}

esp_err_t touch_pad_set_fsm_mode(touch_fsm_mode_t mode)
{
    RTC_MODULE_CHECK((mode < TOUCH_FSM_MODE_MAX), "touch fsm mode error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl2.touch_start_en = 0;
    SENS.sar_touch_ctrl2.touch_start_force = mode;
    RTCCNTL.state0.touch_slp_timer_en = (mode == TOUCH_FSM_MODE_TIMER ? 1 : 0);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_fsm_mode(touch_fsm_mode_t *mode)
{
    if (mode) {
        *mode = SENS.sar_touch_ctrl2.touch_start_force;
    }
    return ESP_OK;
}

esp_err_t touch_pad_sw_start(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl2.touch_start_en = 0;
    SENS.sar_touch_ctrl2.touch_start_en = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint16_t threshold)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    touch_pad_t tp_wrap = touch_pad_num_wrap(touch_num);
    portENTER_CRITICAL(&rtc_spinlock);
    if (tp_wrap & 0x1) {
        SENS.touch_thresh[tp_wrap / 2].l_thresh = threshold;
    } else {
        SENS.touch_thresh[tp_wrap / 2].h_thresh = threshold;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint16_t *threshold)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    touch_pad_t tp_wrap = touch_pad_num_wrap(touch_num);
    if (threshold) {
        *threshold = (tp_wrap & 0x1 )? \
                SENS.touch_thresh[tp_wrap / 2].l_thresh : \
                SENS.touch_thresh[tp_wrap / 2].h_thresh;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_trigger_mode(touch_trigger_mode_t mode)
{
    RTC_MODULE_CHECK((mode < TOUCH_TRIGGER_MAX), "touch trigger mode error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl1.touch_out_sel = mode;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_trigger_mode(touch_trigger_mode_t *mode)
{
    if (mode) {
        *mode = SENS.sar_touch_ctrl1.touch_out_sel;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_trigger_source(touch_trigger_src_t src)
{
    RTC_MODULE_CHECK((src < TOUCH_TRIGGER_SOURCE_MAX), "touch trigger source error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl1.touch_out_1en = src;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_trigger_source(touch_trigger_src_t *src)
{
    if (src) {
        *src = SENS.sar_touch_ctrl1.touch_out_1en;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask)
{
    RTC_MODULE_CHECK((set1_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set1 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((set2_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set2 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((en_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch work_en bitmask error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_enable.touch_pad_outen1 |= TOUCH_BITS_SWAP(set1_mask);
    SENS.sar_touch_enable.touch_pad_outen2 |= TOUCH_BITS_SWAP(set2_mask);
    SENS.sar_touch_enable.touch_pad_worken |= TOUCH_BITS_SWAP(en_mask);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t touch_pad_get_group_mask(uint16_t *set1_mask, uint16_t *set2_mask, uint16_t *en_mask)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (set1_mask) {
        *set1_mask = TOUCH_BITS_SWAP(SENS.sar_touch_enable.touch_pad_outen1);
    }
    if (set2_mask) {
        *set2_mask = TOUCH_BITS_SWAP(SENS.sar_touch_enable.touch_pad_outen2);
    }
    if (en_mask) {
        *en_mask = TOUCH_BITS_SWAP(SENS.sar_touch_enable.touch_pad_worken);
    }
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t touch_pad_clear_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask)
{
    RTC_MODULE_CHECK((set1_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set1 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((set2_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set2 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((en_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch work_en bitmask error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_enable.touch_pad_outen1 &= TOUCH_BITS_SWAP(~set1_mask);
    SENS.sar_touch_enable.touch_pad_outen2 &= TOUCH_BITS_SWAP(~set2_mask);
    SENS.sar_touch_enable.touch_pad_worken &= TOUCH_BITS_SWAP(~en_mask);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

uint32_t IRAM_ATTR touch_pad_get_status(void)
{
    uint32_t status = SENS.sar_touch_ctrl2.touch_meas_en;
    return TOUCH_BITS_SWAP(status);
}

esp_err_t IRAM_ATTR touch_pad_clear_status(void)
{
    SENS.sar_touch_ctrl2.touch_meas_en_clr = 1;
    return ESP_OK;
}

esp_err_t touch_pad_intr_enable(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.int_ena.rtc_touch = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_intr_disable(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.int_ena.rtc_touch = 0;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_config(touch_pad_t touch_num, uint16_t threshold)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    touch_fsm_mode_t mode;
    touch_pad_set_thresh(touch_num, threshold);
    touch_pad_io_init(touch_num);
    touch_pad_set_cnt_mode(touch_num, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_LOW);
    touch_pad_get_fsm_mode(&mode);
    if (TOUCH_FSM_MODE_SW == mode) {
        touch_pad_clear_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
        s_touch_pad_init_bit |= (1 << touch_num);
    } else if (TOUCH_FSM_MODE_TIMER == mode){
        uint16_t sleep_time = 0;
        uint16_t meas_cycle = 0;
        uint32_t wait_time_ms = 0;
        uint32_t wait_tick = 0;
        uint32_t rtc_clk = rtc_clk_slow_freq_get_hz();
        touch_pad_set_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
        touch_pad_get_meas_time(&sleep_time, &meas_cycle);
        //If the FSM mode is 'TOUCH_FSM_MODE_TIMER', The data will be ready after one measurement cycle
        //after this function is executed, otherwise, the "touch_value" by "touch_pad_read" is 0.
        wait_time_ms = sleep_time/(rtc_clk/1000) + meas_cycle/(RTC_FAST_CLK_FREQ_APPROX/1000);
        wait_tick = wait_time_ms/portTICK_RATE_MS;
        vTaskDelay(wait_tick ? wait_tick : 1);
        s_touch_pad_init_bit |= (1 << touch_num);
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t touch_pad_init(void)
{
    if (rtc_touch_mux == NULL) {
        rtc_touch_mux = xSemaphoreCreateMutex();
    }
    if (rtc_touch_mux == NULL) {
        return ESP_FAIL;
    }
    touch_pad_intr_disable();
    touch_pad_clear_group_mask(TOUCH_PAD_BIT_MASK_MAX, TOUCH_PAD_BIT_MASK_MAX, TOUCH_PAD_BIT_MASK_MAX);
    touch_pad_set_trigger_mode(TOUCH_TRIGGER_MODE_DEFAULT);
    touch_pad_set_trigger_source(TOUCH_TRIGGER_SOURCE_DEFAULT);
    touch_pad_clear_status();
    touch_pad_set_meas_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT, TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_DEFAULT);
    return ESP_OK;
}

esp_err_t touch_pad_deinit(void)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    if (s_touch_pad_filter != NULL) {
        touch_pad_filter_stop();
        touch_pad_filter_delete();
    }
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    s_touch_pad_init_bit = 0x0000;
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW);
    touch_pad_clear_status();
    touch_pad_intr_disable();
    xSemaphoreGive(rtc_touch_mux);
    vSemaphoreDelete(rtc_touch_mux);
    rtc_touch_mux = NULL;
    return ESP_OK;
}

static esp_err_t _touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value, touch_fsm_mode_t mode)
{
    esp_err_t res = ESP_OK;
    touch_pad_t tp_wrap = touch_pad_num_wrap(touch_num);
    if (TOUCH_FSM_MODE_SW == mode) {
        touch_pad_set_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
        touch_pad_sw_start();
        while (SENS.sar_touch_ctrl2.touch_meas_done == 0) {};
        *touch_value = (tp_wrap & 0x1) ? \
                            SENS.touch_meas[tp_wrap / 2].l_val: \
                            SENS.touch_meas[tp_wrap / 2].h_val;

        touch_pad_clear_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
    } else if (TOUCH_FSM_MODE_TIMER == mode) {
        while (SENS.sar_touch_ctrl2.touch_meas_done == 0) {};
        *touch_value = (tp_wrap & 0x1) ? \
                            SENS.touch_meas[tp_wrap / 2].l_val: \
                            SENS.touch_meas[tp_wrap / 2].h_val;
    } else {
        res = ESP_FAIL;
    }
    if (*touch_value == 0) {
        res = ESP_ERR_INVALID_STATE;
    }
    return res;
}

esp_err_t touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value)
{
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);

    esp_err_t res = ESP_OK;
    touch_fsm_mode_t mode;
    touch_pad_get_fsm_mode(&mode);
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    res = _touch_pad_read(touch_num, touch_value, mode);
    xSemaphoreGive(rtc_touch_mux);
    return res;
}

IRAM_ATTR esp_err_t touch_pad_read_raw_data(touch_pad_t touch_num, uint16_t *touch_value)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_FAIL);
    *touch_value = s_touch_pad_filter->raw_val[touch_num];
    if (*touch_value == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

IRAM_ATTR esp_err_t touch_pad_read_filtered(touch_pad_t touch_num, uint16_t *touch_value)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_FAIL);
    *touch_value = (s_touch_pad_filter->filtered_val[touch_num]);
    if (*touch_value == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_filter_period(uint32_t new_period_ms)
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    RTC_MODULE_CHECK(new_period_ms > 0, "Touch pad filter period error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        xTimerChangePeriod(s_touch_pad_filter->timer, new_period_ms / portTICK_PERIOD_MS, portMAX_DELAY);
        s_touch_pad_filter->period = new_period_ms;
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_get_filter_period(uint32_t* p_period_ms)
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    RTC_MODULE_CHECK(p_period_ms != NULL, "Touch pad period pointer error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        *p_period_ms = s_touch_pad_filter->period;
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_start(uint32_t filter_period_ms)
{
    RTC_MODULE_CHECK(filter_period_ms >= portTICK_PERIOD_MS, "Touch pad filter period error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter == NULL) {
        s_touch_pad_filter = (touch_pad_filter_t *) calloc(1, sizeof(touch_pad_filter_t));
        if (s_touch_pad_filter == NULL) {
            goto err_no_mem;
        }
    }
    if (s_touch_pad_filter->timer == NULL) {
        s_touch_pad_filter->timer = xTimerCreate("filter_tmr", filter_period_ms / portTICK_PERIOD_MS, pdFALSE,
        NULL, touch_pad_filter_cb);
        if (s_touch_pad_filter->timer == NULL) {
            free(s_touch_pad_filter);
            s_touch_pad_filter = NULL;
            goto err_no_mem;
        }
        s_touch_pad_filter->period = filter_period_ms;
    }
    xSemaphoreGive(rtc_touch_mux);
    touch_pad_filter_cb(NULL);
    return ESP_OK;

err_no_mem:
    xSemaphoreGive(rtc_touch_mux);
    return ESP_ERR_NO_MEM;
}

esp_err_t touch_pad_filter_stop(void)
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);
    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        xTimerStop(s_touch_pad_filter->timer, portMAX_DELAY);
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_delete(void)
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        if (s_touch_pad_filter->timer != NULL) {
            xTimerStop(s_touch_pad_filter->timer, portMAX_DELAY);
            xTimerDelete(s_touch_pad_filter->timer, portMAX_DELAY);
            s_touch_pad_filter->timer = NULL;
        }
        free(s_touch_pad_filter);
        s_touch_pad_filter = NULL;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ESP_OK;
}

esp_err_t touch_pad_get_wakeup_status(touch_pad_t *pad_num)
{
    uint32_t touch_mask = SENS.sar_touch_ctrl2.touch_meas_en;
    if(touch_mask == 0) {
        return ESP_FAIL;
    }
    *pad_num = touch_pad_num_wrap((touch_pad_t)(__builtin_ffs(touch_mask) - 1));
    return ESP_OK;
}
#endif
/*---------------------------------------------------------------
                    ADC Common
---------------------------------------------------------------*/
#if CONFIG_IDF_TARGET_ESP32S2BETA
#define SENS_FORCE_XPD_AMP_FSM 0 // Use FSM to control power down
#define SENS_FORCE_XPD_AMP_PD  2 // Force power down
#define SENS_FORCE_XPD_AMP_PU  3 // Force power up

#define SENS_SAR1_ATTEN_VAL_MASK   0x3
#define SENS_SAR2_ATTEN_VAL_MASK   0x3

#define SENS_FORCE_XPD_SAR_SW_M (BIT(1))
#define SENS_FORCE_XPD_SAR_FSM 0 // Use FSM to control power down
#define SENS_FORCE_XPD_SAR_PD  2 // Force power down
#define SENS_FORCE_XPD_SAR_PU  3 // Force power up
#endif

static esp_err_t adc_set_fsm_time(int rst_wait, int start_wait, int standby_wait, int sample_cycle)
{
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    // Internal FSM reset wait time
    if (rst_wait >= 0) {
        SYSCON.saradc_fsm.rstb_wait = rst_wait;
    }
    // Internal FSM start wait time
    if (start_wait >= 0) {
        SYSCON.saradc_fsm.start_wait = start_wait;
    }
    // Internal FSM standby wait time
    if (standby_wait >= 0) {
        SYSCON.saradc_fsm.standby_wait = standby_wait;
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    // Internal FSM reset wait time
    if (rst_wait >= 0) {
        SYSCON.saradc_fsm_wait.rstb_wait = rst_wait;
    }
    // Internal FSM start wait time
    if (start_wait >= 0) {
        SYSCON.saradc_fsm_wait.xpd_wait = start_wait;
    }
    // Internal FSM standby wait time
    if (standby_wait >= 0) {
        SYSCON.saradc_fsm_wait.standby_wait = standby_wait;
    }
#endif
    // Internal FSM standby sample cycle
    if (sample_cycle >= 0) {
        SYSCON.saradc_fsm.sample_cycle = sample_cycle;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

static esp_err_t adc_set_data_format(adc_i2s_encode_t mode)
{
    portENTER_CRITICAL(&rtc_spinlock);
    //data format:
    //0: ADC_ENCODE_12BIT  [15:12]-channel [11:0]-12 bits ADC data
    //1: ADC_ENCODE_11BIT  [15]-1 [14:11]-channel [10:0]-11 bits ADC data, the resolution should not be larger than 11 bits in this case.
    SYSCON.saradc_ctrl.data_sar_sel = mode;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

static esp_err_t adc_set_measure_limit(uint8_t meas_num, bool lim_en)
{
    portENTER_CRITICAL(&rtc_spinlock);
    // Set max measure number
    SYSCON.saradc_ctrl2.max_meas_num = meas_num;
    // Enable max measure number limit
    SYSCON.saradc_ctrl2.meas_num_limit = lim_en;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

static esp_err_t adc_set_work_mode(adc_unit_t adc_unit)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (adc_unit == ADC_UNIT_1) {
        // saradc mode sel : 0--single saradc;  1--double saradc;  2--alternative saradc
        SYSCON.saradc_ctrl.work_mode = 0;
        //ENABLE ADC  0: ADC1  1: ADC2, only work for single SAR mode
        SYSCON.saradc_ctrl.sar_sel = 0;
    } else if (adc_unit == ADC_UNIT_2) {
        // saradc mode sel : 0--single saradc;  1--double saradc;  2--alternative saradc
        SYSCON.saradc_ctrl.work_mode = 0;
        //ENABLE ADC1  0: SAR1  1: SAR2  only work for single SAR mode
        SYSCON.saradc_ctrl.sar_sel = 1;
    } else if (adc_unit == ADC_UNIT_BOTH) {
        // saradc mode sel : 0--single saradc;  1--double saradc;  2--alternative saradc
        SYSCON.saradc_ctrl.work_mode = 1;
    } else if (adc_unit == ADC_UNIT_ALTER) {
        // saradc mode sel : 0--single saradc;  1--double saradc;  2--alternative saradc
        SYSCON.saradc_ctrl.work_mode = 2;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

static esp_err_t adc_set_atten(adc_unit_t adc_unit, adc_channel_t channel, adc_atten_t atten)
{
    ADC_CHECK_UNIT(adc_unit);
    if (adc_unit & ADC_UNIT_1) {
        RTC_MODULE_CHECK((adc1_channel_t)channel < ADC1_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);
    }
    RTC_MODULE_CHECK(atten < ADC_ATTEN_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    if (adc_unit & ADC_UNIT_1) {
        //SAR1_atten
        SET_PERI_REG_BITS(SENS_SAR_ATTEN1_REG, SENS_SAR1_ATTEN_VAL_MASK, atten, (channel * 2));
    }
    if (adc_unit & ADC_UNIT_2) {
        //SAR2_atten
        SET_PERI_REG_BITS(SENS_SAR_ATTEN2_REG, SENS_SAR2_ATTEN_VAL_MASK, atten, (channel * 2));
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

void adc_power_always_on(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
#endif
    portEXIT_CRITICAL(&rtc_spinlock);
}

void adc_power_on(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    //The power FSM controlled mode saves more power, while the ADC noise may get increased.
#ifndef CONFIG_ADC_FORCE_XPD_FSM
    //Set the power always on to increase precision.
    SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
#else
    //Use the FSM to turn off the power while not used to save power.
    if (SENS.sar_meas_wait2.force_xpd_sar & SENS_FORCE_XPD_SAR_SW_M) {
        SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
    } else {
        SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_FSM;
    }
#endif
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    //The power FSM controlled mode saves more power, while the ADC noise may get increased.
#ifndef CONFIG_ADC_FORCE_XPD_FSM
    //Set the power always on to increase precision.
    SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
#else    
    //Use the FSM to turn off the power while not used to save power.
    if (SENS.sar_power_xpd_sar.force_xpd_sar & SENS_FORCE_XPD_SAR_SW_M) {
        SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
    } else {
        SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_FSM;
    }
#endif
#endif
    portEXIT_CRITICAL(&rtc_spinlock);
}

void adc_power_off(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    //Bit1  0:Fsm  1: SW mode
    //Bit0  0:SW mode power down  1: SW mode power on
    SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PD;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_PD;
#endif

    portEXIT_CRITICAL(&rtc_spinlock);
}

esp_err_t adc_set_clk_div(uint8_t clk_div)
{
    portENTER_CRITICAL(&rtc_spinlock);
    // ADC clock devided from APB clk, 80 / 2 = 40Mhz,
    SYSCON.saradc_ctrl.sar_clk_div = clk_div;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t adc_set_i2s_data_source(adc_i2s_source_t src)
{
    RTC_MODULE_CHECK(src < ADC_I2S_DATA_SRC_MAX, "ADC i2s data source error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    // 1: I2S input data is from SAR ADC (for DMA)  0: I2S input data is from GPIO matrix
    SYSCON.saradc_ctrl.data_to_i2s = src;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t adc_gpio_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    ADC_CHECK_UNIT(adc_unit);
    gpio_num_t gpio_num = 0;
    if (adc_unit & ADC_UNIT_1) {
        RTC_MODULE_CHECK((adc1_channel_t) channel < ADC1_CHANNEL_MAX, "ADC1 channel error", ESP_ERR_INVALID_ARG);
        ADC1_CHECK_FUNCTION_RET(adc1_pad_get_io_num((adc1_channel_t) channel, &gpio_num));
        ADC1_CHECK_FUNCTION_RET(rtc_gpio_init(gpio_num));
        ADC1_CHECK_FUNCTION_RET(rtc_gpio_output_disable(gpio_num));
        ADC1_CHECK_FUNCTION_RET(rtc_gpio_input_disable(gpio_num));
        ADC1_CHECK_FUNCTION_RET(gpio_set_pull_mode(gpio_num, GPIO_FLOATING));
    }
    return ESP_OK;
}

esp_err_t adc_set_data_inv(adc_unit_t adc_unit, bool inv_en)
{
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    if (adc_unit & ADC_UNIT_1) {
        // Enable ADC data invert
        SENS.sar_read_ctrl.sar1_data_inv = inv_en;
    }
    if (adc_unit & ADC_UNIT_2) {
        // Enable ADC data invert
        SENS.sar_read_ctrl2.sar2_data_inv = inv_en;
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    if (adc_unit & ADC_UNIT_1) {
        // Enable ADC data invert
        SENS.sar_reader1_ctrl.sar1_data_inv = inv_en;
    }
    if (adc_unit & ADC_UNIT_2) {
        // Enable ADC data invert
        SENS.sar_reader2_ctrl.sar2_data_inv = inv_en;
    }
#endif
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t adc_set_data_width(adc_unit_t adc_unit, adc_bits_width_t bits)
{
    ADC_CHECK_UNIT(adc_unit);
    RTC_MODULE_CHECK(bits < ADC_WIDTH_MAX, "ADC bit width error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    if (adc_unit & ADC_UNIT_1) {
        SENS.sar_start_force.sar1_bit_width = bits;
        SENS.sar_read_ctrl.sar1_sample_bit = bits;
    }
    if (adc_unit & ADC_UNIT_2) {
        SENS.sar_start_force.sar2_bit_width = bits;
        SENS.sar_read_ctrl2.sar2_sample_bit = bits;
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    if (adc_unit & ADC_UNIT_1) {
        SENS.sar_meas1_ctrl1.sar1_bit_width = bits;
        SENS.sar_reader1_ctrl.sar1_sample_bit = bits;
    }
    if (adc_unit & ADC_UNIT_2) {
        SENS.sar_meas2_ctrl1.sar2_bit_width = bits;
        SENS.sar_reader2_ctrl.sar2_sample_bit = bits;
    }
#endif
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

// this function should be called in the critical section
static void adc_set_controller(adc_unit_t unit, adc_controller_t ctrl )
{
#if CONFIG_IDF_TARGET_ESP32
    if ( unit == ADC_UNIT_1 ) {
        switch( ctrl ) {
            case ADC_CTRL_RTC:
                SENS.sar_read_ctrl.sar1_dig_force = false;      //RTC controller controls the ADC, not digital controller
                SENS.sar_meas_start1.meas1_start_force = true;  //RTC controller controls the ADC,not ulp coprocessor
                SENS.sar_meas_start1.sar1_en_pad_force = true;  //RTC controller controls the data port, not ulp coprocessor
                SENS.sar_touch_ctrl1.xpd_hall_force = true;     // RTC controller controls the hall sensor power,not ulp coprocessor
                SENS.sar_touch_ctrl1.hall_phase_force = true;   // RTC controller controls the hall sensor phase,not ulp coprocessor
                break;
            case ADC_CTRL_ULP:
                SENS.sar_read_ctrl.sar1_dig_force = false;
                SENS.sar_meas_start1.meas1_start_force = false;
                SENS.sar_meas_start1.sar1_en_pad_force = false;
                SENS.sar_touch_ctrl1.xpd_hall_force = false;
                SENS.sar_touch_ctrl1.hall_phase_force = false;
                break;
            case ADC_CTRL_DIG:
                SENS.sar_read_ctrl.sar1_dig_force = true;
                SENS.sar_meas_start1.meas1_start_force = true;
                SENS.sar_meas_start1.sar1_en_pad_force = true;
                SENS.sar_touch_ctrl1.xpd_hall_force = true;
                SENS.sar_touch_ctrl1.hall_phase_force = true;
                break;
            default:
                ESP_LOGE(TAG, "adc1 selects invalid controller");
                break;
        }
    } else if ( unit == ADC_UNIT_2) {
        switch( ctrl ) {
            case ADC_CTRL_RTC:
                SENS.sar_meas_start2.meas2_start_force = true;  //RTC controller controls the ADC,not ulp coprocessor
                SENS.sar_meas_start2.sar2_en_pad_force = true;  //RTC controller controls the data port, not ulp coprocessor
                SENS.sar_read_ctrl2.sar2_dig_force = false;     //RTC controller controls the ADC, not digital controller
                SENS.sar_read_ctrl2.sar2_pwdet_force = false;   //RTC controller controls the ADC, not PWDET
                SYSCON.saradc_ctrl.sar2_mux = true;             //RTC controller controls the ADC, not PWDET
                break;
            case ADC_CTRL_ULP:
                SENS.sar_meas_start2.meas2_start_force = false;
                SENS.sar_meas_start2.sar2_en_pad_force = false;
                SENS.sar_read_ctrl2.sar2_dig_force = false;
                SENS.sar_read_ctrl2.sar2_pwdet_force = false;
                SYSCON.saradc_ctrl.sar2_mux = true;
                break;
            case ADC_CTRL_DIG:
                SENS.sar_meas_start2.meas2_start_force = true;
                SENS.sar_meas_start2.sar2_en_pad_force = true;
                SENS.sar_read_ctrl2.sar2_dig_force = true;
                SENS.sar_read_ctrl2.sar2_pwdet_force = false;
                SYSCON.saradc_ctrl.sar2_mux = true;
                break;
            case ADC2_CTRL_PWDET:
                //currently only used by Wi-Fi
                SENS.sar_meas_start2.meas2_start_force = true;
                SENS.sar_meas_start2.sar2_en_pad_force = true;
                SENS.sar_read_ctrl2.sar2_dig_force = false;
                SENS.sar_read_ctrl2.sar2_pwdet_force = true;
                SYSCON.saradc_ctrl.sar2_mux = false;
                break;
            default:
                ESP_LOGE(TAG, "adc2 selects invalid controller");
                break;            
        }
    } else {
      ESP_LOGE(TAG, "invalid adc unit");
      assert(0);
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    if ( unit == ADC_UNIT_1 ) {
        switch( ctrl ) {
            case ADC_CTRL_RTC:
                SENS.sar_meas1_mux.sar1_dig_force = false;      //RTC controller controls the ADC, not digital controller
                SENS.sar_meas1_ctrl2.meas1_start_force = true;  //RTC controller controls the ADC,not ulp coprocessor
                SENS.sar_meas1_ctrl2.sar1_en_pad_force = true;  //RTC controller controls the data port, not ulp coprocessor
                SENS.sar_hall_ctrl.xpd_hall_force = true;     // RTC controller controls the hall sensor power,not ulp coprocessor
                SENS.sar_hall_ctrl.hall_phase_force = true;   // RTC controller controls the hall sensor phase,not ulp coprocessor
                break;
            case ADC_CTRL_ULP:
                SENS.sar_meas1_mux.sar1_dig_force = false;
                SENS.sar_meas1_ctrl2.meas1_start_force = false;
                SENS.sar_meas1_ctrl2.sar1_en_pad_force = false;
                SENS.sar_hall_ctrl.xpd_hall_force = false;
                SENS.sar_hall_ctrl.hall_phase_force = false;
                break;
            case ADC_CTRL_DIG:
                SENS.sar_meas1_mux.sar1_dig_force = true;
                SENS.sar_meas1_ctrl2.meas1_start_force = true;
                SENS.sar_meas1_ctrl2.sar1_en_pad_force = true;
                SENS.sar_hall_ctrl.xpd_hall_force = true;
                SENS.sar_hall_ctrl.hall_phase_force = true;
                break;
            default:
                ESP_LOGE(TAG, "adc1 selects invalid controller");
                break;            
        }
    } else if ( unit == ADC_UNIT_2) {
        switch( ctrl ) {
            case ADC_CTRL_RTC:
                SENS.sar_meas2_ctrl2.meas2_start_force = true;  //RTC controller controls the ADC,not ulp coprocessor 
                SENS.sar_meas2_ctrl2.sar2_en_pad_force = true;  //RTC controller controls the data port, not ulp coprocessor
                break;
            case ADC_CTRL_ULP:
                SENS.sar_meas2_ctrl2.meas2_start_force = false;
                SENS.sar_meas2_ctrl2.sar2_en_pad_force = false;
                break;
            case ADC_CTRL_DIG:
                SENS.sar_meas2_ctrl2.meas2_start_force = true;
                SENS.sar_meas2_ctrl2.sar2_en_pad_force = true;
                break;
            case ADC2_CTRL_PWDET:
                //currently only used by Wi-Fi
                SENS.sar_meas2_ctrl2.meas2_start_force = true;
                SENS.sar_meas2_ctrl2.sar2_en_pad_force = true;
                break;
            default:
                ESP_LOGE(TAG, "adc2 selects invalid controller");
                break;            
        }
    } else {
      ESP_LOGE(TAG, "invalid adc unit");
      assert(0);
    }
#endif
}

// this function should be called in the critical section
static int adc_convert( adc_unit_t unit, int channel)
{
    uint16_t adc_value = 0;
#if CONFIG_IDF_TARGET_ESP32
    if ( unit == ADC_UNIT_1 ) {
        SENS.sar_meas_start1.sar1_en_pad = (1 << channel); //only one channel is selected.
        while (SENS.sar_slave_addr1.meas_status != 0);
        SENS.sar_meas_start1.meas1_start_sar = 0;
        SENS.sar_meas_start1.meas1_start_sar = 1;
        while (SENS.sar_meas_start1.meas1_done_sar == 0);
        adc_value = SENS.sar_meas_start1.meas1_data_sar;
    } else if ( unit == ADC_UNIT_2 ) {
        SENS.sar_meas_start2.sar2_en_pad = (1 << channel); //only one channel is selected.

        SENS.sar_meas_start2.meas2_start_sar = 0; //start force 0
        SENS.sar_meas_start2.meas2_start_sar = 1; //start force 1
        while (SENS.sar_meas_start2.meas2_done_sar == 0) {}; //read done
        adc_value = SENS.sar_meas_start2.meas2_data_sar;
    } else {
        ESP_LOGE(TAG, "invalid adc unit");
        return ESP_ERR_INVALID_ARG;
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
     if ( unit == ADC_UNIT_1 ) {
        SENS.sar_meas1_ctrl2.sar1_en_pad = (1 << channel); //only one channel is selected.
        while (SENS.sar_slave_addr1.meas_status != 0);
        SENS.sar_meas1_ctrl2.meas1_start_sar = 0;
        SENS.sar_meas1_ctrl2.meas1_start_sar = 1;
        while (SENS.sar_meas1_ctrl2.meas1_done_sar == 0);
        adc_value = SENS.sar_meas1_ctrl2.meas1_data_sar;
    } else if ( unit == ADC_UNIT_2 ) {
        SENS.sar_meas2_ctrl2.sar2_en_pad = (1 << channel); //only one channel is selected.    
        
        SENS.sar_meas2_ctrl2.meas2_start_sar = 0; //start force 0
        SENS.sar_meas2_ctrl2.meas2_start_sar = 1; //start force 1
        while (SENS.sar_meas2_ctrl2.meas2_done_sar == 0) {}; //read done
        adc_value = SENS.sar_meas2_ctrl2.meas2_data_sar;    
    } else {
        ESP_LOGE(TAG, "invalid adc unit");
        return ESP_ERR_INVALID_ARG;
    }
#endif
    return adc_value;
}

/*-------------------------------------------------------------------------------------
 *                      ADC I2S
 *------------------------------------------------------------------------------------*/
static esp_err_t adc_set_i2s_data_len(adc_unit_t adc_unit, int patt_len)
{
    ADC_CHECK_UNIT(adc_unit);
    RTC_MODULE_CHECK((patt_len < ADC_PATT_LEN_MAX) && (patt_len > 0), "ADC pattern length error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    if(adc_unit & ADC_UNIT_1) {
        SYSCON.saradc_ctrl.sar1_patt_len = patt_len - 1;
    }
    if(adc_unit & ADC_UNIT_2) {
        SYSCON.saradc_ctrl.sar2_patt_len = patt_len - 1;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

static esp_err_t adc_set_i2s_data_pattern(adc_unit_t adc_unit, int seq_num, adc_channel_t channel, adc_bits_width_t bits, adc_atten_t atten)
{
    ADC_CHECK_UNIT(adc_unit);
    if (adc_unit & ADC_UNIT_1) {
        RTC_MODULE_CHECK((adc1_channel_t) channel < ADC1_CHANNEL_MAX, "ADC1 channel error", ESP_ERR_INVALID_ARG);
    }
    RTC_MODULE_CHECK(bits < ADC_WIDTH_MAX, "ADC bit width error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(atten < ADC_ATTEN_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    //Configure pattern table, each 8 bit defines one channel
    //[7:4]-channel [3:2]-bit width [1:0]- attenuation
    //BIT WIDTH: 3: 12BIT  2: 11BIT  1: 10BIT  0: 9BIT
    //ATTEN: 3: ATTEN = 11dB 2: 6dB 1: 2.5dB 0: 0dB
    uint8_t val = (channel << 4) | (bits << 2) | (atten << 0);
    if (adc_unit & ADC_UNIT_1) {
        SYSCON.saradc_sar1_patt_tab[seq_num / 4] &= (~(0xff << ((3 - (seq_num % 4)) * 8)));
        SYSCON.saradc_sar1_patt_tab[seq_num / 4] |= (val << ((3 - (seq_num % 4)) * 8));
    }
    if (adc_unit & ADC_UNIT_2) {
        SYSCON.saradc_sar2_patt_tab[seq_num / 4] &= (~(0xff << ((3 - (seq_num % 4)) * 8)));
        SYSCON.saradc_sar2_patt_tab[seq_num / 4] |= (val << ((3 - (seq_num % 4)) * 8));
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t adc_i2s_mode_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    ADC_CHECK_UNIT(adc_unit);
    if (adc_unit & ADC_UNIT_1) {
        RTC_MODULE_CHECK((adc1_channel_t) channel < ADC1_CHANNEL_MAX, "ADC1 channel error", ESP_ERR_INVALID_ARG);
    }

    uint8_t table_len = 1;
    //POWER ON SAR
    adc_power_always_on();
    adc_gpio_init(adc_unit, channel);
    adc_set_i2s_data_len(adc_unit, table_len);
    adc_set_i2s_data_pattern(adc_unit, 0, channel, ADC_WIDTH_BIT_12, ADC_ATTEN_DB_11);
    portENTER_CRITICAL(&rtc_spinlock);
    if (adc_unit & ADC_UNIT_1) {
        adc_set_controller( ADC_UNIT_1, ADC_CTRL_DIG );
    }
    if (adc_unit & ADC_UNIT_2) {
        adc_set_controller( ADC_UNIT_2, ADC_CTRL_DIG );
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    adc_set_i2s_data_source(ADC_I2S_DATA_SRC_ADC);
    adc_set_clk_div(SAR_ADC_CLK_DIV_DEFUALT);
    // Set internal FSM wait time.
    adc_set_fsm_time(ADC_FSM_RSTB_WAIT_DEFAULT, ADC_FSM_START_WAIT_DEFAULT, ADC_FSM_STANDBY_WAIT_DEFAULT,
            ADC_FSM_TIME_KEEP);
    adc_set_work_mode(adc_unit);
    adc_set_data_format(ADC_ENCODE_12BIT);
    adc_set_measure_limit(ADC_MAX_MEAS_NUM_DEFAULT, ADC_MEAS_NUM_LIM_DEFAULT);
    //Invert The Level, Invert SAR ADC1 data
    adc_set_data_inv(adc_unit, true);
    return ESP_OK;
 }

/*-------------------------------------------------------------------------------------
 *                      ADC1
 *------------------------------------------------------------------------------------*/
esp_err_t adc1_pad_get_io_num(adc1_channel_t channel, gpio_num_t *gpio_num)
{
    RTC_MODULE_CHECK(channel < ADC1_CHANNEL_MAX, "ADC1 Channel Err", ESP_ERR_INVALID_ARG);

    switch (channel) {
    case ADC1_CHANNEL_0:
        *gpio_num = ADC1_CHANNEL_0_GPIO_NUM;
        break;
    case ADC1_CHANNEL_1:
        *gpio_num = ADC1_CHANNEL_1_GPIO_NUM;
        break;
    case ADC1_CHANNEL_2:
        *gpio_num = ADC1_CHANNEL_2_GPIO_NUM;
        break;
    case ADC1_CHANNEL_3:
        *gpio_num = ADC1_CHANNEL_3_GPIO_NUM;
        break;
    case ADC1_CHANNEL_4:
        *gpio_num = ADC1_CHANNEL_4_GPIO_NUM;
        break;
    case ADC1_CHANNEL_5:
        *gpio_num = ADC1_CHANNEL_5_GPIO_NUM;
        break;
    case ADC1_CHANNEL_6:
        *gpio_num = ADC1_CHANNEL_6_GPIO_NUM;
        break;
    case ADC1_CHANNEL_7:
        *gpio_num = ADC1_CHANNEL_7_GPIO_NUM;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)
{
    RTC_MODULE_CHECK(channel < ADC1_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(atten < ADC_ATTEN_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);
    adc_gpio_init(ADC_UNIT_1, channel);
    adc_set_atten(ADC_UNIT_1, channel, atten);
    return ESP_OK;
}

esp_err_t adc1_config_width(adc_bits_width_t width_bit)
{
    RTC_MODULE_CHECK(width_bit < ADC_WIDTH_MAX, "ADC bit width error", ESP_ERR_INVALID_ARG);
    adc_set_data_width(ADC_UNIT_1, width_bit);
    adc_set_data_inv(ADC_UNIT_1, true);
    return ESP_OK;
}

static inline void adc1_fsm_disable(void)
{
#if CONFIG_IDF_TARGET_ESP32
    //channel is set in the  convert function
    SENS.sar_meas_wait2.force_xpd_amp = SENS_FORCE_XPD_AMP_PD;
    //disable FSM, it's only used by the LNA.
    SENS.sar_meas_ctrl.amp_rst_fb_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_gnd_fsm = 0;
    SENS.sar_meas_wait1.sar_amp_wait1 = 1;
    SENS.sar_meas_wait1.sar_amp_wait2 = 1;
    SENS.sar_meas_wait2.sar_amp_wait3 = 1;    
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    //channel is set in the  convert function
    SENS.sar_meas1_ctrl1.force_xpd_amp = SENS_FORCE_XPD_AMP_PD;
    //disable FSM, it's only used by the LNA.
    SENS.sar_amp_ctrl3.amp_rst_fb_fsm = 0;
    SENS.sar_amp_ctrl3.amp_short_ref_fsm = 0;
    SENS.sar_amp_ctrl3.amp_short_ref_gnd_fsm = 0;
    SENS.sar_amp_ctrl1.sar_amp_wait1 = 1;
    SENS.sar_amp_ctrl1.sar_amp_wait2 = 1;
    SENS.sar_amp_ctrl2.sar_amp_wait3 = 1;
#endif
}

esp_err_t adc1_i2s_mode_acquire(void)
{
    //lazy initialization
    //for i2s, block until acquire the lock
    _lock_acquire( &adc1_i2s_lock );
    ESP_LOGD( RTC_MODULE_TAG, "i2s mode takes adc1 lock." );
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
    //switch SARADC into DIG channel
    SENS.sar_read_ctrl.sar1_dig_force = 1;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    SENS.sar_power_xpd_sar.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;
    //switch SARADC into DIG channel
    SENS.sar_meas1_mux.sar1_dig_force = 1;
#endif
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t adc1_adc_mode_acquire(void)
{
    //lazy initialization
    //for adc1, block until acquire the lock
    _lock_acquire( &adc1_i2s_lock );
    ESP_LOGD( RTC_MODULE_TAG, "adc mode takes adc1 lock." );
    portENTER_CRITICAL(&rtc_spinlock);
    // for now the WiFi would use ADC2 and set xpd_sar force on.
    // so we can not reset xpd_sar to fsm mode directly.
    // We should handle this after the synchronization mechanism is established.

    //switch SARADC into RTC channel
#if CONFIG_IDF_TARGET_ESP32
    SENS.sar_read_ctrl.sar1_dig_force = 0;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    SENS.sar_meas1_mux.sar1_dig_force = 0;
#endif
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t adc1_lock_release(void)
{
    RTC_MODULE_CHECK((uint32_t*)adc1_i2s_lock != NULL, "adc1 lock release called before acquire", ESP_ERR_INVALID_STATE );
    // for now the WiFi would use ADC2 and set xpd_sar force on.
    // so we can not reset xpd_sar to fsm mode directly.
    // We should handle this after the synchronization mechanism is established.

    _lock_release( &adc1_i2s_lock );
    return ESP_OK;
}

int adc1_get_raw(adc1_channel_t channel)
{
    uint16_t adc_value;
    RTC_MODULE_CHECK(channel < ADC1_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);
    adc1_adc_mode_acquire();
    adc_power_on();

    portENTER_CRITICAL(&rtc_spinlock);
    //disable other peripherals
    adc1_hall_enable(false);
    adc1_fsm_disable(); //currently the LNA is not open, close it by default
    //set controller
    adc_set_controller( ADC_UNIT_1, ADC_CTRL_RTC );
    //start conversion
    adc_value = adc_convert( ADC_UNIT_1, channel );
    portEXIT_CRITICAL(&rtc_spinlock);
    adc1_lock_release();
    return adc_value;
}

void adc1_ulp_enable(void)
{
    adc_power_on();

    portENTER_CRITICAL(&rtc_spinlock);
    adc_set_controller( ADC_UNIT_1, ADC_CTRL_ULP );
    // since most users do not need LNA and HALL with uLP, we disable them here
    // open them in the uLP if needed.
    adc1_fsm_disable();
    adc1_hall_enable(false);
    portEXIT_CRITICAL(&rtc_spinlock);
}

/*---------------------------------------------------------------
                    ADC2
---------------------------------------------------------------*/
esp_err_t adc2_pad_get_io_num(adc2_channel_t channel, gpio_num_t *gpio_num)
{
    RTC_MODULE_CHECK(channel < ADC2_CHANNEL_MAX, "ADC2 Channel Err", ESP_ERR_INVALID_ARG);

    switch (channel) {
    case ADC2_CHANNEL_0:
        *gpio_num = ADC2_CHANNEL_0_GPIO_NUM;
        break;
    case ADC2_CHANNEL_1:
        *gpio_num = ADC2_CHANNEL_1_GPIO_NUM;
        break;
    case ADC2_CHANNEL_2:
        *gpio_num = ADC2_CHANNEL_2_GPIO_NUM;
        break;
    case ADC2_CHANNEL_3:
        *gpio_num = ADC2_CHANNEL_3_GPIO_NUM;
        break;
    case ADC2_CHANNEL_4:
        *gpio_num = ADC2_CHANNEL_4_GPIO_NUM;
        break;
    case ADC2_CHANNEL_5:
        *gpio_num = ADC2_CHANNEL_5_GPIO_NUM;
        break;
    case ADC2_CHANNEL_6:
        *gpio_num = ADC2_CHANNEL_6_GPIO_NUM;
        break;
    case ADC2_CHANNEL_7:
        *gpio_num = ADC2_CHANNEL_7_GPIO_NUM;
        break;
    case ADC2_CHANNEL_8:
        *gpio_num = ADC2_CHANNEL_8_GPIO_NUM;
        break;
    case ADC2_CHANNEL_9:
        *gpio_num = ADC2_CHANNEL_9_GPIO_NUM;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t adc2_wifi_acquire(void)
{
    //lazy initialization
    //for wifi, block until acquire the lock
    _lock_acquire( &adc2_wifi_lock );
    ESP_LOGD( RTC_MODULE_TAG, "Wi-Fi takes adc2 lock." );
    return ESP_OK;
}

esp_err_t adc2_wifi_release(void)
{
    RTC_MODULE_CHECK((uint32_t*)adc2_wifi_lock != NULL, "wifi release called before acquire", ESP_ERR_INVALID_STATE );

    _lock_release( &adc2_wifi_lock );
    ESP_LOGD( RTC_MODULE_TAG, "Wi-Fi returns adc2 lock." );
    return ESP_OK;
}

static esp_err_t adc2_pad_init(adc2_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    ADC2_CHECK_FUNCTION_RET(adc2_pad_get_io_num(channel, &gpio_num));
    ADC2_CHECK_FUNCTION_RET(rtc_gpio_init(gpio_num));
    ADC2_CHECK_FUNCTION_RET(rtc_gpio_output_disable(gpio_num));
    ADC2_CHECK_FUNCTION_RET(rtc_gpio_input_disable(gpio_num));
    ADC2_CHECK_FUNCTION_RET(gpio_set_pull_mode(gpio_num, GPIO_FLOATING));
    return ESP_OK;
}

esp_err_t adc2_config_channel_atten(adc2_channel_t channel, adc_atten_t atten)
{
    RTC_MODULE_CHECK(channel < ADC2_CHANNEL_MAX, "ADC2 Channel Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(atten <= ADC_ATTEN_11db, "ADC2 Atten Err", ESP_ERR_INVALID_ARG);

    adc2_pad_init(channel);
    portENTER_CRITICAL( &adc2_spinlock );

    //lazy initialization
    //avoid collision with other tasks
    if ( _lock_try_acquire( &adc2_wifi_lock ) == -1 ) {
        //try the lock, return if failed (wifi using).
        portEXIT_CRITICAL( &adc2_spinlock );
        return ESP_ERR_TIMEOUT;
    }
    SENS.sar_atten2 = ( SENS.sar_atten2 & ~(3<<(channel*2)) ) | ((atten&3) << (channel*2));
    _lock_release( &adc2_wifi_lock );

    portEXIT_CRITICAL( &adc2_spinlock );
    return ESP_OK;
}

static inline void adc2_config_width(adc_bits_width_t width_bit)
{
    portENTER_CRITICAL(&rtc_spinlock);
#if CONFIG_IDF_TARGET_ESP32
    //sar_start_force shared with ADC1
    SENS.sar_start_force.sar2_bit_width = width_bit;
    //cct set to the same value with PHY
    SENS.sar_start_force.sar2_pwdet_cct = 4;
    portEXIT_CRITICAL(&rtc_spinlock);

    //Invert the adc value,the Output value is invert
    SENS.sar_read_ctrl2.sar2_data_inv = 1;
    //Set The adc sample width,invert adc value,must digital sar2_bit_width[1:0]=3
    SENS.sar_read_ctrl2.sar2_sample_bit = width_bit;
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    //sar_start_force shared with ADC1
    SENS.sar_meas2_ctrl1.sar2_bit_width = width_bit;
    //cct set to the same value with PHY
    SENS.sar_meas2_mux.sar2_pwdet_cct = 4;
    portEXIT_CRITICAL(&rtc_spinlock);
    //Invert the adc value,the Output value is invert
    SENS.sar_reader2_ctrl.sar2_data_inv = 1;
    //Set The adc sample width,invert adc value,must digital sar2_bit_width[1:0]=3
    SENS.sar_reader2_ctrl.sar2_sample_bit = width_bit;
#endif
}

static inline void adc2_dac_disable( adc2_channel_t channel)
{
#if CONFIG_IDF_TARGET_ESP32
    if ( channel == ADC2_CHANNEL_8 ) { // the same as DAC channel 1
        dac_output_set_enable( DAC_CHANNEL_1, false );
    } else if ( channel == ADC2_CHANNEL_9 ) {
        dac_output_set_enable( DAC_CHANNEL_2, false );
    }
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    if ( channel == ADC2_CHANNEL_6 ) { // the same as DAC channel 1
        dac_output_set_enable( DAC_CHANNEL_1, false );
    } else if ( channel == ADC2_CHANNEL_7 ) {
        dac_output_set_enable( DAC_CHANNEL_2, false );
    }
#endif
}

//registers in critical section with adc1:
//SENS_SAR_START_FORCE_REG,
esp_err_t adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width_bit, int* raw_out)
{
    uint16_t adc_value = 0;
    RTC_MODULE_CHECK(channel < ADC2_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);

    //in critical section with whole rtc module
    adc_power_on();

    //avoid collision with other tasks
    portENTER_CRITICAL(&adc2_spinlock);
    //lazy initialization
    //try the lock, return if failed (wifi using).
    if ( _lock_try_acquire( &adc2_wifi_lock ) == -1 ) {
        portEXIT_CRITICAL( &adc2_spinlock );
        return ESP_ERR_TIMEOUT;
    }

    //disable other peripherals
#ifdef CONFIG_ADC_DISABLE_DAC
    adc2_dac_disable( channel );
#endif
    // set controller
    // in critical section with whole rtc module
    // because the PWDET use the same registers, place it here.
    adc2_config_width( width_bit );
    adc_set_controller( ADC_UNIT_2, ADC_CTRL_RTC );
    //start converting
    adc_value = adc_convert( ADC_UNIT_2, channel );
    _lock_release( &adc2_wifi_lock );
    portEXIT_CRITICAL(&adc2_spinlock);

    *raw_out = (int)adc_value;
    return ESP_OK;
}

esp_err_t adc2_vref_to_gpio(gpio_num_t gpio)
{
#if CONFIG_IDF_TARGET_ESP32
    int channel;
    if(gpio == GPIO_NUM_25){
        channel = 8;    //Channel 8 bit
    }else if (gpio == GPIO_NUM_26){
        channel = 9;    //Channel 9 bit
    }else if (gpio == GPIO_NUM_27){
        channel = 7;    //Channel 7 bit
    }else{
        return ESP_ERR_INVALID_ARG;
    }

    //Configure RTC gpio
    rtc_gpio_init(gpio);
    rtc_gpio_output_disable(gpio);
    rtc_gpio_input_disable(gpio);
    rtc_gpio_pullup_dis(gpio);
    rtc_gpio_pulldown_dis(gpio);
    //force fsm
    adc_power_always_on();               //Select power source of ADC

    RTCCNTL.bias_conf.dbg_atten = 0;     //Check DBG effect outside sleep mode
    //set dtest (MUX_SEL : 0 -> RTC; 1-> vdd_sar2)
    RTCCNTL.test_mux.dtest_rtc = 1;      //Config test mux to route v_ref to ADC2 Channels
    //set ent
    RTCCNTL.test_mux.ent_rtc = 1;
    //set sar2_en_test
    SENS.sar_start_force.sar2_en_test = 1;
    //set sar2 en force
    SENS.sar_meas_start2.sar2_en_pad_force = 1;      //Pad bitmap controlled by SW
    //set en_pad for channels 7,8,9 (bits 0x380)
    SENS.sar_meas_start2.sar2_en_pad = 1<<channel;
#endif
    return ESP_OK;
}

/*---------------------------------------------------------------
                    DAC
---------------------------------------------------------------*/
esp_err_t dac_pad_get_io_num(dac_channel_t channel, gpio_num_t *gpio_num)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(gpio_num, "Param null", ESP_ERR_INVALID_ARG);

    switch (channel) {
    case DAC_CHANNEL_1:
        *gpio_num = DAC_CHANNEL_1_GPIO_NUM;
        break;
    case DAC_CHANNEL_2:
        *gpio_num = DAC_CHANNEL_2_GPIO_NUM;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t dac_rtc_pad_init(dac_channel_t channel)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    gpio_num_t gpio_num = 0;
    dac_pad_get_io_num(channel, &gpio_num);
    rtc_gpio_init(gpio_num);
    rtc_gpio_output_disable(gpio_num);
    rtc_gpio_input_disable(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
    rtc_gpio_pulldown_dis(gpio_num);

    return ESP_OK;
}

static inline void dac_output_set_enable(dac_channel_t channel, bool enable)
{
    RTCIO.pad_dac[channel-DAC_CHANNEL_1].dac_xpd_force = enable;
    RTCIO.pad_dac[channel-DAC_CHANNEL_1].xpd_dac = enable;
}

esp_err_t dac_output_enable(dac_channel_t channel)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    dac_rtc_pad_init(channel);
    portENTER_CRITICAL(&rtc_spinlock);
    dac_output_set_enable(channel, true);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_output_disable(dac_channel_t channel)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    dac_output_set_enable(channel, false);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_output_voltage(dac_channel_t channel, uint8_t dac_value)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    //Disable Tone
    CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_SW_TONE_EN);

    //Disable Channel Tone
    if (channel == DAC_CHANNEL_1) {
        CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN1_M);
    } else if (channel == DAC_CHANNEL_2) {
        CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN2_M);
    }

    //Set the Dac value
    if (channel == DAC_CHANNEL_1) {
        SET_PERI_REG_BITS(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_DAC, dac_value, RTC_IO_PDAC1_DAC_S);   //dac_output
    } else if (channel == DAC_CHANNEL_2) {
        SET_PERI_REG_BITS(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_DAC, dac_value, RTC_IO_PDAC2_DAC_S);   //dac_output
    }

    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_i2s_enable(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_DAC_DIG_FORCE_M | SENS_DAC_CLK_INV_M);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t dac_i2s_disable(void)
{
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_DAC_DIG_FORCE_M | SENS_DAC_CLK_INV_M);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

/*---------------------------------------------------------------
                        HALL SENSOR
---------------------------------------------------------------*/

static inline void adc1_hall_enable(bool enable)
{
#if CONFIG_IDF_TARGET_ESP32
    RTCIO.hall_sens.xpd_hall = enable;
#endif
}

static int hall_sensor_get_value(void)    //hall sensor without LNA
{
    int hall_value = 0;

    adc_power_on();

#if CONFIG_IDF_TARGET_ESP32
    int Sens_Vp0;
    int Sens_Vn0;
    int Sens_Vp1;
    int Sens_Vn1;
    portENTER_CRITICAL(&rtc_spinlock);
    //disable other peripherals
    adc1_fsm_disable();//currently the LNA is not open, close it by default
    adc1_hall_enable(true);
    // set controller
    adc_set_controller( ADC_UNIT_1, ADC_CTRL_RTC );
    // convert for 4 times with different phase and outputs
    RTCIO.hall_sens.hall_phase = 0;      // hall phase
    Sens_Vp0 = adc_convert( ADC_UNIT_1, ADC1_CHANNEL_0 );
    Sens_Vn0 = adc_convert( ADC_UNIT_1, ADC1_CHANNEL_3 );
    RTCIO.hall_sens.hall_phase = 1;
    Sens_Vp1 = adc_convert( ADC_UNIT_1, ADC1_CHANNEL_0 );
    Sens_Vn1 = adc_convert( ADC_UNIT_1, ADC1_CHANNEL_3 );
    portEXIT_CRITICAL(&rtc_spinlock);
    hall_value = (Sens_Vp1 - Sens_Vp0) - (Sens_Vn1 - Sens_Vn0);
#endif
    return hall_value;
}

int hall_sensor_read(void)
{
    adc_gpio_init(ADC_UNIT_1, ADC1_CHANNEL_0);
    adc_gpio_init(ADC_UNIT_1, ADC1_CHANNEL_3);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);
    return hall_sensor_get_value();
}

/*---------------------------------------------------------------
                        INTERRUPT HANDLER
---------------------------------------------------------------*/


typedef struct rtc_isr_handler_ {
    uint32_t mask;
    intr_handler_t handler;
    void* handler_arg;
    SLIST_ENTRY(rtc_isr_handler_) next;
} rtc_isr_handler_t;

static SLIST_HEAD(rtc_isr_handler_list_, rtc_isr_handler_) s_rtc_isr_handler_list =
        SLIST_HEAD_INITIALIZER(s_rtc_isr_handler_list);
portMUX_TYPE s_rtc_isr_handler_list_lock = portMUX_INITIALIZER_UNLOCKED;
static intr_handle_t s_rtc_isr_handle;

static void rtc_isr(void* arg)
{
    uint32_t status = REG_READ(RTC_CNTL_INT_ST_REG);
    rtc_isr_handler_t* it;
    portENTER_CRITICAL_ISR(&s_rtc_isr_handler_list_lock);
    SLIST_FOREACH(it, &s_rtc_isr_handler_list, next) {
        if (it->mask & status) {
            portEXIT_CRITICAL_ISR(&s_rtc_isr_handler_list_lock);
            (*it->handler)(it->handler_arg);
            portENTER_CRITICAL_ISR(&s_rtc_isr_handler_list_lock);
        }
    }
    portEXIT_CRITICAL_ISR(&s_rtc_isr_handler_list_lock);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, status);
}

static esp_err_t rtc_isr_ensure_installed(void)
{
    esp_err_t err = ESP_OK;
    portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
    if (s_rtc_isr_handle) {
        goto out;
    }

    REG_WRITE(RTC_CNTL_INT_ENA_REG, 0);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, UINT32_MAX);
    err = esp_intr_alloc(ETS_RTC_CORE_INTR_SOURCE, 0, &rtc_isr, NULL, &s_rtc_isr_handle);
    if (err != ESP_OK) {
        goto out;
    }

out:
    portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
    return err;
}


esp_err_t rtc_isr_register(intr_handler_t handler, void* handler_arg, uint32_t rtc_intr_mask)
{
    esp_err_t err = rtc_isr_ensure_installed();
    if (err != ESP_OK) {
        return err;
    }

    rtc_isr_handler_t* item = malloc(sizeof(*item));
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    item->handler = handler;
    item->handler_arg = handler_arg;
    item->mask = rtc_intr_mask;
    portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
    SLIST_INSERT_HEAD(&s_rtc_isr_handler_list, item, next);
    portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
    return ESP_OK;
}

esp_err_t rtc_isr_deregister(intr_handler_t handler, void* handler_arg)
{
    rtc_isr_handler_t* it;
    rtc_isr_handler_t* prev = NULL;
    bool found = false;
    portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
    SLIST_FOREACH(it, &s_rtc_isr_handler_list, next) {
        if (it->handler == handler && it->handler_arg == handler_arg) {
            if (it == SLIST_FIRST(&s_rtc_isr_handler_list)) {
                SLIST_REMOVE_HEAD(&s_rtc_isr_handler_list, next);
            } else {
                SLIST_REMOVE_AFTER(prev, next);
            }
            found = true;
            free(it);
            break;
        }
        prev = it;
    }
    portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
    return found ? ESP_OK : ESP_ERR_INVALID_STATE;
}
