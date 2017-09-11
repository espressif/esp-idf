// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DRIVER_TOUCH_PAD_H_
#define _DRIVER_TOUCH_PAD_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "esp_intr.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "soc/touch_channel.h"

typedef enum {
    TOUCH_PAD_NUM0 = 0, /*!< Touch pad channel 0 is GPIO4 */
    TOUCH_PAD_NUM1,     /*!< Touch pad channel 1 is GPIO0 */
    TOUCH_PAD_NUM2,     /*!< Touch pad channel 2 is GPIO2 */
    TOUCH_PAD_NUM3,     /*!< Touch pad channel 3 is GPIO15*/
    TOUCH_PAD_NUM4,     /*!< Touch pad channel 4 is GPIO13*/
    TOUCH_PAD_NUM5,     /*!< Touch pad channel 5 is GPIO12*/
    TOUCH_PAD_NUM6,     /*!< Touch pad channel 6 is GPIO14*/
    TOUCH_PAD_NUM7,     /*!< Touch pad channel 7 is GPIO27*/
    TOUCH_PAD_NUM8,     /*!< Touch pad channel 8 is GPIO32*/
    TOUCH_PAD_NUM9,     /*!< Touch pad channel 9 is GPIO33*/
    TOUCH_PAD_MAX,
} touch_pad_t;

typedef enum {
    TOUCH_HVOLT_KEEP = -1, /*!<Touch sensor high reference voltage, no change  */
    TOUCH_HVOLT_2V4 = 0,   /*!<Touch sensor high reference voltage, 2.4V  */
    TOUCH_HVOLT_2V5,       /*!<Touch sensor high reference voltage, 2.5V  */
    TOUCH_HVOLT_2V6,       /*!<Touch sensor high reference voltage, 2.6V  */
    TOUCH_HVOLT_2V7,       /*!<Touch sensor high reference voltage, 2.7V  */
    TOUCH_HVOLT_MAX,
} touch_high_volt_t;

typedef enum {
    TOUCH_LVOLT_KEEP = -1, /*!<Touch sensor low reference voltage, no change  */
    TOUCH_LVOLT_0V5 = 0,   /*!<Touch sensor low reference voltage, 0.5V  */
    TOUCH_LVOLT_0V6,       /*!<Touch sensor low reference voltage, 0.6V  */
    TOUCH_LVOLT_0V7,       /*!<Touch sensor low reference voltage, 0.7V  */
    TOUCH_LVOLT_0V8,       /*!<Touch sensor low reference voltage, 0.8V  */
    TOUCH_LVOLT_MAX,
} touch_low_volt_t;

typedef enum {
    TOUCH_HVOLT_ATTEN_KEEP = -1,  /*!<Touch sensor high reference voltage attenuation, no change  */
    TOUCH_HVOLT_ATTEN_1V5 = 0,    /*!<Touch sensor high reference voltage attenuation, 1.5V attenuation  */
    TOUCH_HVOLT_ATTEN_1V,         /*!<Touch sensor high reference voltage attenuation, 1.0V attenuation  */
    TOUCH_HVOLT_ATTEN_0V5,        /*!<Touch sensor high reference voltage attenuation, 0.5V attenuation  */
    TOUCH_HVOLT_ATTEN_0V,         /*!<Touch sensor high reference voltage attenuation,   0V attenuation  */
    TOUCH_HVOLT_ATTEN_MAX,
} touch_volt_atten_t;

typedef enum {
    TOUCH_PAD_SLOPE_0 = 0,       /*!<Touch sensor charge / discharge speed, always zero  */
    TOUCH_PAD_SLOPE_1 = 1,       /*!<Touch sensor charge / discharge speed, slowest  */
    TOUCH_PAD_SLOPE_2 = 2,       /*!<Touch sensor charge / discharge speed */
    TOUCH_PAD_SLOPE_3 = 3,       /*!<Touch sensor charge / discharge speed  */
    TOUCH_PAD_SLOPE_4 = 4,       /*!<Touch sensor charge / discharge speed  */
    TOUCH_PAD_SLOPE_5 = 5,       /*!<Touch sensor charge / discharge speed  */
    TOUCH_PAD_SLOPE_6 = 6,       /*!<Touch sensor charge / discharge speed  */
    TOUCH_PAD_SLOPE_7 = 7,       /*!<Touch sensor charge / discharge speed, fast  */
    TOUCH_PAD_SLOPE_MAX,
} touch_cnt_slope_t;

typedef enum {
    TOUCH_TRIGGER_BELOW = 0,   /*!<Touch interrupt will happen if counter value is less than threshold.*/
    TOUCH_TRIGGER_ABOVE = 1,   /*!<Touch interrupt will happen if counter value is larger than threshold.*/
    TOUCH_TRIGGER_MAX,
} touch_trigger_mode_t;

typedef enum {
    TOUCH_TRIGGER_SOURCE_BOTH = 0,  /*!< wakeup interrupt is generated if both SET1 and SET2 are "touched"*/
    TOUCH_TRIGGER_SOURCE_SET1 = 1,  /*!< wakeup interrupt is generated if SET1 is "touched"*/
    TOUCH_TRIGGER_SOURCE_MAX,
} touch_trigger_src_t;

typedef enum {
    TOUCH_PAD_TIE_OPT_LOW = 0,    /*!<Initial level of charging voltage, low level */
    TOUCH_PAD_TIE_OPT_HIGH = 1,   /*!<Initial level of charging voltage, high level */
    TOUCH_PAD_TIE_OPT_MAX,
} touch_tie_opt_t;

typedef enum {
    TOUCH_FSM_MODE_TIMER = 0,   /*!<To start touch FSM by timer */
    TOUCH_FSM_MODE_SW,          /*!<To start touch FSM by software trigger */
    TOUCH_FSM_MODE_MAX,
} touch_fsm_mode_t;


typedef intr_handle_t touch_isr_handle_t;

#define TOUCH_PAD_SLEEP_CYCLE_DEFAULT   (0x1000)  /*!<The timer frequency is RTC_SLOW_CLK (can be 150k or 32k depending on the options), max value is 0xffff */
#define TOUCH_PAD_MEASURE_CYCLE_DEFAULT (0xffff)  /*!<The timer frequency is 8Mhz, the max value is 0xffff */
#define TOUCH_FSM_MODE_DEFAULT          (TOUCH_FSM_MODE_TIMER)  /*!<The touch FSM my be started by the software or timer */
#define TOUCH_TRIGGER_MODE_DEFAULT      (TOUCH_TRIGGER_BELOW)   /*!<Interrupts can be triggered if sensor value gets below or above threshold */
#define TOUCH_TRIGGER_SOURCE_DEFAULT    (TOUCH_TRIGGER_SOURCE_SET1)  /*!<The wakeup trigger source can be SET1 or both SET1 and SET2 */
#define TOUCH_PAD_BIT_MASK_MAX          (0x3ff)

/**
 * @brief Initialize touch module.
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Touch pad init error
 */
esp_err_t touch_pad_init();

/**
 * @brief Un-install touch pad driver.
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Touch pad driver not initialized
 */
esp_err_t touch_pad_deinit();

/**
 * @brief Configure touch pad interrupt threshold.
 * @param touch_num touch pad index
 * @param threshold interrupt threshold,
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG if argument wrong
 *     - ESP_FAIL if touch pad not initialized
 */
esp_err_t touch_pad_config(touch_pad_t touch_num, uint16_t threshold);

/**
 * @brief get touch sensor counter value.
 *  Each touch sensor has a counter to count the number of charge/discharge cycles.
 *  When the pad is not 'touched', we can get a number of the counter.
 *  When the pad is 'touched', the value in counter will get smaller because of the larger equivalent capacitance.
 *  User can use this function to determine the interrupt trigger threshold.
 *
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch pad error
 *     - ESP_FAIL Touch pad not initialized
 */
esp_err_t touch_pad_read(touch_pad_t touch_num, uint16_t * touch_value);

/**
 * @brief get filtered touch sensor counter value by IIR filter.
 * @note touch_pad_filter_start has to be called before calling touch_pad_read_filtered.
 *       This function can be called from ISR
 *
 * @param touch_num touch pad index
 * @param touch_value pointer to accept touch sensor value
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Touch pad error
 *     - ESP_FAIL Touch pad not initialized
 */
esp_err_t touch_pad_read_filtered(touch_pad_t touch_num, uint16_t *touch_value);

/**
 * @brief   Register touch-pad ISR,
 * @note Deprecated function, users should replace this with touch_pad_isr_register,
 *       because RTC modules share a same interrupt index.
 * @param fn  Pointer to ISR handler
 * @param arg  Parameter for ISR
 * @param unused Reserved, not used
 * @param handle_unused Reserved, not used
 * @return
 *     - ESP_OK Success ;
 *     - ESP_ERR_INVALID_ARG GPIO error
 */
esp_err_t touch_pad_isr_handler_register(void(*fn)(void *), void *arg, int unused, intr_handle_t *handle_unused) __attribute__ ((deprecated));

/**
 * @brief   Register touch-pad ISR.
 *          The handler will be attached to the same CPU core that this function is running on.
 * @param fn  Pointer to ISR handler
 * @param arg  Parameter for ISR
 * @return
 *     - ESP_OK Success ;
 *     - ESP_ERR_INVALID_ARG GPIO error
 */
esp_err_t touch_pad_isr_register(intr_handler_t fn, void* arg);

/**
 * @brief Deregister the handler previously registered using touch_pad_isr_handler_register
 * @param fn  handler function to call (as passed to touch_pad_isr_handler_register)
 * @param arg  argument of the handler (as passed to touch_pad_isr_handler_register)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if a handler matching both fn and
 *        arg isn't registered
 */
esp_err_t touch_pad_isr_deregister(void(*fn)(void *), void *arg);

/**
 * @brief Set touch sensor measurement and sleep time
 * @param sleep_cycle  The touch sensor will sleep after each measurement.
 *                     sleep_cycle decide the interval between each measurement.
 *                     t_sleep = sleep_cycle / (RTC_SLOW_CLK frequency).
 *                     The approximate frequency value of RTC_SLOW_CLK can be obtained using rtc_clk_slow_freq_get_hz function.
 * @param meas_cycle The duration of the touch sensor measurement.
 *                   t_meas = meas_cycle / 8M, the maximum measure time is 0xffff / 8M = 8.19 ms
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_cycle);

/**
 * @brief Get touch sensor measurement and sleep time
 * @param sleep_cycle  Pointer to accept sleep cycle number
 * @param meas_cycle Pointer to accept measurement cycle count.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_cycle);

/**
 * @brief Set touch sensor reference voltage, if the voltage gap between high and low reference voltage get less,
 *        the charging and discharging time would be faster, accordingly, the counter value would be larger.
 *        In the case of detecting very slight change of capacitance, we can narrow down the gap so as to increase
 *        the sensitivity. On the other hand, narrow voltage gap would also introduce more noise, but we can use a
 *        software filter to pre-process the counter value.
 * @param refh the value of DREFH
 * @param refl the value of DREFL
 * @param atten the attenuation on DREFH
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_voltage(touch_high_volt_t refh, touch_low_volt_t refl, touch_volt_atten_t atten);

/**
 * @brief Get touch sensor reference voltage,
 * @param refh pointer to accept DREFH value
 * @param refl pointer to accept DREFL value
 * @param atten pointer to accept the attenuation on DREFH
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_voltage(touch_high_volt_t *refh, touch_low_volt_t *refl, touch_volt_atten_t *atten);

/**
 * @brief Set touch sensor charge/discharge speed for each pad.
 *        If the slope is 0, the counter would always be zero.
 *        If the slope is 1, the charging and discharging would be slow, accordingly, the counter value would be small.
 *        If the slope is set 7, which is the maximum value, the charging and discharging would be fast, accordingly, the
 *        counter value would be larger.
 * @param touch_num touch pad index
 * @param slope touch pad charge/discharge speed
 * @param opt the initial voltage
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t slope, touch_tie_opt_t opt);

/**
 * @brief Get touch sensor charge/discharge speed for each pad
 * @param touch_num touch pad index
 * @param slope pointer to accept touch pad charge/discharge slope
 * @param opt pointer to accept the initial voltage
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_get_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t *slope, touch_tie_opt_t *opt);

/**
 * @brief Initialize touch pad GPIO
 * @param touch_num touch pad index
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_io_init(touch_pad_t touch_num);

/**
 * @brief Set touch sensor FSM mode, the test action can be triggered by the timer,
 *        as well as by the software.
 * @param mode FSM mode
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_fsm_mode(touch_fsm_mode_t mode);

/**
 * @brief Get touch sensor FSM mode
 * @param mode pointer to accept FSM mode
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_fsm_mode(touch_fsm_mode_t *mode);

/**
 * @brief Trigger a touch sensor measurement, only support in SW mode of FSM
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_sw_start();

/**
 * @brief Set touch sensor interrupt threshold
 * @param touch_num touch pad index
 * @param threshold threshold of touchpad count, refer to touch_pad_set_trigger_mode to see how to set trigger mode.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint16_t threshold);

/**
 * @brief Get touch sensor interrupt threshold
 * @param touch_num touch pad index
 * @param threshold pointer to accept threshold
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint16_t *threshold);

/**
 * @brief Set touch sensor interrupt trigger mode.
 *        Interrupt can be triggered either when counter result is less than
 *        threshold or when counter result is more than threshold.
 * @param mode touch sensor interrupt trigger mode
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_trigger_mode(touch_trigger_mode_t mode);

/**
 * @brief Get touch sensor interrupt trigger mode
 * @param mode pointer to accept touch sensor interrupt trigger mode
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_trigger_mode(touch_trigger_mode_t *mode);

/**
 * @brief Set touch sensor interrupt trigger source. There are two sets of touch signals.
 *        Set1 and set2 can be mapped to several touch signals. Either set will be triggered
 *        if at least one of its touch signal is 'touched'. The interrupt can be configured to be generated
 *        if set1 is triggered, or only if both sets are triggered.
 * @param src touch sensor interrupt trigger source
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_trigger_source(touch_trigger_src_t src);

/**
 * @brief Get touch sensor interrupt trigger source
 * @param src pointer to accept touch sensor interrupt trigger source
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_trigger_source(touch_trigger_src_t *src);

/**
 * @brief Set touch sensor group mask.
 *        Touch pad module has two sets of signals, 'Touched' signal is triggered only if
 *        at least one of touch pad in this group is "touched".
 *        This function will set the register bits according to the given bitmask.
 * @param set1_mask bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask bitmask of touch sensor signal group2, it's a 10-bit value
 * @param en_mask bitmask of touch sensor work enable, it's a 10-bit value
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_set_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask);

/**
 * @brief Get touch sensor group mask.
 * @param set1_mask pointer to accept bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask pointer to accept bitmask of touch sensor signal group2, it's a 10-bit value
 * @param en_mask pointer to accept bitmask of touch sensor work enable, it's a 10-bit value
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_get_group_mask(uint16_t *set1_mask, uint16_t *set2_mask, uint16_t *en_mask);

/**
 * @brief Clear touch sensor group mask.
 *        Touch pad module has two sets of signals, Interrupt is triggered only if
 *        at least one of touch pad in this group is "touched".
 *        This function will clear the register bits according to the given bitmask.
 * @param set1_mask bitmask touch sensor signal group1, it's a 10-bit value
 * @param set2_mask bitmask touch sensor signal group2, it's a 10-bit value
 * @param en_mask bitmask of touch sensor work enable, it's a 10-bit value
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if argument is wrong
 */
esp_err_t touch_pad_clear_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask);

/**
 * @brief To clear the touch status register, usually use this function in touch ISR to clear status.
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_clear_status();

/**
 * @brief Get the touch sensor status, usually used in ISR to decide which pads are 'touched'.
 * @return
 *      - touch status
 */
uint32_t touch_pad_get_status();

/**
 * @brief To enable touch pad interrupt
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_enable();

/**
 * @brief To disable touch pad interrupt
 * @return
 *      - ESP_OK on success
 */
esp_err_t touch_pad_intr_disable();

/**
 * @brief set touch pad filter calibration period, in ms.
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @param new_period_ms filter period, in ms
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 *      - ESP_ERR_INVALID_ARG parameter error
 */
esp_err_t touch_pad_set_filter_period(uint32_t new_period_ms);

/**
 * @brief get touch pad filter calibration period, in ms
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @param p_period_ms pointer to accept period
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 *      - ESP_ERR_INVALID_ARG parameter error
 */
esp_err_t touch_pad_get_filter_period(uint32_t* p_period_ms);

/**
 * @brief start touch pad filter function
 *      This API will start a filter to process the noise in order to prevent false triggering
 *      when detecting slight change of capacitance.
 *      Need to call touch_pad_filter_start before all touch filter APIs
 *
 *      If filter is not initialized, this API will initialize the filter with given period.
 *      If filter is already initialized, this API will update the filter period.
 * @note This filter uses FreeRTOS timer, which is dispatched from a task with
 *       priority 1 by default on CPU 0. So if some application task with higher priority
 *       takes a lot of CPU0 time, then the quality of data obtained from this filter will be affected.
 *       You can adjust FreeRTOS timer task priority in menuconfig.
 * @param filter_period_ms filter calibration period, in ms
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG parameter error
 *      - ESP_ERR_NO_MEM No memory for driver
 *      - ESP_ERR_INVALID_STATE driver state error
 */
esp_err_t touch_pad_filter_start(uint32_t filter_period_ms);

/**
 * @brief stop touch pad filter function
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 */
esp_err_t touch_pad_filter_stop();

/**
 * @brief delete touch pad filter driver and release the memory
 *        Need to call touch_pad_filter_start before all touch filter APIs
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_STATE driver state error
 */
esp_err_t touch_pad_filter_delete();

#ifdef __cplusplus
}
#endif

#endif/*_DRIVER_TOUCH_PAD_H_*/

