/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soc/gpio_periph.h"
#include "soc/ledc_periph.h"
#include "esp_private/esp_clk.h"
#include "soc/soc_caps.h"
#include "hal/ledc_hal.h"
#include "hal/gpio_hal.h"
#include "driver/ledc.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "clk_ctrl_os.h"
#include "esp_private/periph_ctrl.h"

static __attribute__((unused)) const char *LEDC_TAG = "ledc";

#define LEDC_CHECK(a, str, ret_val) ESP_RETURN_ON_FALSE(a, ret_val, LEDC_TAG, "%s", str)
#define LEDC_ARG_CHECK(a, param) ESP_RETURN_ON_FALSE(a, ESP_ERR_INVALID_ARG, LEDC_TAG, param " argument is invalid")

#define LEDC_CLK_NOT_FOUND  0

typedef enum {
    LEDC_FSM_IDLE,
    LEDC_FSM_HW_FADE,
    LEDC_FSM_ISR_CAL,
    LEDC_FSM_KILLED_PENDING,
} ledc_fade_fsm_t;

typedef struct {
    ledc_mode_t speed_mode;
    ledc_duty_direction_t direction;
    uint32_t target_duty;
    int cycle_num;
    int scale;
    ledc_fade_mode_t mode;
    SemaphoreHandle_t ledc_fade_sem;
    SemaphoreHandle_t ledc_fade_mux;
#if CONFIG_SPIRAM_USE_MALLOC
    StaticQueue_t ledc_fade_sem_storage;
#endif
    ledc_cb_t ledc_fade_callback;
    void *cb_user_arg;
    volatile ledc_fade_fsm_t fsm;
} ledc_fade_t;

typedef struct {
    ledc_hal_context_t ledc_hal;        /*!< LEDC hal context*/
} ledc_obj_t;

static ledc_obj_t *p_ledc_obj[LEDC_SPEED_MODE_MAX] = {0};
static ledc_fade_t *s_ledc_fade_rec[LEDC_SPEED_MODE_MAX][LEDC_CHANNEL_MAX];
static ledc_isr_handle_t s_ledc_fade_isr_handle = NULL;
static portMUX_TYPE ledc_spinlock = portMUX_INITIALIZER_UNLOCKED;

#define LEDC_VAL_NO_CHANGE        (-1)
#define LEDC_DUTY_NUM_MAX         LEDC_LL_DUTY_NUM_MAX            // Maximum steps per hardware fade
#define LEDC_DUTY_DECIMAL_BIT_NUM (4)
#define LEDC_TIMER_DIV_NUM_MAX    (0x3FFFF)
#define LEDC_FADE_TOO_SLOW_STR    "LEDC FADE TOO SLOW"
#define LEDC_FADE_TOO_FAST_STR    "LEDC FADE TOO FAST"
#define DIM(array)                (sizeof(array)/sizeof(*array))
#define LEDC_IS_DIV_INVALID(div)  ((div) <= LEDC_LL_FRACTIONAL_MAX || (div) > LEDC_TIMER_DIV_NUM_MAX)

static __attribute__((unused)) const char *LEDC_NOT_INIT = "LEDC is not initialized";
static __attribute__((unused)) const char *LEDC_FADE_SERVICE_ERR_STR = "LEDC fade service not installed";
static __attribute__((unused)) const char *LEDC_FADE_INIT_ERROR_STR = "LEDC fade channel init error, not enough memory or service not installed";

//This value will be calibrated when in use.
static uint32_t s_ledc_slow_clk_8M = 0;

static const ledc_slow_clk_sel_t s_glb_clks[] = LEDC_LL_GLOBAL_CLOCKS;
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
static const struct { ledc_clk_src_t clk; uint32_t freq; } s_timer_specific_clks[] = LEDC_LL_TIMER_SPECIFIC_CLOCKS;
#endif

static void ledc_ls_timer_update(ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    if (speed_mode == LEDC_LOW_SPEED_MODE) {
        ledc_hal_ls_timer_update(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel);
    }
}

static IRAM_ATTR void ledc_ls_channel_update(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if (speed_mode == LEDC_LOW_SPEED_MODE) {
        ledc_hal_ls_channel_update(&(p_ledc_obj[speed_mode]->ledc_hal), channel);
    }
}

//We know that CLK8M is about 8M, but don't know the actual value. So we need to do a calibration.
static bool ledc_slow_clk_calibrate(void)
{
    if (periph_rtc_dig_clk8m_enable()) {
        s_ledc_slow_clk_8M = periph_rtc_dig_clk8m_get_freq();
#if CONFIG_IDF_TARGET_ESP32H2
        /* Workaround: Calibration cannot be done for CLK8M on H2, we just use its theoretic frequency */
        ESP_LOGD(LEDC_TAG, "Calibration cannot be performed, approximate CLK8M_CLK : %"PRIu32" Hz", s_ledc_slow_clk_8M);
#else
        ESP_LOGD(LEDC_TAG, "Calibrate CLK8M_CLK : %"PRIu32" Hz", s_ledc_slow_clk_8M);
#endif
        return true;
    }
    ESP_LOGE(LEDC_TAG, "Calibrate CLK8M_CLK failed");
    return false;
}

static uint32_t ledc_get_src_clk_freq(ledc_clk_cfg_t clk_cfg)
{
    uint32_t src_clk_freq = 0;
    if (clk_cfg == LEDC_USE_RTC8M_CLK) {
        src_clk_freq = s_ledc_slow_clk_8M;
#if SOC_LEDC_SUPPORT_APB_CLOCK
    } else if (clk_cfg == LEDC_USE_APB_CLK) {
        src_clk_freq = esp_clk_apb_freq();
#endif
#if SOC_LEDC_SUPPORT_PLL_DIV_CLOCK
    } else if (clk_cfg == LEDC_USE_PLL_DIV_CLK) {
        src_clk_freq = LEDC_LL_PLL_DIV_CLK_FREQ;
#endif
#if SOC_LEDC_SUPPORT_REF_TICK
    } else if (clk_cfg == LEDC_USE_REF_TICK) {
        src_clk_freq = REF_CLK_FREQ;
#endif
#if SOC_LEDC_SUPPORT_XTAL_CLOCK
    } else if (clk_cfg == LEDC_USE_XTAL_CLK) {
        src_clk_freq = esp_clk_xtal_freq();
#endif
    }
    return src_clk_freq;
}

/* Retrieve the clock frequency for global clocks only */
static uint32_t ledc_get_glb_clk_freq(ledc_slow_clk_sel_t clk_cfg)
{
    uint32_t src_clk_freq = 0;

    switch (clk_cfg) {
#if SOC_LEDC_SUPPORT_APB_CLOCK
        case LEDC_SLOW_CLK_APB:
            src_clk_freq = esp_clk_apb_freq();
            break;
#endif
#if SOC_LEDC_SUPPORT_PLL_DIV_CLOCK
        case LEDC_SLOW_CLK_PLL_DIV:
            src_clk_freq = LEDC_LL_PLL_DIV_CLK_FREQ;
            break;
#endif
        case LEDC_SLOW_CLK_RTC8M:
            src_clk_freq = s_ledc_slow_clk_8M;
            break;
#if SOC_LEDC_SUPPORT_XTAL_CLOCK
        case LEDC_SLOW_CLK_XTAL:
            src_clk_freq = esp_clk_xtal_freq();
            break;
#endif
    }

    return src_clk_freq;
}


static esp_err_t ledc_enable_intr_type(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_intr_type_t type)
{
    if (type == LEDC_INTR_FADE_END) {
        ledc_hal_set_fade_end_intr(&(p_ledc_obj[speed_mode]->ledc_hal), channel, true);
    } else {
        ledc_hal_set_fade_end_intr(&(p_ledc_obj[speed_mode]->ledc_hal), channel, false);
    }
    return ESP_OK;
}

static void _ledc_fade_hw_acquire(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t *fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreTake(fade->ledc_fade_sem, portMAX_DELAY);
        portENTER_CRITICAL(&ledc_spinlock);
        ledc_enable_intr_type(mode, channel, LEDC_INTR_DISABLE);
        portEXIT_CRITICAL(&ledc_spinlock);
    }
}

static void _ledc_fade_hw_release(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t *fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreGive(fade->ledc_fade_sem);
    }
}

static void _ledc_op_lock_acquire(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t *fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreTake(fade->ledc_fade_mux, portMAX_DELAY);
    }
}

static void _ledc_op_lock_release(ledc_mode_t mode, ledc_channel_t channel)
{
    ledc_fade_t *fade = s_ledc_fade_rec[mode][channel];
    if (fade) {
        xSemaphoreGive(fade->ledc_fade_mux);
    }
}

static uint32_t ledc_get_max_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    // The arguments are checked before internally calling this function.
    uint32_t max_duty;
    ledc_hal_get_max_duty(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &max_duty);
    return max_duty;
}

esp_err_t ledc_timer_set(ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t clock_divider, uint32_t duty_resolution,
                         ledc_clk_src_t clk_src)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_hal_set_clock_divider(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel, clock_divider);
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
    /* Clock source can only be configured on boards which support timer-specific
     * source clock. */
    ledc_hal_set_clock_source(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel, clk_src);
#endif
    ledc_hal_set_duty_resolution(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel, duty_resolution);
    ledc_ls_timer_update(speed_mode, timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

static IRAM_ATTR esp_err_t ledc_duty_config(ledc_mode_t speed_mode, ledc_channel_t channel, int hpoint_val,
int duty_val, ledc_duty_direction_t duty_direction, uint32_t duty_num, uint32_t duty_cycle, uint32_t duty_scale)
{
    if (hpoint_val >= 0) {
        ledc_hal_set_hpoint(&(p_ledc_obj[speed_mode]->ledc_hal), channel, hpoint_val);
    }
    if (duty_val >= 0) {
        ledc_hal_set_duty_int_part(&(p_ledc_obj[speed_mode]->ledc_hal), channel, duty_val);
    }
    ledc_hal_set_duty_direction(&(p_ledc_obj[speed_mode]->ledc_hal), channel, duty_direction);
    ledc_hal_set_duty_num(&(p_ledc_obj[speed_mode]->ledc_hal), channel, duty_num);
    ledc_hal_set_duty_cycle(&(p_ledc_obj[speed_mode]->ledc_hal), channel, duty_cycle);
    ledc_hal_set_duty_scale(&(p_ledc_obj[speed_mode]->ledc_hal), channel, duty_scale);
    ledc_ls_channel_update(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_bind_channel_timer(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_timer_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_hal_bind_channel_timer(&(p_ledc_obj[speed_mode]->ledc_hal), channel, timer_sel);
    ledc_ls_channel_update(speed_mode, channel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_rst(ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_hal_timer_rst(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_pause(ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_hal_timer_pause(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_timer_resume(ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_sel < LEDC_TIMER_MAX, "timer_select");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_hal_timer_resume(&(p_ledc_obj[speed_mode]->ledc_hal), timer_sel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, ledc_isr_handle_t *handle)
{
    esp_err_t ret;
    LEDC_ARG_CHECK(fn, "fn");
    portENTER_CRITICAL(&ledc_spinlock);
    ret = esp_intr_alloc(ETS_LEDC_INTR_SOURCE, intr_alloc_flags, fn, arg, handle);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ret;
}

static inline uint32_t ledc_calculate_divisor(uint32_t src_clk_freq, int freq_hz, uint32_t precision)
{
    /**
     * In order to find the right divisor, we need to divide the source clock
     * frequency by the desired frequency. However, two things to note here:
     * - The lowest LEDC_LL_FRACTIONAL_BITS bits of the result are the FRACTIONAL
     *   part. The higher bits represent the integer part, this is why we need
     *   to right shift the source frequency.
     * - The `precision` parameter represents the granularity of the clock. It
     *   **must** be a power of 2. It means that the resulted divisor is
     *   a multiplier of `precision`.
     *
     * Let's take a concrete example, we need to generate a 5KHz clock out of
     * a 80MHz clock (APB).
     * If the precision is 1024 (10 bits), the resulted multiplier is:
     * (80000000 << 8) / (5000 * 1024) = 4000 (0xfa0)
     * Let's ignore the fractional part to simplify the explanation, so we get
     * a result of 15 (0xf).
     * This can be interpreted as: every 15 "precision" ticks, the resulted
     * clock will go high, where one precision tick is made out of 1024 source
     * clock ticks.
     * Thus, every `15 * 1024` source clock ticks, the resulted clock will go
     * high.
     *
     * NOTE: We are also going to round up the value when necessary, thanks to:
     * (freq_hz * precision) / 2
     */
    return ( ( (uint64_t) src_clk_freq << LEDC_LL_FRACTIONAL_BITS ) + ((freq_hz * precision) / 2 ) )
           / (freq_hz * precision);
}

static inline uint32_t ledc_auto_global_clk_divisor(int freq_hz, uint32_t precision, ledc_slow_clk_sel_t* clk_target)
{
    uint32_t ret = LEDC_CLK_NOT_FOUND;
    uint32_t clk_freq = 0;

    /* This function will go through all the following clock sources to look
     * for a valid divisor which generates the requested frequency. */
    for (int i = 0; i < DIM(s_glb_clks); i++) {
        /* Before calculating the divisor, we need to have the RTC frequency.
         * If it hasn't been measured yet, try calibrating it now. */
        if (s_glb_clks[i] == LEDC_SLOW_CLK_RTC8M && s_ledc_slow_clk_8M == 0 && !ledc_slow_clk_calibrate()) {
            ESP_LOGD(LEDC_TAG, "Unable to retrieve RTC clock frequency, skipping it\n");
            continue;
        }

        clk_freq = ledc_get_glb_clk_freq(s_glb_clks[i]);
        uint32_t div_param = ledc_calculate_divisor(clk_freq, freq_hz, precision);

        /* If the divisor is valid, we can return this value. */
        if (!LEDC_IS_DIV_INVALID(div_param)) {
            *clk_target = s_glb_clks[i];
            ret = div_param;
            break;
        }
    }

    return ret;
}

#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
static inline uint32_t ledc_auto_timer_specific_clk_divisor(ledc_mode_t speed_mode, int freq_hz, uint32_t precision,
                                                            ledc_clk_src_t* clk_source)
{
    uint32_t ret = LEDC_CLK_NOT_FOUND;

    for (int i = 0; i < DIM(s_timer_specific_clks); i++) {
        uint32_t div_param = ledc_calculate_divisor(s_timer_specific_clks[i].freq, freq_hz, precision);

        /* If the divisor is valid, we can return this value. */
        if (!LEDC_IS_DIV_INVALID(div_param)) {
            *clk_source = s_timer_specific_clks[i].clk;
            ret = div_param;
            break;
        }
    }

#if SOC_LEDC_SUPPORT_HS_MODE
    /* On board that support LEDC high-speed mode, APB clock becomes a timer-
     * specific clock when in high speed mode. Check if it is necessary here
     * to test APB. */
    if (speed_mode == LEDC_HIGH_SPEED_MODE && ret == LEDC_CLK_NOT_FOUND) {
        /* No divider was found yet, try with APB! */
        uint32_t div_param = ledc_calculate_divisor(esp_clk_apb_freq(), freq_hz, precision);

        if (!LEDC_IS_DIV_INVALID(div_param)) {
            *clk_source = LEDC_APB_CLK;
            ret = div_param;
        }
    }
#endif

    return ret;
}
#endif

/**
 * @brief Try to find the clock with its divisor giving the frequency requested
 * by the caller.
 */
static uint32_t ledc_auto_clk_divisor(ledc_mode_t speed_mode, int freq_hz, uint32_t precision,
                                      ledc_clk_src_t* clk_source, ledc_slow_clk_sel_t* clk_target)
{
    uint32_t ret = LEDC_CLK_NOT_FOUND;

#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
    /* If the SoC presents timer-specific clock(s), try to achieve the given frequency
     * thanks to it/them.
     * clk_source parameter will returned by this function. */
    uint32_t div_param_timer = ledc_auto_timer_specific_clk_divisor(speed_mode, freq_hz, precision, clk_source);

    if (div_param_timer != LEDC_CLK_NOT_FOUND) {
        /* The dividor is valid, no need try any other clock, return directly. */
        ret = div_param_timer;
    }
#endif

    /* On ESP32, only low speed channel can use the global clocks. For other
     * chips, there are no high speed channels. */
    if (ret == LEDC_CLK_NOT_FOUND && speed_mode == LEDC_LOW_SPEED_MODE) {
        uint32_t div_param_global = ledc_auto_global_clk_divisor(freq_hz, precision, clk_target);
        if (div_param_global != LEDC_CLK_NOT_FOUND) {
            *clk_source = LEDC_SCLK;
            ret = div_param_global;
        }
    }

    return ret;
}

static ledc_slow_clk_sel_t ledc_clk_cfg_to_global_clk(const ledc_clk_cfg_t clk_cfg)
{
    ledc_slow_clk_sel_t glb_clk;

    switch (clk_cfg) {
#if SOC_LEDC_SUPPORT_APB_CLOCK
        case LEDC_USE_APB_CLK:
            glb_clk = LEDC_SLOW_CLK_APB;
            break;
#endif
#if SOC_LEDC_SUPPORT_PLL_DIV_CLOCK
        case LEDC_USE_PLL_DIV_CLK:
            glb_clk = LEDC_SLOW_CLK_PLL_DIV;
            break;
#endif
        case LEDC_USE_RTC8M_CLK:
            glb_clk = LEDC_SLOW_CLK_RTC8M;
            break;
#if SOC_LEDC_SUPPORT_XTAL_CLOCK
        case LEDC_USE_XTAL_CLK:
            glb_clk = LEDC_SLOW_CLK_XTAL;
            break;
#endif
#if SOC_LEDC_SUPPORT_REF_TICK
        case LEDC_USE_REF_TICK:
#endif
        default:
            /* We should not get here, REF_TICK is NOT a global clock,
             * it is a timer-specific clock. */
            abort();
    }

    return glb_clk;
}

extern void esp_sleep_periph_use_8m(bool use_or_not);

/**
 * @brief Function setting the LEDC timer divisor with the given source clock,
 * frequency and resolution. If the clock configuration passed is
 * LEDC_AUTO_CLK, the clock will be determined automatically (if possible).
 */
static esp_err_t ledc_set_timer_div(ledc_mode_t speed_mode, ledc_timer_t timer_num, ledc_clk_cfg_t clk_cfg, int freq_hz, int duty_resolution)
{
    uint32_t div_param = 0;
    const uint32_t precision = ( 0x1 << duty_resolution );
    /* The clock sources are not initialized on purpose. To produce compiler warning if used but the selector functions
     * don't set them properly. */
    /* Timer-specific mux. Set to timer-specific clock or LEDC_SCLK if a global clock is used. */
    ledc_clk_src_t timer_clk_src;
    /* Global clock mux. Should be set when LEDC_SCLK is used in LOW_SPEED_MODE. Otherwise left uninitialized. */
    ledc_slow_clk_sel_t glb_clk;

    if (clk_cfg == LEDC_AUTO_CLK) {
        /* User hasn't specified the speed, we should try to guess it. */
        div_param = ledc_auto_clk_divisor(speed_mode, freq_hz, precision, &timer_clk_src, &glb_clk);
    } else if (clk_cfg == LEDC_USE_RTC8M_CLK) {
        /* User specified source clock(RTC8M_CLK) for low speed channel.
         * Make sure the speed mode is correct. */
        ESP_RETURN_ON_FALSE((speed_mode == LEDC_LOW_SPEED_MODE), ESP_ERR_INVALID_ARG, LEDC_TAG, "RTC clock can only be used in low speed mode");

        /* Before calculating the divisor, we need to have the RTC frequency.
         * If it hasn't been measured yet, try calibrating it now. */
        if(s_ledc_slow_clk_8M == 0 && ledc_slow_clk_calibrate() == false) {
            goto error;
        }

        /* Set the global clock source */
        timer_clk_src = LEDC_SCLK;
        glb_clk =  LEDC_SLOW_CLK_RTC8M;

        /* We have the RTC clock frequency now. */
        div_param = ledc_calculate_divisor(s_ledc_slow_clk_8M, freq_hz, precision);
        if (LEDC_IS_DIV_INVALID(div_param)) {
            div_param = LEDC_CLK_NOT_FOUND;
        }
    } else {
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
        if (LEDC_LL_IS_TIMER_SPECIFIC_CLOCK(speed_mode, clk_cfg)) {
            /* Currently we can convert a timer-specific clock to a source clock that
             * easily because their values are identical in the enumerations (on purpose)
             * If we decide to change the values in the future, we should consider defining
             * a macro/function to convert timer-specific clock to clock source .*/
            timer_clk_src = (ledc_clk_src_t) clk_cfg;
        } else
#endif
        {
            timer_clk_src = LEDC_SCLK;
            glb_clk = ledc_clk_cfg_to_global_clk(clk_cfg);
        }

        uint32_t src_clk_freq = ledc_get_src_clk_freq(clk_cfg);
        div_param = ledc_calculate_divisor(src_clk_freq, freq_hz, precision);
        if (LEDC_IS_DIV_INVALID(div_param)) {
            div_param = LEDC_CLK_NOT_FOUND;
        }
    }

    if (div_param == LEDC_CLK_NOT_FOUND) {
        goto error;
    }

    /* The following debug message makes more sense for AUTO mode. */
    ESP_LOGD(LEDC_TAG, "Using clock source %d (in %s mode), divisor: 0x%"PRIx32,
             timer_clk_src, (speed_mode == LEDC_LOW_SPEED_MODE ? "slow" : "fast"), div_param);

    /* The following block configures the global clock.
     * Thus, in theory, this only makes sense when configuring the LOW_SPEED timer and the source clock is LEDC_SCLK (as
     * HIGH_SPEED timers won't be clocked by the global clock). However, there are some limitations due to HW design.
     */
    if (speed_mode == LEDC_LOW_SPEED_MODE) {
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
        /* On ESP32 and ESP32-S2, when the source clock of LOW_SPEED timer is a timer-specific one (i.e. REF_TICK), the
         * global clock MUST be set to APB_CLK. For HIGH_SPEED timers, this is not necessary.
         */
        if (timer_clk_src != LEDC_SCLK) {
            glb_clk = LEDC_SLOW_CLK_APB;
        }
#else
        /* On later chips, there is only one type of timer/channel (referred as LOW_SPEED in the code), which can only be
         * clocked by the global clock. So there's no limitation on the global clock, except that it must be set.
         */
        assert(timer_clk_src == LEDC_SCLK);
#endif
        ESP_LOGD(LEDC_TAG, "In slow speed mode, global clk set: %d", glb_clk);

        /* keep ESP_PD_DOMAIN_RTC8M on during light sleep */
        esp_sleep_periph_use_8m(glb_clk == LEDC_SLOW_CLK_RTC8M);

        portENTER_CRITICAL(&ledc_spinlock);
        ledc_hal_set_slow_clk_sel(&(p_ledc_obj[speed_mode]->ledc_hal), glb_clk);
        portEXIT_CRITICAL(&ledc_spinlock);
    }

    /* The divisor is correct, we can write in the hardware. */
    ledc_timer_set(speed_mode, timer_num, div_param, duty_resolution, timer_clk_src);
    return ESP_OK;

error:
    ESP_LOGE(LEDC_TAG, "requested frequency and duty resolution can not be achieved, try reducing freq_hz or duty_resolution. div_param=%"PRIu32, div_param);
    return ESP_FAIL;
}

esp_err_t ledc_timer_config(const ledc_timer_config_t *timer_conf)
{
    LEDC_ARG_CHECK(timer_conf != NULL, "timer_conf");
    uint32_t freq_hz = timer_conf->freq_hz;
    uint32_t duty_resolution = timer_conf->duty_resolution;
    uint32_t timer_num = timer_conf->timer_num;
    uint32_t speed_mode = timer_conf->speed_mode;
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(!((timer_conf->clk_cfg == LEDC_USE_RTC8M_CLK) && (speed_mode != LEDC_LOW_SPEED_MODE)), "Only low speed channel support RTC8M_CLK");
    periph_module_enable(PERIPH_LEDC_MODULE);
    if (freq_hz == 0 || duty_resolution == 0 || duty_resolution >= LEDC_TIMER_BIT_MAX) {
        ESP_LOGE(LEDC_TAG, "freq_hz=%"PRIu32" duty_resolution=%"PRIu32, freq_hz, duty_resolution);
        return ESP_ERR_INVALID_ARG;
    }
    if (timer_num > LEDC_TIMER_3) {
        ESP_LOGE(LEDC_TAG, "invalid timer #%"PRIu32, timer_num);
        return ESP_ERR_INVALID_ARG;
    }

    if (p_ledc_obj[speed_mode] == NULL) {
        p_ledc_obj[speed_mode] = (ledc_obj_t *) heap_caps_calloc(1, sizeof(ledc_obj_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (p_ledc_obj[speed_mode] == NULL) {
            return ESP_ERR_NO_MEM;
        }
        ledc_hal_init(&(p_ledc_obj[speed_mode]->ledc_hal), speed_mode);
    }

    esp_err_t ret = ledc_set_timer_div(speed_mode, timer_num, timer_conf->clk_cfg, freq_hz, duty_resolution);
    if (ret == ESP_OK) {
        /* Reset the timer. */
        ledc_timer_rst(speed_mode, timer_num);
    }
    return ret;
}

esp_err_t ledc_set_pin(int gpio_num, ledc_mode_t speed_mode, ledc_channel_t ledc_channel)
{
    LEDC_ARG_CHECK(ledc_channel < LEDC_CHANNEL_MAX, "ledc_channel");
    LEDC_ARG_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "gpio_num");
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(gpio_num, ledc_periph_signal[speed_mode].sig_out0_idx + ledc_channel, 0, 0);
    return ESP_OK;
}

esp_err_t ledc_channel_config(const ledc_channel_config_t *ledc_conf)
{
    LEDC_ARG_CHECK(ledc_conf, "ledc_conf");
    uint32_t speed_mode = ledc_conf->speed_mode;
    int gpio_num = ledc_conf->gpio_num;
    uint32_t ledc_channel = ledc_conf->channel;
    uint32_t timer_select = ledc_conf->timer_sel;
    uint32_t intr_type = ledc_conf->intr_type;
    uint32_t duty = ledc_conf->duty;
    uint32_t hpoint = ledc_conf->hpoint;
    bool output_invert = ledc_conf->flags.output_invert;
    LEDC_ARG_CHECK(ledc_channel < LEDC_CHANNEL_MAX, "ledc_channel");
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "gpio_num");
    LEDC_ARG_CHECK(timer_select < LEDC_TIMER_MAX, "timer_select");
    LEDC_ARG_CHECK(intr_type < LEDC_INTR_MAX, "intr_type");

    periph_module_enable(PERIPH_LEDC_MODULE);
    esp_err_t ret = ESP_OK;

    if (p_ledc_obj[speed_mode] == NULL) {
        p_ledc_obj[speed_mode] = (ledc_obj_t *) heap_caps_calloc(1, sizeof(ledc_obj_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (p_ledc_obj[speed_mode] == NULL) {
            return ESP_ERR_NO_MEM;
        }
        ledc_hal_init(&(p_ledc_obj[speed_mode]->ledc_hal), speed_mode);
    }

    /*set channel parameters*/
    /*   channel parameters decide how the waveform looks like in one period*/
    /*   set channel duty and hpoint value, duty range is (0 ~ ((2 ** duty_resolution) - 1)), max hpoint value is 0xfffff*/
    ledc_set_duty_with_hpoint(speed_mode, ledc_channel, duty, hpoint);
    /*update duty settings*/
    ledc_update_duty(speed_mode, ledc_channel);
    /*bind the channel with the timer*/
    ledc_bind_channel_timer(speed_mode, ledc_channel, timer_select);
    /*set interrupt type*/
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_enable_intr_type(speed_mode, ledc_channel, intr_type);
    portEXIT_CRITICAL(&ledc_spinlock);
    ESP_LOGD(LEDC_TAG, "LEDC_PWM CHANNEL %"PRIu32"|GPIO %02u|Duty %04"PRIu32"|Time %"PRIu32,
             ledc_channel, gpio_num, duty, timer_select);
    /*set LEDC signal in gpio matrix*/
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    gpio_set_level(gpio_num, output_invert);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(gpio_num, ledc_periph_signal[speed_mode].sig_out0_idx + ledc_channel, output_invert, 0);

    return ret;
}

static void _ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    ledc_hal_set_sig_out_en(&(p_ledc_obj[speed_mode]->ledc_hal), channel, true);
    ledc_hal_set_duty_start(&(p_ledc_obj[speed_mode]->ledc_hal), channel, true);
    ledc_ls_channel_update(speed_mode, channel);
}

esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    _ledc_update_duty(speed_mode, channel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_stop(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t idle_level)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_hal_set_idle_level(&(p_ledc_obj[speed_mode]->ledc_hal), channel, idle_level);
    ledc_hal_set_sig_out_en(&(p_ledc_obj[speed_mode]->ledc_hal), channel, false);
    ledc_hal_set_duty_start(&(p_ledc_obj[speed_mode]->ledc_hal), channel, false);
    ledc_ls_channel_update(speed_mode, channel);
    portEXIT_CRITICAL(&ledc_spinlock);
    return ESP_OK;
}

esp_err_t ledc_set_fade(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, ledc_duty_direction_t fade_direction,
                        uint32_t step_num, uint32_t duty_cyle_num, uint32_t duty_scale)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_direction < LEDC_DUTY_DIR_MAX, "fade_direction");
    LEDC_ARG_CHECK(step_num <= LEDC_LL_DUTY_NUM_MAX, "step_num");
    LEDC_ARG_CHECK(duty_cyle_num <= LEDC_LL_DUTY_CYCLE_MAX, "duty_cycle_num");
    LEDC_ARG_CHECK(duty_scale <= LEDC_LL_DUTY_SCALE_MAX, "duty_scale");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    _ledc_fade_hw_acquire(speed_mode, channel);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_duty_config(speed_mode,
                     channel,        //uint32_t chan_num,
                     LEDC_VAL_NO_CHANGE,
                     duty,           //uint32_t duty_val,
                     fade_direction, //uint32_t increase,
                     step_num,       //uint32_t duty_num,
                     duty_cyle_num,  //uint32_t duty_cycle,
                     duty_scale      //uint32_t duty_scale
                    );
    portEXIT_CRITICAL(&ledc_spinlock);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_duty_with_hpoint(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, uint32_t hpoint)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(hpoint <= LEDC_LL_HPOINT_VAL_MAX, "hpoint");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    /* The channel configuration should not be changed before the fade operation is done. */
    _ledc_fade_hw_acquire(speed_mode, channel);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_duty_config(speed_mode,
                     channel,         //uint32_t chan_num,
                     hpoint,          //uint32_t hpoint_val,
                     duty,           //uint32_t duty_val,
                     1,               //uint32_t increase,
                     0,               //uint32_t duty_num,
                     0,               //uint32_t duty_cycle,
                     0                //uint32_t duty_scale
                    );
    portEXIT_CRITICAL(&ledc_spinlock);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    /* The channel configuration should not be changed before the fade operation is done. */
    _ledc_fade_hw_acquire(speed_mode, channel);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_duty_config(speed_mode,
                     channel,         //uint32_t chan_num,
                     LEDC_VAL_NO_CHANGE,
                     duty,           //uint32_t duty_val,
                     1,               //uint32_t increase,
                     0,               //uint32_t duty_num,
                     0,               //uint32_t duty_cycle,
                     0                //uint32_t duty_scale
                    );
    portEXIT_CRITICAL(&ledc_spinlock);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

uint32_t ledc_get_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    uint32_t duty = 0;
    ledc_hal_get_duty(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &duty);
    return duty;
}

int ledc_get_hpoint(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode argument is invalid", LEDC_ERR_VAL);
    LEDC_CHECK(channel < LEDC_CHANNEL_MAX, "channel argument is invalid", LEDC_ERR_VAL);
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    uint32_t hpoint = 0;
    ledc_hal_get_hpoint(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &hpoint);
    return hpoint;
}

esp_err_t ledc_set_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num, uint32_t freq_hz)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_num < LEDC_TIMER_MAX, "timer_num");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    ledc_clk_cfg_t clk_cfg = LEDC_AUTO_CLK;
    uint32_t duty_resolution = 0;
    ledc_hal_get_clk_cfg(&(p_ledc_obj[speed_mode]->ledc_hal), timer_num, &clk_cfg);
    ledc_hal_get_duty_resolution(&(p_ledc_obj[speed_mode]->ledc_hal), timer_num, &duty_resolution);
    return ledc_set_timer_div(speed_mode, timer_num, clk_cfg, freq_hz, duty_resolution);
}

uint32_t ledc_get_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(timer_num < LEDC_TIMER_MAX, "timer_num");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    portENTER_CRITICAL(&ledc_spinlock);
    uint32_t clock_divider = 0;
    uint32_t duty_resolution = 0;
    ledc_clk_cfg_t clk_cfg = LEDC_AUTO_CLK;
    ledc_hal_get_clock_divider(&(p_ledc_obj[speed_mode]->ledc_hal), timer_num, &clock_divider);
    ledc_hal_get_duty_resolution(&(p_ledc_obj[speed_mode]->ledc_hal), timer_num, &duty_resolution);
    ledc_hal_get_clk_cfg(&(p_ledc_obj[speed_mode]->ledc_hal), timer_num, &clk_cfg);
    uint32_t precision = (0x1 << duty_resolution);
    uint32_t src_clk_freq = ledc_get_src_clk_freq(clk_cfg);
    portEXIT_CRITICAL(&ledc_spinlock);
    if (clock_divider == 0) {
        ESP_LOGW(LEDC_TAG, "LEDC timer not configured, call ledc_timer_config to set timer frequency");
        return 0;
    }
    return ((uint64_t) src_clk_freq << 8) / precision / clock_divider;
}

static inline void ledc_calc_fade_end_channel(uint32_t *fade_end_status, uint32_t *channel)
{
    uint32_t i = __builtin_ffs((*fade_end_status)) - 1;
    (*fade_end_status) &= ~(1 << i);
    *channel = i;
}

void IRAM_ATTR ledc_fade_isr(void *arg)
{
    bool cb_yield = false;
    portBASE_TYPE HPTaskAwoken = pdFALSE;
    uint32_t speed_mode = 0;
    uint32_t channel = 0;
    uint32_t intr_status = 0;
    ledc_fade_fsm_t state;

    for (speed_mode = 0; speed_mode < LEDC_SPEED_MODE_MAX; speed_mode++) {
        if (p_ledc_obj[speed_mode] == NULL) {
            continue;
        }
        ledc_hal_get_fade_end_intr_status(&(p_ledc_obj[speed_mode]->ledc_hal), &intr_status);
        while (intr_status) {
            ledc_calc_fade_end_channel(&intr_status, &channel);

            // clear interrupt
            ledc_hal_clear_fade_end_intr_status(&(p_ledc_obj[speed_mode]->ledc_hal), channel);

            if (s_ledc_fade_rec[speed_mode][channel] == NULL) {
                //fade object not initialized yet.
                continue;
            }

            // Switch fade state to ISR_CAL if current state is HW_FADE
            bool already_stopped = false;
            portENTER_CRITICAL_ISR(&ledc_spinlock);
            state = s_ledc_fade_rec[speed_mode][channel]->fsm;
            assert(state != LEDC_FSM_ISR_CAL && state != LEDC_FSM_KILLED_PENDING);
            if (state == LEDC_FSM_HW_FADE) {
                s_ledc_fade_rec[speed_mode][channel]->fsm = LEDC_FSM_ISR_CAL;
            } else if (state == LEDC_FSM_IDLE) {
                // interrupt seen, but has already been stopped by task
                already_stopped = true;
            }
            portEXIT_CRITICAL_ISR(&ledc_spinlock);
            if (already_stopped) {
                continue;
            }

            bool set_to_idle = false;
            int cycle = 0;
            int delta = 0;
            int step = 0;
            int next_duty = 0;
            uint32_t duty_cur = 0;
            ledc_hal_get_duty(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &duty_cur);
            uint32_t duty_tar = s_ledc_fade_rec[speed_mode][channel]->target_duty;
            int scale = s_ledc_fade_rec[speed_mode][channel]->scale;
            if (duty_cur == duty_tar || scale == 0) {
                // Target duty has reached
                set_to_idle = true;
            } else {
                // Calculate new duty config parameters
                delta = (s_ledc_fade_rec[speed_mode][channel]->direction == LEDC_DUTY_DIR_DECREASE) ?
                            (duty_cur - duty_tar) : (duty_tar - duty_cur);
                if (delta > scale) {
                    next_duty = duty_cur;
                    step = (delta / scale > LEDC_DUTY_NUM_MAX) ? LEDC_DUTY_NUM_MAX : (delta / scale);
                    cycle = s_ledc_fade_rec[speed_mode][channel]->cycle_num;
                } else {
                    next_duty = duty_tar;
                    step = 1;
                    cycle = 1;
                    scale = 0;
                }
            }

            bool finished = false;
            portENTER_CRITICAL_ISR(&ledc_spinlock);
            state = s_ledc_fade_rec[speed_mode][channel]->fsm;
            assert(state != LEDC_FSM_IDLE && state != LEDC_FSM_HW_FADE);
            if (set_to_idle || state == LEDC_FSM_KILLED_PENDING) {
                // Either fade has completed or has been killed, skip HW duty config
                finished = true;
                s_ledc_fade_rec[speed_mode][channel]->fsm = LEDC_FSM_IDLE;
            } else if (state == LEDC_FSM_ISR_CAL) {
                // Loading new fade to start
                ledc_duty_config(speed_mode,
                                 channel,
                                 LEDC_VAL_NO_CHANGE,
                                 next_duty,
                                 s_ledc_fade_rec[speed_mode][channel]->direction,
                                 step,
                                 cycle,
                                 scale);
                s_ledc_fade_rec[speed_mode][channel]->fsm = LEDC_FSM_HW_FADE;
                ledc_hal_set_duty_start(&(p_ledc_obj[speed_mode]->ledc_hal), channel, true);
            }
            portEXIT_CRITICAL_ISR(&ledc_spinlock);
            if (finished) {
                xSemaphoreGiveFromISR(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem, &HPTaskAwoken);
                ledc_cb_t fade_cb = s_ledc_fade_rec[speed_mode][channel]->ledc_fade_callback;
                if (fade_cb) {
                    ledc_cb_param_t param = {
                        .event = LEDC_FADE_END_EVT,
                        .speed_mode = speed_mode,
                        .channel = channel,
                        .duty = duty_cur
                    };
                    cb_yield |= fade_cb(&param, s_ledc_fade_rec[speed_mode][channel]->cb_user_arg);
                }
            }
        }
    }
    if (HPTaskAwoken == pdTRUE || cb_yield) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t ledc_fade_channel_deinit(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if (s_ledc_fade_rec[speed_mode][channel]) {
        if (s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux) {
            vSemaphoreDelete(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux);
            s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux = NULL;
        }
        if (s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem) {
            vSemaphoreDelete(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem);
            s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem = NULL;
        }
        free(s_ledc_fade_rec[speed_mode][channel]);
        s_ledc_fade_rec[speed_mode][channel] = NULL;
    }
    return ESP_OK;
}

static esp_err_t ledc_fade_channel_init_check(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    if (s_ledc_fade_isr_handle == NULL) {
        ESP_LOGE(LEDC_TAG, "Fade service not installed, call ledc_fade_func_install");
        return ESP_FAIL;
    }
    if (s_ledc_fade_rec[speed_mode][channel] == NULL) {
#if CONFIG_SPIRAM_USE_MALLOC
        s_ledc_fade_rec[speed_mode][channel] = (ledc_fade_t *) heap_caps_calloc(1, sizeof(ledc_fade_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (s_ledc_fade_rec[speed_mode][channel] == NULL) {
            ledc_fade_channel_deinit(speed_mode, channel);
            return ESP_ERR_NO_MEM;
        }

        memset(&s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem_storage, 0, sizeof(StaticQueue_t));
        s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem = xSemaphoreCreateBinaryStatic(&s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem_storage);
#else
        s_ledc_fade_rec[speed_mode][channel] = (ledc_fade_t *) calloc(1, sizeof(ledc_fade_t));
        if (s_ledc_fade_rec[speed_mode][channel] == NULL) {
            ledc_fade_channel_deinit(speed_mode, channel);
            return ESP_ERR_NO_MEM;
        }
        s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem = xSemaphoreCreateBinary();
#endif
        s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux = xSemaphoreCreateMutex();
        xSemaphoreGive(s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem);
        s_ledc_fade_rec[speed_mode][channel]->fsm = LEDC_FSM_IDLE;
    }
    if (s_ledc_fade_rec[speed_mode][channel]
            && s_ledc_fade_rec[speed_mode][channel]->ledc_fade_mux
            && s_ledc_fade_rec[speed_mode][channel]->ledc_fade_sem) {
        return ESP_OK;
    } else {
        ledc_fade_channel_deinit(speed_mode, channel);
        return ESP_FAIL;
    }
}

static esp_err_t _ledc_set_fade_with_step(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int scale, int cycle_num)
{
    portENTER_CRITICAL(&ledc_spinlock);
    uint32_t duty_cur = 0;
    ledc_hal_get_duty(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &duty_cur);
    // When duty == max_duty, meanwhile, if scale == 1 and fade_down == 1, counter would overflow.
    if (duty_cur == ledc_get_max_duty(speed_mode, channel)) {
        duty_cur -= 1;
    }
    s_ledc_fade_rec[speed_mode][channel]->speed_mode = speed_mode;
    s_ledc_fade_rec[speed_mode][channel]->target_duty = target_duty;
    s_ledc_fade_rec[speed_mode][channel]->cycle_num = cycle_num;
    s_ledc_fade_rec[speed_mode][channel]->scale = scale;
    int step_num = 0;
    int dir = LEDC_DUTY_DIR_DECREASE;
    if (scale > 0) {
        if (duty_cur > target_duty) {
            s_ledc_fade_rec[speed_mode][channel]->direction = LEDC_DUTY_DIR_DECREASE;
            step_num = (duty_cur - target_duty) / scale;
            step_num = step_num > LEDC_DUTY_NUM_MAX ? LEDC_DUTY_NUM_MAX : step_num;
        } else {
            s_ledc_fade_rec[speed_mode][channel]->direction = LEDC_DUTY_DIR_INCREASE;
            dir = LEDC_DUTY_DIR_INCREASE;
            step_num = (target_duty - duty_cur) / scale;
            step_num = step_num > LEDC_DUTY_NUM_MAX ? LEDC_DUTY_NUM_MAX : step_num;
        }
    }

    portEXIT_CRITICAL(&ledc_spinlock);
    if (scale > 0 && step_num > 0) {
        portENTER_CRITICAL(&ledc_spinlock);
        ledc_duty_config(speed_mode, channel, LEDC_VAL_NO_CHANGE, duty_cur, dir, step_num, cycle_num, scale);
        portEXIT_CRITICAL(&ledc_spinlock);
        ESP_LOGD(LEDC_TAG, "cur duty: %"PRIu32"; target: %"PRIu32", step: %d, cycle: %d; scale: %d; dir: %d\n",
                 duty_cur, target_duty, step_num, cycle_num, scale, dir);
    } else {
        portENTER_CRITICAL(&ledc_spinlock);
        ledc_duty_config(speed_mode, channel, LEDC_VAL_NO_CHANGE, target_duty, dir, 0, 1, 0);
        portEXIT_CRITICAL(&ledc_spinlock);
        ESP_LOGD(LEDC_TAG, "Set to target duty: %"PRIu32, target_duty);
    }
    return ESP_OK;
}

static esp_err_t _ledc_set_fade_with_time(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms)
{
    ledc_timer_t timer_sel;
    uint32_t duty_cur = 0;
    ledc_hal_get_channel_timer(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &timer_sel);
    ledc_hal_get_duty(&(p_ledc_obj[speed_mode]->ledc_hal), channel, &duty_cur);
    uint32_t freq = ledc_get_freq(speed_mode, timer_sel);
    uint32_t duty_delta = target_duty > duty_cur ? target_duty - duty_cur : duty_cur - target_duty;

    if (duty_delta == 0) {
        return _ledc_set_fade_with_step(speed_mode, channel, target_duty, 0, 0);
    }
    uint32_t total_cycles = max_fade_time_ms * freq / 1000;
    if (total_cycles == 0) {
        ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_FAST_STR);
        return _ledc_set_fade_with_step(speed_mode, channel, target_duty, 0, 0);
    }
    int scale, cycle_num;
    if (total_cycles > duty_delta) {
        scale = 1;
        cycle_num = total_cycles / duty_delta;
        if (cycle_num > LEDC_LL_DUTY_NUM_MAX) {
            ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_SLOW_STR);
            cycle_num = LEDC_LL_DUTY_NUM_MAX;
        }
    } else {
        cycle_num = 1;
        scale = duty_delta / total_cycles;
        if (scale > LEDC_LL_DUTY_SCALE_MAX) {
            ESP_LOGW(LEDC_TAG, LEDC_FADE_TOO_FAST_STR);
            scale = LEDC_LL_DUTY_SCALE_MAX;
        }
    }
    return _ledc_set_fade_with_step(speed_mode, channel, target_duty, scale, cycle_num);
}

static void _ledc_fade_start(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_fade_mode_t fade_mode)
{
    ledc_fade_t *fade = s_ledc_fade_rec[speed_mode][channel];
    fade->mode = fade_mode;
    // Clear interrupt status of channel
    ledc_hal_clear_fade_end_intr_status(&(p_ledc_obj[speed_mode]->ledc_hal), channel);
    // Enable interrupt for channel
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_enable_intr_type(speed_mode, channel, LEDC_INTR_FADE_END);
    // Set fade state to HW_FADE state for starting the fade
    assert(fade->fsm == LEDC_FSM_IDLE);
    fade->fsm = LEDC_FSM_HW_FADE;
    portEXIT_CRITICAL(&ledc_spinlock);
    // Trigger the fade
    ledc_update_duty(speed_mode, channel);
    if (fade_mode == LEDC_FADE_WAIT_DONE) {
        // Waiting for fade done
        _ledc_fade_hw_acquire(speed_mode, channel);
        // Release hardware to support next time fade configure
        _ledc_fade_hw_release(speed_mode, channel);
    }
}

esp_err_t ledc_set_fade_with_time(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);

    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_time(speed_mode, channel, target_duty, max_fade_time_ms);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_fade_with_step(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t scale, uint32_t cycle_num)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK((scale > 0) && (scale <= LEDC_LL_DUTY_SCALE_MAX), "fade scale");
    LEDC_ARG_CHECK((cycle_num > 0) && (cycle_num <= LEDC_LL_DUTY_CYCLE_MAX), "cycle_num");
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);

    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_step(speed_mode, channel, target_duty, scale, cycle_num);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_fade_start(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_fade_mode_t fade_mode)
{
    LEDC_CHECK(s_ledc_fade_rec[speed_mode][channel] != NULL, LEDC_FADE_SERVICE_ERR_STR, ESP_ERR_INVALID_STATE);
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_mode < LEDC_FADE_MAX, "fade_mode");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_fade_start(speed_mode, channel, fade_mode);
    return ESP_OK;
}

// ESP32 does not support this functionality, fade cannot be overwritten with new duty config
#if SOC_LEDC_SUPPORT_FADE_STOP
esp_err_t ledc_fade_stop(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK , LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    ledc_fade_t *fade = s_ledc_fade_rec[speed_mode][channel];
    ledc_fade_fsm_t state = fade->fsm;
    bool wait_for_idle = false;
    assert(state != LEDC_FSM_KILLED_PENDING);
    if (state == LEDC_FSM_IDLE) {
        // if there is no fade going on, do nothing
        return ESP_OK;
    }
    // Fade state is either HW_FADE or ISR_CAL (there is a fade in process)
    portENTER_CRITICAL(&ledc_spinlock);
    // Disable ledc channel interrupt first
    ledc_enable_intr_type(speed_mode, channel, LEDC_INTR_DISABLE);
    // Config duty to the duty cycle at this moment
    uint32_t duty_cur = ledc_get_duty(speed_mode, channel);
    ledc_duty_config(speed_mode,
                     channel,              //uint32_t chan_num,
                     LEDC_VAL_NO_CHANGE,
                     duty_cur,             //uint32_t duty_val,
                     1,                    //uint32_t increase,
                     0,                    //uint32_t duty_num,
                     0,                    //uint32_t duty_cycle,
                     0                     //uint32_t duty_scale
                     );
    _ledc_update_duty(speed_mode, channel);
    state = fade->fsm;
    assert(state != LEDC_FSM_IDLE && state != LEDC_FSM_KILLED_PENDING);
    if (state == LEDC_FSM_HW_FADE) {
        fade->fsm = LEDC_FSM_IDLE;
    } else if (state == LEDC_FSM_ISR_CAL) {
        fade->fsm = LEDC_FSM_KILLED_PENDING;
        wait_for_idle = true;
    }
    portEXIT_CRITICAL(&ledc_spinlock);
    if (wait_for_idle) {
        // Wait for ISR return, which gives the semaphore and switchs state to IDLE
        _ledc_fade_hw_acquire(speed_mode, channel);
        assert(fade->fsm == LEDC_FSM_IDLE);
    }
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}
#endif

esp_err_t ledc_fade_func_install(int intr_alloc_flags)
{
    //OR intr_alloc_flags with ESP_INTR_FLAG_IRAM because the fade isr is in IRAM
    return ledc_isr_register(ledc_fade_isr, NULL, intr_alloc_flags | ESP_INTR_FLAG_IRAM, &s_ledc_fade_isr_handle);
}

void ledc_fade_func_uninstall(void)
{
    if (s_ledc_fade_isr_handle) {
        esp_intr_free(s_ledc_fade_isr_handle);
        s_ledc_fade_isr_handle = NULL;
    }
    int channel, mode;
    for (mode = 0; mode < LEDC_SPEED_MODE_MAX; mode++) {
        for (channel = 0; channel < LEDC_CHANNEL_MAX; channel++) {
            ledc_fade_channel_deinit(mode, channel);
        }
    }
    return;
}

esp_err_t ledc_cb_register(ledc_mode_t speed_mode, ledc_channel_t channel, ledc_cbs_t *cbs, void *user_arg)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    s_ledc_fade_rec[speed_mode][channel]->ledc_fade_callback = cbs->fade_cb;
    s_ledc_fade_rec[speed_mode][channel]->cb_user_arg = user_arg;
    return ESP_OK;
}

/*
 * The functions below are thread-safe version of APIs for duty and fade control.
 * These APIs can be called from different tasks.
 */
esp_err_t ledc_set_duty_and_update(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty, uint32_t hpoint)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    LEDC_ARG_CHECK(hpoint <= LEDC_LL_HPOINT_VAL_MAX, "hpoint");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    _ledc_fade_hw_acquire(speed_mode, channel);
    portENTER_CRITICAL(&ledc_spinlock);
    ledc_duty_config(speed_mode, channel, hpoint, duty, 1, 0, 0, 0);
    _ledc_update_duty(speed_mode, channel);
    portEXIT_CRITICAL(&ledc_spinlock);
    _ledc_fade_hw_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_fade_time_and_start(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t max_fade_time_ms, ledc_fade_mode_t fade_mode)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_mode < LEDC_FADE_MAX, "fade_mode");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    _ledc_op_lock_acquire(speed_mode, channel);
    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_time(speed_mode, channel, target_duty, max_fade_time_ms);
    _ledc_fade_start(speed_mode, channel, fade_mode);
    _ledc_op_lock_release(speed_mode, channel);
    return ESP_OK;
}

esp_err_t ledc_set_fade_step_and_start(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, uint32_t scale, uint32_t cycle_num, ledc_fade_mode_t fade_mode)
{
    LEDC_ARG_CHECK(speed_mode < LEDC_SPEED_MODE_MAX, "speed_mode");
    LEDC_ARG_CHECK(channel < LEDC_CHANNEL_MAX, "channel");
    LEDC_ARG_CHECK(fade_mode < LEDC_FADE_MAX, "fade_mode");
    LEDC_CHECK(p_ledc_obj[speed_mode] != NULL, LEDC_NOT_INIT, ESP_ERR_INVALID_STATE);
    LEDC_CHECK(ledc_fade_channel_init_check(speed_mode, channel) == ESP_OK, LEDC_FADE_INIT_ERROR_STR, ESP_FAIL);
    LEDC_ARG_CHECK((scale > 0) && (scale <= LEDC_LL_DUTY_SCALE_MAX), "fade scale");
    LEDC_ARG_CHECK((cycle_num > 0) && (cycle_num <= LEDC_LL_DUTY_CYCLE_MAX), "cycle_num");
    LEDC_ARG_CHECK(target_duty <= ledc_get_max_duty(speed_mode, channel), "target_duty");
    _ledc_op_lock_acquire(speed_mode, channel);
    _ledc_fade_hw_acquire(speed_mode, channel);
    _ledc_set_fade_with_step(speed_mode, channel, target_duty, scale, cycle_num);
    _ledc_fade_start(speed_mode, channel, fade_mode);
    _ledc_op_lock_release(speed_mode, channel);
    return ESP_OK;
}
