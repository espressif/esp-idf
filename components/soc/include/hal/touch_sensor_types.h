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

#pragma once

#include "soc/touch_sensor_caps.h"
#include "sdkconfig.h"

typedef enum {
    TOUCH_PAD_NUM0 = 0, /*!< Touch pad channel 0 is GPIO4(ESP32) */
    TOUCH_PAD_NUM1,     /*!< Touch pad channel 1 is GPIO0(ESP32) / GPIO1(ESP32-S2) */
    TOUCH_PAD_NUM2,     /*!< Touch pad channel 2 is GPIO2(ESP32) / GPIO2(ESP32-S2) */
    TOUCH_PAD_NUM3,     /*!< Touch pad channel 3 is GPIO15(ESP32) / GPIO3(ESP32-S2) */
    TOUCH_PAD_NUM4,     /*!< Touch pad channel 4 is GPIO13(ESP32) / GPIO4(ESP32-S2) */
    TOUCH_PAD_NUM5,     /*!< Touch pad channel 5 is GPIO12(ESP32) / GPIO5(ESP32-S2) */
    TOUCH_PAD_NUM6,     /*!< Touch pad channel 6 is GPIO14(ESP32) / GPIO6(ESP32-S2) */
    TOUCH_PAD_NUM7,     /*!< Touch pad channel 7 is GPIO27(ESP32) / GPIO7(ESP32-S2) */
    TOUCH_PAD_NUM8,     /*!< Touch pad channel 8 is GPIO33(ESP32) / GPIO8(ESP32-S2) */
    TOUCH_PAD_NUM9,     /*!< Touch pad channel 9 is GPIO32(ESP32) / GPIO9(ESP32-S2) */
#if SOC_TOUCH_SENSOR_NUM > 10
    TOUCH_PAD_NUM10,    /*!< Touch channel 10 is GPIO10(ESP32-S2) */
    TOUCH_PAD_NUM11,    /*!< Touch channel 11 is GPIO11(ESP32-S2) */
    TOUCH_PAD_NUM12,    /*!< Touch channel 12 is GPIO12(ESP32-S2) */
    TOUCH_PAD_NUM13,    /*!< Touch channel 13 is GPIO13(ESP32-S2) */
    TOUCH_PAD_NUM14,    /*!< Touch channel 14 is GPIO14(ESP32-S2) */
#endif
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
    TOUCH_PAD_TIE_OPT_LOW = 0,    /*!<Initial level of charging voltage, low level */
    TOUCH_PAD_TIE_OPT_HIGH = 1,   /*!<Initial level of charging voltage, high level */
    TOUCH_PAD_TIE_OPT_MAX,
} touch_tie_opt_t;

typedef enum {
    TOUCH_FSM_MODE_TIMER = 0,   /*!<To start touch FSM by timer */
    TOUCH_FSM_MODE_SW,          /*!<To start touch FSM by software trigger */
    TOUCH_FSM_MODE_MAX,
} touch_fsm_mode_t;

/**** ESP32 Only *****/

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

/********************************/
#define TOUCH_PAD_SLOPE_DEFAULT             (TOUCH_PAD_SLOPE_7)
#define TOUCH_PAD_TIE_OPT_DEFAULT           (TOUCH_PAD_TIE_OPT_LOW)
#define TOUCH_PAD_BIT_MASK_MAX              (SOC_TOUCH_SENSOR_BIT_MASK_MAX)
#define TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD    (TOUCH_HVOLT_2V7)
#define TOUCH_PAD_LOW_VOLTAGE_THRESHOLD     (TOUCH_LVOLT_0V5)
#define TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD   (TOUCH_HVOLT_ATTEN_0V5)
#define TOUCH_PAD_INACTIVE_CONNECT_DEFAULT  (TOUCH_PAD_CONN_GND)
#define TOUCH_PAD_THRESHOLD_MAX             (SOC_TOUCH_PAD_THRESHOLD_MAX) /*!<If set touch threshold max value, The touch sensor can't be in touched status */

#ifdef CONFIG_IDF_TARGET_ESP32

#define TOUCH_PAD_SLEEP_CYCLE_DEFAULT   (0x1000)  /*!<The timer frequency is RTC_SLOW_CLK (can be 150k or 32k depending on the options), max value is 0xffff */
#define TOUCH_PAD_MEASURE_CYCLE_DEFAULT (0x7fff)  /*!<The timer frequency is 8Mhz, the max value is 0x7fff */
#define TOUCH_FSM_MODE_DEFAULT          (TOUCH_FSM_MODE_SW)  /*!<The touch FSM my be started by the software or timer */
#define TOUCH_TRIGGER_MODE_DEFAULT      (TOUCH_TRIGGER_BELOW)   /*!<Interrupts can be triggered if sensor value gets below or above threshold */
#define TOUCH_TRIGGER_SOURCE_DEFAULT    (TOUCH_TRIGGER_SOURCE_SET1)  /*!<The wakeup trigger source can be SET1 or both SET1 and SET2 */

#elif CONFIG_IDF_TARGET_ESP32S2BETA
/**
 * Excessive total time will slow down the touch response.
 * Too small measurement time will not be sampled enough, resulting in inaccurate measurements.
 *
 * @note The greater the duty cycle of the measurement time, the more system power is consumed.
 */
#define TOUCH_PAD_SLEEP_CYCLE_DEFAULT   (0xf)   /*!<The number of sleep cycle in each measure process of touch channels.
                                                    The timer frequency is RTC_SLOW_CLK (can be 150k or 32k depending on the options).
                                                    Range: 0 ~ 0xffff */
#define TOUCH_PAD_MEASURE_CYCLE_DEFAULT (500)   /*!<The times of charge and discharge in each measure process of touch channels.
                                                    The timer frequency is 8Mhz.
                                                    Recommended typical value: Modify this value to make the measurement time around 1ms.
                                                    Range: 0 ~ 0xffff */
#endif // CONFIG_IDF_TARGET_ESP32

#ifdef CONFIG_IDF_TARGET_ESP32S2BETA

typedef enum {
    TOUCH_PAD_INTR_DONE = 0,    /*!<Each enabled channel measure done */
    TOUCH_PAD_INTR_ACTIVE = 1,  /*!<Each enabled channel be touched */
    TOUCH_PAD_INTR_INACTIVE = 2,/*!<Each enabled channel be released */
    TOUCH_PAD_INTR_ALL,         /*!<All touch interrupt measure done & touched & released */
    TOUCH_PAD_INTR_MAX
} touch_pad_intr_type_t;

typedef enum {
    TOUCH_PAD_INTR_MASK_DONE = BIT(0),      /*!<Each enabled channel measure done */
    TOUCH_PAD_INTR_MASK_ACTIVE = BIT(1),    /*!<Each enabled channel be touched */
    TOUCH_PAD_INTR_MASK_INACTIVE = BIT(2),  /*!<Each enabled channel be released */
    TOUCH_PAD_INTR_MASK_ALL = BIT(2) | BIT(1) | BIT(0), /*!<All touch interrupt measure done & touched & released */
    TOUCH_PAD_INTR_MASK_MAX
} touch_pad_intr_mask_t;

typedef enum {
    TOUCH_PAD_DENOISE_BIT12 = 0,    /*!<Denoise range is 12bit */
    TOUCH_PAD_DENOISE_BIT10 = 1,    /*!<Denoise range is 10bit */
    TOUCH_PAD_DENOISE_BIT8 = 2,     /*!<Denoise range is 8bit */
    TOUCH_PAD_DENOISE_BIT4 = 3,     /*!<Denoise range is 4bit */
    TOUCH_PAD_DENOISE_MAX
} touch_pad_denoise_grade_t;

typedef enum {
    TOUCH_PAD_DENOISE_CAP_L0 = 0,   /*!<Denoise channel internal reference capacitance is 0pf */
    TOUCH_PAD_DENOISE_CAP_L1 = 4,   /*!<Denoise channel internal reference capacitance is 1.4pf */
    TOUCH_PAD_DENOISE_CAP_L2 = 2,   /*!<Denoise channel internal reference capacitance is 2.8pf */
    TOUCH_PAD_DENOISE_CAP_L3 = 6,   /*!<Denoise channel internal reference capacitance is 4.2pf */
    TOUCH_PAD_DENOISE_CAP_L4 = 1,   /*!<Denoise channel internal reference capacitance is 5.6pf */
    TOUCH_PAD_DENOISE_CAP_L5 = 5,   /*!<Denoise channel internal reference capacitance is 7.0pf */
    TOUCH_PAD_DENOISE_CAP_L6 = 3,   /*!<Denoise channel internal reference capacitance is 8.4pf */
    TOUCH_PAD_DENOISE_CAP_L7 = 7,   /*!<Denoise channel internal reference capacitance is 9.8pf */
    TOUCH_PAD_DENOISE_CAP_MAX
} touch_pad_denoise_cap_t;

typedef struct touch_pad_denoise {
    touch_pad_denoise_grade_t grade;    /*!<Select denoise range of denoise channel.
                                            Determined by measuring the noise amplitude of the denoise channel. */
    touch_pad_denoise_cap_t cap_level;  /*!<Select internal reference capacitance of denoise channel.
                                            The equivalent capacitance of the shielded channel can be calculated
                                            from the reading of denoise channel. */
} touch_pad_denoise_t;

typedef enum {
    TOUCH_PAD_SHIELD_DRV_L0 = 0,/*!<The max equivalent capacitance in shield channel is 40pf */
    TOUCH_PAD_SHIELD_DRV_L1,    /*!<The max equivalent capacitance in shield channel is 80pf */
    TOUCH_PAD_SHIELD_DRV_L2,    /*!<The max equivalent capacitance in shield channel is 120pf */
    TOUCH_PAD_SHIELD_DRV_L3,    /*!<The max equivalent capacitance in shield channel is 160pf */
    TOUCH_PAD_SHIELD_DRV_L4,    /*!<The max equivalent capacitance in shield channel is 200pf */
    TOUCH_PAD_SHIELD_DRV_L5,    /*!<The max equivalent capacitance in shield channel is 240pf */
    TOUCH_PAD_SHIELD_DRV_L6,    /*!<The max equivalent capacitance in shield channel is 280pf */
    TOUCH_PAD_SHIELD_DRV_L7,    /*!<The max equivalent capacitance in shield channel is 320pf */
    TOUCH_PAD_SHIELD_DRV_MAX
} touch_pad_shield_driver_t;

typedef struct touch_pad_waterproof {
    touch_pad_t guard_ring_pad;             /*!<Waterproof. Select touch channel use for guard pad */
    touch_pad_shield_driver_t shield_driver;/*!<Waterproof. Select max equivalent capacitance for sheild pad
                                                Config the Touch14 to the touch sensor and compare the measured
                                                reading to the Touch0 reading to estimate the equivalent capacitance.*/
} touch_pad_waterproof_t;

typedef struct touch_pad_proximity {
    touch_pad_t select_pad[SOC_TOUCH_PROXIMITY_CHANNEL_NUM];  /*!<Set touch channel number for proximity pad.
                                                                  If clear the proximity channel, point this pad to `TOUCH_PAD_NUM0` */
    uint32_t meas_num;                                        /*!<Set cumulative times of measurements for proximity pad */
#define TOUCH_PROXIMITY_MEAS_NUM_MAX (0xFF)
} touch_pad_proximity_t;

typedef enum {
    TOUCH_PAD_CONN_HIGHZ = 0,   /*!<Idel status of touch channel is high resistance state */
    TOUCH_PAD_CONN_GND = 1,     /*!<Idel status of touch channel is ground connection */
    TOUCH_PAD_CONN_MAX
} touch_pad_conn_type_t;

typedef enum {
    TOUCH_PAD_FILTER_IIR_2 = 0, /*!<The filter mode is first-order IIR filter. The coefficient is 2 */
    TOUCH_PAD_FILTER_IIR_4,     /*!<The filter mode is first-order IIR filter. The coefficient is 4 */
    TOUCH_PAD_FILTER_IIR_8,     /*!<The filter mode is first-order IIR filter. The coefficient is 8 */
    TOUCH_PAD_FILTER_JITTER,    /*!<The filter mode is jitter filter */
    TOUCH_PAD_FILTER_MAX
} touch_filter_mode_t;

typedef struct touch_filter_config {
    touch_filter_mode_t mode;   /*!<Set filter mode. The input to the filter is raw data and the output is the baseline value.
                                    Larger filter coefficients increase the stability of the baseline. */
    uint32_t debounce_cnt;       /*!<Set debounce count, such as `n`. If the measured values continue to exceed
                                    the threshold for `n` times, it is determined that the touch sensor state changes.
                                    Range: 0 ~ 7 */
    uint32_t hysteresis_thr;     /*!<Hysteresis threshold coefficient. hysteresis = hysteresis_thr * touch_threshold.
                                    If (raw data - baseline) > (touch threshold + hysteresis), the touch channel be touched.
                                    If (raw data - baseline) < (touch threshold - hysteresis), the touch channel be released.
                                    Range: 0 ~ 3. The coefficient is 0: 1/8;  1: 3/32;  2: 1/16;  3: 1/32 */
    uint32_t noise_thr;          /*!<Noise threshold coefficient. noise = noise_thr * touch threshold.
                                    If (raw data - baseline) > (noise), the baseline stop updating.
                                    If (raw data - baseline) < (noise), the baseline start updating.
                                    Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8; */
    uint32_t noise_neg_thr;      /*!<Negative noise threshold coefficient. negative noise = noise_neg_thr * touch threshold.
                                    If (baseline - raw data) > (negative noise), the baseline restart reset process(refer to `baseline_reset`).
                                    If (baseline - raw data) < (negative noise), the baseline stop reset process(refer to `baseline_reset`).
                                    Range: 0 ~ 3. The coefficient is 0: 1/2;  1: 3/8;   2: 1/4;   3: 1/8; */
    uint32_t neg_noise_limit;    /*!<Set the cumulative number of baseline reset processes. such as `n`. If the measured values continue to exceed
                                    the negative noise threshold for `n` times, the baseline reset to raw data.
                                    Range: 0 ~ 15 */
    uint32_t jitter_step;        /*!<Set jitter filter step size. Range: 0 ~ 15 */
#define TOUCH_DEBOUNCE_CNT_MAX      (7)
#define TOUCH_HYSTERESIS_THR_MAX    (3)
#define TOUCH_NOISE_THR_MAX         (3)
#define TOUCH_NOISE_NEG_THR_MAX     (3)
#define TOUCH_NEG_NOISE_CNT_LIMIT   (15)
#define TOUCH_JITTER_STEP_MAX       (15)
} touch_filter_config_t;

typedef struct {
    touch_pad_t touch_num;          /*!<Set touch channel number for sleep pad.
                                        Only one touch sensor channel is supported in deep sleep mode. */
    uint32_t sleep_pad_threshold;   /*!<Set the trigger threshold of touch sensor in deep sleep.
                                        The threshold at sleep is the same as the threshold before sleep. */
    bool en_proximity;              /*!<enable proximity function for sleep pad */
} touch_pad_sleep_channel_t;

#endif // CONFIG_IDF_TARGET_ESP32S2BETA