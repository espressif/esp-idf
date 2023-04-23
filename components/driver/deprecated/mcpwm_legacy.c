/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_rom_gpio.h"
#include "esp_intr_alloc.h"
#include "soc/mcpwm_periph.h"
#include "hal/mcpwm_hal.h"
#include "hal/gpio_hal.h"
#include "hal/mcpwm_ll.h"
#include "driver/mcpwm_types_legacy.h"
#include "driver/gpio.h"
#include "esp_private/periph_ctrl.h"
#include "esp_clk_tree.h"
#include "esp_private/esp_clk.h"

static const char *TAG = "mcpwm(legacy)";

_Static_assert(MCPWM_UNIT_MAX == SOC_MCPWM_GROUPS, "MCPWM unit number not equal to chip capabilities");

#define MCPWM_DRIVER_INIT_ERROR "MCPWM DRIVER NOT INITIALIZED"
#define MCPWM_GROUP_NUM_ERROR   "MCPWM GROUP NUM ERROR"
#define MCPWM_PRESCALE_ERROR    "MCPWM PRESCALE ERROR"
#define MCPWM_TIMER_ERROR       "MCPWM TIMER NUM ERROR"
#define MCPWM_CAPTURE_ERROR     "MCPWM CAPTURE NUM ERROR"
#define MCPWM_PARAM_ADDR_ERROR  "MCPWM PARAM ADDR ERROR"
#define MCPWM_DUTY_TYPE_ERROR   "MCPWM DUTY TYPE ERROR"
#define MCPWM_GPIO_ERROR        "MCPWM GPIO NUM ERROR"
#define MCPWM_GEN_ERROR         "MCPWM GENERATOR ERROR"
#define MCPWM_DT_ERROR          "MCPWM DEADTIME TYPE ERROR"
#define MCPWM_CAP_EXIST_ERROR   "MCPWM USER CAP INT SERVICE ALREADY EXISTS"

#ifdef CONFIG_MCPWM_ISR_IRAM_SAFE
#define MCPWM_ISR_ATTR     IRAM_ATTR
#define MCPWM_INTR_FLAG    ESP_INTR_FLAG_IRAM
#else
#define MCPWM_ISR_ATTR
#define MCPWM_INTR_FLAG  0
#endif

// Note: we can't modify the default MCPWM group resolution once it's determined
// otherwise we may break user's existing code which configures the dead-time based on this resolution, see `mcpwm_deadtime_enable`
#if CONFIG_IDF_TARGET_ESP32H2
#define MCPWM_DEFAULT_GROUP_CLK_RESOLUTION_HZ (12 * 1000 * 1000)
#else
#define MCPWM_DEFAULT_GROUP_CLK_RESOLUTION_HZ (10 * 1000 * 1000)
#endif

// Preset MCPWM Timer clock resolution (1MHz)
#define MCPWM_DEFAULT_TIMER_CLK_RESOLUTION_HZ (1 * 1000 * 1000)

ESP_STATIC_ASSERT(SOC_MCPWM_OPERATORS_PER_GROUP >= SOC_MCPWM_TIMERS_PER_GROUP, "This driver assumes the timer num equals to the operator num.");
ESP_STATIC_ASSERT(SOC_MCPWM_COMPARATORS_PER_OPERATOR >= SOC_MCPWM_GENERATORS_PER_OPERATOR, "This driver assumes the generator num equals to the generator num.");
ESP_STATIC_ASSERT(SOC_MCPWM_GENERATORS_PER_OPERATOR == 2, "This driver assumes the generator num equals to 2.");

#define MCPWM_TIMER_ID_CHECK(mcpwm_num, timer_num)                                                                  \
    do {                                                                                                            \
        ESP_RETURN_ON_FALSE((mcpwm_num) < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);       \
        ESP_RETURN_ON_FALSE((timer_num) < SOC_MCPWM_TIMERS_PER_GROUP, ESP_ERR_INVALID_ARG, TAG, MCPWM_TIMER_ERROR); \
    } while (0)

#define MCPWM_TIMER_CHECK(mcpwm_num, timer_num)                                                               \
    do {                                                                                                      \
        MCPWM_TIMER_ID_CHECK(mcpwm_num, timer_num);                                                           \
        ESP_RETURN_ON_FALSE(context[mcpwm_num].hal.dev, ESP_ERR_INVALID_STATE, TAG, MCPWM_DRIVER_INIT_ERROR); \
    } while (0)

#define MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen)                                             \
    do {                                                                                       \
        MCPWM_TIMER_CHECK(mcpwm_num, timer_num);                                               \
        ESP_RETURN_ON_FALSE((gen) < MCPWM_GEN_MAX, ESP_ERR_INVALID_ARG, TAG, MCPWM_GEN_ERROR); \
    } while (0)

typedef struct {
    cap_isr_cb_t fn;   // isr function
    void *args;      // isr function args
} cap_isr_func_t;

typedef struct {
    mcpwm_hal_context_t hal;
    portMUX_TYPE spinlock;
    _lock_t mutex_lock;
    const int group_id;
    int group_resolution_hz;
    int timer_resolution_hz[SOC_MCPWM_TIMERS_PER_GROUP];
    intr_handle_t mcpwm_intr_handle;  // handler for ISR register, one per MCPWM group
    cap_isr_func_t cap_isr_func[SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER]; // handler for ISR callback, one for each cap ch
} mcpwm_context_t;

static mcpwm_context_t context[SOC_MCPWM_GROUPS] = {
    [0] = {
        .hal = {MCPWM_LL_GET_HW(0)},
        .spinlock = portMUX_INITIALIZER_UNLOCKED,
        .group_id = 0,
    },
#if SOC_MCPWM_GROUPS > 1
    [1] = {
        .hal = {MCPWM_LL_GET_HW(1)},
        .spinlock = portMUX_INITIALIZER_UNLOCKED,
        .group_id = 1,
    },
#endif
};

typedef void (*mcpwm_ll_gen_set_event_action_t)(mcpwm_dev_t *mcpwm, int op, int gen, int action);

static inline void mcpwm_critical_enter(mcpwm_unit_t mcpwm_num)
{
    portENTER_CRITICAL(&context[mcpwm_num].spinlock);
}

static inline void mcpwm_critical_exit(mcpwm_unit_t mcpwm_num)
{
    portEXIT_CRITICAL(&context[mcpwm_num].spinlock);
}

static inline void mcpwm_mutex_lock(mcpwm_unit_t mcpwm_num)
{
    _lock_acquire(&context[mcpwm_num].mutex_lock);
}

static inline void mcpwm_mutex_unlock(mcpwm_unit_t mcpwm_num)
{
    _lock_release(&context[mcpwm_num].mutex_lock);
}

esp_err_t mcpwm_gpio_init(mcpwm_unit_t mcpwm_num, mcpwm_io_signals_t io_signal, int gpio_num)
{
    if (gpio_num < 0) { // ignore on minus gpio number
        return ESP_OK;
    }

    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, MCPWM_GPIO_ERROR);

    if (io_signal <= MCPWM2B) { // Generator output signal
        ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), ESP_ERR_INVALID_ARG, TAG, MCPWM_GPIO_ERROR);
        gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
        int operator_id = io_signal / 2;
        int generator_id = io_signal % 2;
        esp_rom_gpio_connect_out_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].operators[operator_id].generators[generator_id].pwm_sig, 0, 0);
    } else if (io_signal <= MCPWM_SYNC_2) { // External sync input signal
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        int gpio_sync_id = io_signal - MCPWM_SYNC_0;
        esp_rom_gpio_connect_in_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].gpio_synchros[gpio_sync_id].sync_sig, 0);
    } else if (io_signal <= MCPWM_FAULT_2) { // Fault input signal
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        int fault_id = io_signal - MCPWM_FAULT_0;
        esp_rom_gpio_connect_in_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].gpio_faults[fault_id].fault_sig, 0);
    } else if (io_signal >= MCPWM_CAP_0 && io_signal <= MCPWM_CAP_2) { // Capture input signal
        gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
        int capture_id = io_signal - MCPWM_CAP_0;
        esp_rom_gpio_connect_in_signal(gpio_num, mcpwm_periph_signals.groups[mcpwm_num].captures[capture_id].cap_sig, 0);
    }
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
    return ESP_OK;
}

esp_err_t mcpwm_set_pin(mcpwm_unit_t mcpwm_num, const mcpwm_pin_config_t *mcpwm_pin)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    mcpwm_gpio_init(mcpwm_num, MCPWM0A, mcpwm_pin->mcpwm0a_out_num);    //MCPWM0A
    mcpwm_gpio_init(mcpwm_num, MCPWM0B, mcpwm_pin->mcpwm0b_out_num);    //MCPWM0B
    mcpwm_gpio_init(mcpwm_num, MCPWM1A, mcpwm_pin->mcpwm1a_out_num);    //MCPWM1A
    mcpwm_gpio_init(mcpwm_num, MCPWM1B, mcpwm_pin->mcpwm1b_out_num);    //MCPWM1B
    mcpwm_gpio_init(mcpwm_num, MCPWM2A, mcpwm_pin->mcpwm2a_out_num);    //MCPWM2A
    mcpwm_gpio_init(mcpwm_num, MCPWM2B, mcpwm_pin->mcpwm2b_out_num);    //MCPWM2B
    mcpwm_gpio_init(mcpwm_num, MCPWM_SYNC_0,  mcpwm_pin->mcpwm_sync0_in_num);   //SYNC0
    mcpwm_gpio_init(mcpwm_num, MCPWM_SYNC_1,  mcpwm_pin->mcpwm_sync1_in_num);   //SYNC1
    mcpwm_gpio_init(mcpwm_num, MCPWM_SYNC_2,  mcpwm_pin->mcpwm_sync2_in_num);   //SYNC2
    mcpwm_gpio_init(mcpwm_num, MCPWM_FAULT_0, mcpwm_pin->mcpwm_fault0_in_num);  //FAULT0
    mcpwm_gpio_init(mcpwm_num, MCPWM_FAULT_1, mcpwm_pin->mcpwm_fault1_in_num);  //FAULT1
    mcpwm_gpio_init(mcpwm_num, MCPWM_FAULT_2, mcpwm_pin->mcpwm_fault2_in_num);  //FAULT2
    mcpwm_gpio_init(mcpwm_num, MCPWM_CAP_0,   mcpwm_pin->mcpwm_cap0_in_num);    //CAP0
    mcpwm_gpio_init(mcpwm_num, MCPWM_CAP_1,   mcpwm_pin->mcpwm_cap1_in_num);    //CAP1
    mcpwm_gpio_init(mcpwm_num, MCPWM_CAP_2,   mcpwm_pin->mcpwm_cap2_in_num);    //CAP2
    return ESP_OK;
}

esp_err_t mcpwm_start(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_timer_set_start_stop_command(context[mcpwm_num].hal.dev, timer_num, MCPWM_TIMER_START_NO_STOP);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_timer_set_start_stop_command(context[mcpwm_num].hal.dev, timer_num, MCPWM_TIMER_STOP_EMPTY);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

static inline uint32_t mcpwm_group_get_resolution(mcpwm_unit_t mcpwm_num)
{
    if (context[mcpwm_num].group_resolution_hz == 0) {
        context[mcpwm_num].group_resolution_hz = MCPWM_DEFAULT_GROUP_CLK_RESOLUTION_HZ;
    }
    return context[mcpwm_num].group_resolution_hz;
}

static inline uint32_t mcpwm_timer_get_resolution(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    if (context[mcpwm_num].timer_resolution_hz[timer_num] == 0) {
        context[mcpwm_num].timer_resolution_hz[timer_num] = MCPWM_DEFAULT_TIMER_CLK_RESOLUTION_HZ;
    }
    return context[mcpwm_num].timer_resolution_hz[timer_num];
}

esp_err_t mcpwm_group_set_resolution(mcpwm_unit_t mcpwm_num, uint32_t resolution)
{
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    uint32_t clk_src_hz = 0;
    esp_clk_tree_src_get_freq_hz(MCPWM_TIMER_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_hz);

    int pre_scale_temp = clk_src_hz / resolution;
    ESP_RETURN_ON_FALSE(pre_scale_temp >= 1, ESP_ERR_INVALID_ARG, TAG, "invalid resolution");
    context[mcpwm_num].group_resolution_hz = clk_src_hz / pre_scale_temp;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_group_set_clock_prescale(hal->dev, pre_scale_temp);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_timer_set_resolution(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint32_t resolution)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    uint32_t group_resolution = mcpwm_group_get_resolution(mcpwm_num);

    int pre_scale_temp = group_resolution / resolution;
    ESP_RETURN_ON_FALSE(pre_scale_temp >= 1, ESP_ERR_INVALID_ARG, TAG, "invalid resolution");
    context[mcpwm_num].timer_resolution_hz[timer_num] = context[mcpwm_num].group_resolution_hz / pre_scale_temp;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_timer_set_clock_prescale(hal->dev, timer_num, pre_scale_temp);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_set_frequency(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint32_t frequency)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);

    mcpwm_ll_timer_update_period_at_once(hal->dev, timer_num);
    uint32_t previous_peak = mcpwm_ll_timer_get_peak(hal->dev, timer_num, false);
    uint32_t timer_resolution = mcpwm_timer_get_resolution(mcpwm_num, timer_num);
    uint32_t new_peak = timer_resolution / frequency;
    mcpwm_ll_timer_set_peak(hal->dev, timer_num, new_peak, false);
    // keep the duty cycle unchanged
    float scale = ((float)new_peak) / previous_peak;
    // the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
    uint32_t previous_cmp_a = mcpwm_ll_operator_get_compare_value(hal->dev, op, 0);
    uint32_t previous_cmp_b = mcpwm_ll_operator_get_compare_value(hal->dev, op, 1);
    // update compare value immediately
    mcpwm_ll_operator_update_compare_at_once(hal->dev, op, 0);
    mcpwm_ll_operator_update_compare_at_once(hal->dev, op, 1);
    mcpwm_ll_operator_set_compare_value(hal->dev, op, 0, (uint32_t)(previous_cmp_a * scale));
    mcpwm_ll_operator_set_compare_value(hal->dev, op, 1, (uint32_t)(previous_cmp_b * scale));

    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_set_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen, float duty)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    //the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
    const int cmp = gen;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    uint32_t set_duty = mcpwm_ll_timer_get_peak(hal->dev, timer_num, false) * duty / 100;
    mcpwm_ll_operator_set_compare_value(hal->dev, op, cmp, set_duty);
    mcpwm_ll_operator_enable_update_compare_on_tez(hal->dev, op, cmp, true);
    mcpwm_ll_operator_enable_update_compare_on_tep(hal->dev, op, cmp, true);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_set_duty_in_us(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen, uint32_t duty_in_us)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    //the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
    const int cmp = gen;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    // to avid multiplication overflow, use uint64_t here
    uint64_t timer_resolution = mcpwm_timer_get_resolution(mcpwm_num, timer_num);
    uint64_t compare_val = timer_resolution * duty_in_us / 1000000;
    mcpwm_ll_operator_set_compare_value(hal->dev, op, cmp, (uint32_t)compare_val);
    mcpwm_ll_operator_enable_update_compare_on_tez(hal->dev, op, cmp, true);
    mcpwm_ll_operator_enable_update_compare_on_tep(hal->dev, op, cmp, true);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_set_duty_type(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen,
                              mcpwm_duty_type_t duty_type)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_GEN_CHECK(mcpwm_num, timer_num, gen);
    ESP_RETURN_ON_FALSE(duty_type < MCPWM_DUTY_MODE_MAX, ESP_ERR_INVALID_ARG, TAG, MCPWM_DUTY_TYPE_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    //the driver currently always use the comparator A for PWMxA output, and comparator B for PWMxB output
    mcpwm_critical_enter(mcpwm_num);
    switch (mcpwm_ll_timer_get_count_mode(hal->dev, timer_num)) {
    case MCPWM_TIMER_COUNT_MODE_UP:
        if (duty_type == MCPWM_DUTY_MODE_0) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_KEEP);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_DUTY_MODE_1) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_NO_CHANGE);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_HIGH);
        } else if (duty_type == MCPWM_DUTY_MODE_FORCE_LOW) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_DUTY_MODE_FORCE_HIGH) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_HIGH);
        }
        break;
    case MCPWM_TIMER_COUNT_MODE_DOWN:
        if (duty_type == MCPWM_DUTY_MODE_0) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_NO_CHANGE);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_HIGH);
        } else if (duty_type == MCPWM_DUTY_MODE_1) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_NO_CHANGE);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_DUTY_MODE_FORCE_LOW) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_DUTY_MODE_FORCE_HIGH) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_HIGH);
        }
        break;
    case MCPWM_TIMER_COUNT_MODE_UP_DOWN:
        if (duty_type == MCPWM_DUTY_MODE_0) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_HIGH);
        } else if (duty_type == MCPWM_DUTY_MODE_1) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_DUTY_MODE_FORCE_LOW) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_LOW);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_LOW);
        } else if (duty_type == MCPWM_DUTY_MODE_FORCE_HIGH) {
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_timer_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_DOWN, gen, MCPWM_ACTION_FORCE_HIGH);
            mcpwm_ll_generator_set_action_on_compare_event(hal->dev, op, gen, MCPWM_TIMER_DIRECTION_UP, gen, MCPWM_ACTION_FORCE_HIGH);
        }
        break;
    default:
        break;
    }
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_init(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_config_t *mcpwm_conf)
{
    const int op = timer_num;
    MCPWM_TIMER_ID_CHECK(mcpwm_num, op);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    periph_module_enable(mcpwm_periph_signals.groups[mcpwm_num].module);
    mcpwm_hal_init_config_t config = {
        .group_id = mcpwm_num
    };
    mcpwm_hal_init(hal, &config);

    uint32_t clk_src_hz = 0;
    esp_clk_tree_src_get_freq_hz(MCPWM_TIMER_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_hz);
    uint32_t group_resolution = mcpwm_group_get_resolution(mcpwm_num);
    uint32_t timer_resolution = mcpwm_timer_get_resolution(mcpwm_num, timer_num);
    uint32_t group_pre_scale = clk_src_hz / group_resolution;
    uint32_t timer_pre_scale = group_resolution / timer_resolution;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_group_enable_clock(hal->dev, true);
    mcpwm_ll_group_set_clock_source(hal->dev, (soc_module_clk_t)MCPWM_CAPTURE_CLK_SRC_DEFAULT);
    mcpwm_ll_group_set_clock_prescale(hal->dev, group_pre_scale);
    mcpwm_ll_timer_set_clock_prescale(hal->dev, timer_num, timer_pre_scale);
    mcpwm_ll_timer_set_count_mode(hal->dev, timer_num, mcpwm_conf->counter_mode);
    mcpwm_ll_timer_update_period_at_once(hal->dev, timer_num);
    mcpwm_ll_timer_set_peak(hal->dev, timer_num, timer_resolution / mcpwm_conf->frequency, false);
    mcpwm_ll_operator_connect_timer(hal->dev, timer_num, timer_num); //the driver currently always use the timer x for operator x
    mcpwm_critical_exit(mcpwm_num);

    mcpwm_set_duty(mcpwm_num, timer_num, 0, mcpwm_conf->cmpr_a);
    mcpwm_set_duty(mcpwm_num, timer_num, 1, mcpwm_conf->cmpr_b);
    mcpwm_set_duty_type(mcpwm_num, timer_num, 0, mcpwm_conf->duty_mode);
    mcpwm_set_duty_type(mcpwm_num, timer_num, 1, mcpwm_conf->duty_mode);
    mcpwm_start(mcpwm_num, timer_num);

    return ESP_OK;
}

uint32_t mcpwm_get_frequency(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    if (mcpwm_num >= MCPWM_UNIT_MAX || timer_num >= MCPWM_TIMER_MAX) {
        ESP_LOGE(TAG, "Invalid MCPWM timer instance");
        return 0;
    }

    uint32_t timer_resolution = mcpwm_timer_get_resolution(mcpwm_num, timer_num);

    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_critical_enter(mcpwm_num);
    uint32_t peak = mcpwm_ll_timer_get_peak(hal->dev, timer_num, false);
    uint32_t freq = timer_resolution / peak;
    mcpwm_critical_exit(mcpwm_num);
    return freq;
}

float mcpwm_get_duty(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen)
{
    if (mcpwm_num >= MCPWM_UNIT_MAX || timer_num >= MCPWM_TIMER_MAX || gen >= MCPWM_GEN_MAX) {
        ESP_LOGE(TAG, "Invalid MCPWM generator instance");
        return 0;
    }
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_critical_enter(mcpwm_num);
    float duty = 100.0 * mcpwm_ll_operator_get_compare_value(hal->dev, op, gen) / mcpwm_ll_timer_get_peak(hal->dev, timer_num, false);
    mcpwm_critical_exit(mcpwm_num);
    return duty;
}

uint32_t mcpwm_get_duty_in_us(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_operator_t gen)
{
    if (mcpwm_num >= MCPWM_UNIT_MAX || timer_num >= MCPWM_TIMER_MAX || gen >= MCPWM_GEN_MAX) {
        ESP_LOGE(TAG, "Invalid MCPWM generator instance");
        return 0;
    }

    uint32_t timer_resolution = mcpwm_timer_get_resolution(mcpwm_num, timer_num);

    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_critical_enter(mcpwm_num);
    uint32_t duty = mcpwm_ll_operator_get_compare_value(hal->dev, op, gen) * (1000000.0 / timer_resolution);
    mcpwm_critical_exit(mcpwm_num);
    return duty;
}

esp_err_t mcpwm_set_signal_high(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen)
{
    //the driver currently always use the timer x for operator x
    return mcpwm_set_duty_type(mcpwm_num, timer_num, gen, MCPWM_DUTY_MODE_FORCE_HIGH);
}

esp_err_t mcpwm_set_signal_low(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_generator_t gen)
{
    //the driver currently always use the timer x for operator x
    return mcpwm_set_duty_type(mcpwm_num, timer_num, gen, MCPWM_DUTY_MODE_FORCE_LOW);
}

esp_err_t mcpwm_carrier_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_enable(context[mcpwm_num].hal.dev, op, true);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_enable(context[mcpwm_num].hal.dev, op, false);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_set_period(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t carrier_period)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_set_prescale(context[mcpwm_num].hal.dev, op, carrier_period + 1);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_set_duty_cycle(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t carrier_duty)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_set_duty(context[mcpwm_num].hal.dev, op, carrier_duty);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_oneshot_mode_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, uint8_t pulse_width)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_set_first_pulse_width(context[mcpwm_num].hal.dev, op, pulse_width + 1);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_output_invert(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num,
                                      mcpwm_carrier_out_ivt_t carrier_ivt_mode)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_out_invert(context[mcpwm_num].hal.dev, op, carrier_ivt_mode);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_carrier_init(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_carrier_config_t *carrier_conf)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_carrier_enable(mcpwm_num, timer_num);
    mcpwm_carrier_set_period(mcpwm_num, timer_num, carrier_conf->carrier_period);
    mcpwm_carrier_set_duty_cycle(mcpwm_num, timer_num, carrier_conf->carrier_duty);
    mcpwm_carrier_oneshot_mode_enable(mcpwm_num, timer_num, carrier_conf->pulse_width_in_os);
    mcpwm_carrier_output_invert(mcpwm_num, timer_num, carrier_conf->carrier_ivt_mode);

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_carrier_in_invert(hal->dev, op, false);
    mcpwm_critical_exit(mcpwm_num);

    return ESP_OK;
}

esp_err_t mcpwm_deadtime_enable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_deadtime_type_t dt_mode,
                                uint32_t red, uint32_t fed)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    ESP_RETURN_ON_FALSE(dt_mode < MCPWM_DEADTIME_TYPE_MAX, ESP_ERR_INVALID_ARG, TAG, MCPWM_DT_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_deadtime_enable_update_delay_on_tez(hal->dev, op, true);
    // The dead time delay unit equals to MCPWM group resolution
    mcpwm_ll_operator_set_deadtime_clock_src(hal->dev, op, MCPWM_LL_DEADTIME_CLK_SRC_GROUP);
    mcpwm_ll_deadtime_set_rising_delay(hal->dev, op, red + 1);
    mcpwm_ll_deadtime_set_falling_delay(hal->dev, op, fed + 1);
    switch (dt_mode) {
    case MCPWM_BYPASS_RED:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0=0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, true);     // S1=1
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, false); // S2=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3=0
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4=0
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 0);  // S5=0
        break;
    case MCPWM_BYPASS_FED:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, true);     // S0=1
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, false);    // S1=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, false); // S2=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3=0
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4=0
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 0);  // S5=0
        break;
    case MCPWM_ACTIVE_HIGH_MODE:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0=0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, false);    // S1=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, false); // S2=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3=0
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4=0
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 0);  // S5=0
        break;
    case MCPWM_ACTIVE_LOW_MODE:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);   // S0=0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, false);   // S1=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, true); // S2=1
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, true); // S3=1
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0); // S4=0
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 0); // S5=0
        break;
    case MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0=0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, false);    // S1=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, false); // S2=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, true);  // S3=1
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4=0
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 0);  // S5=0
        break;
    case MCPWM_ACTIVE_LOW_COMPLIMENT_MODE:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0=0
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, false);    // S1=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, true);  // S2=1
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3=0
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4=0
        mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 0);  // S5=0
        break;
    case MCPWM_ACTIVE_RED_FED_FROM_PWMXA:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3=0
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4=0
        mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 0, true);   // S6=1
        mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 1, false);  // S7=0
        mcpwm_ll_deadtime_enable_deb(hal->dev, op, true);         // S8=1
        break;
    case MCPWM_ACTIVE_RED_FED_FROM_PWMXB:
        mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, false);    // S0=0
        mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3=0
        mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 1);  // S4=1
        mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 0, true);   // S6=1
        mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 1, false);  // S7=0
        mcpwm_ll_deadtime_enable_deb(hal->dev, op, true);         // S8=1
        break;
    default :
        break;
    }
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_deadtime_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_deadtime_bypass_path(hal->dev, op, 1, true);     // S0
    mcpwm_ll_deadtime_bypass_path(hal->dev, op, 0, true);     // S1
    mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 0, false); // S2
    mcpwm_ll_deadtime_invert_outpath(hal->dev, op, 1, false); // S3
    mcpwm_ll_deadtime_red_select_generator(hal->dev, op, 0);  // S4
    mcpwm_ll_deadtime_fed_select_generator(hal->dev, op, 0);  // S5
    mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 0, false);  // S6
    mcpwm_ll_deadtime_swap_out_path(hal->dev, op, 1, false);  // S7
    mcpwm_ll_deadtime_enable_deb(hal->dev, op, false);        // S8
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_fault_init(mcpwm_unit_t mcpwm_num, mcpwm_fault_input_level_t intput_level, mcpwm_fault_signal_t fault_sig)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_fault_enable_detection(hal->dev, fault_sig, true);
    mcpwm_ll_fault_set_active_level(hal->dev, fault_sig, intput_level);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_fault_deinit(mcpwm_unit_t mcpwm_num, mcpwm_fault_signal_t fault_sig)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_fault_enable_detection(hal->dev, fault_sig, false);
    for (int i = 0; i < SOC_MCPWM_OPERATORS_PER_GROUP; i++) {
        mcpwm_ll_brake_clear_ost(hal->dev, i); // make sure operator has exit the ost fault state totally
    }
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_fault_set_cyc_mode(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_fault_signal_t fault_sig,
                                   mcpwm_output_action_t action_on_pwmxa, mcpwm_output_action_t action_on_pwmxb)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, op);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_brake_enable_cbc_mode(hal->dev, op, fault_sig, true);
    mcpwm_ll_brake_enable_cbc_refresh_on_tez(hal->dev, op, true);
    mcpwm_ll_brake_enable_oneshot_mode(hal->dev, op, fault_sig, false);
    mcpwm_ll_generator_set_action_on_brake_event(hal->dev, op, 0, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_OPER_BRAKE_MODE_CBC, action_on_pwmxa);
    mcpwm_ll_generator_set_action_on_brake_event(hal->dev, op, 0, MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, action_on_pwmxa);
    mcpwm_ll_generator_set_action_on_brake_event(hal->dev, op, 1, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_OPER_BRAKE_MODE_CBC, action_on_pwmxb);
    mcpwm_ll_generator_set_action_on_brake_event(hal->dev, op, 1, MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, action_on_pwmxb);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_fault_set_oneshot_mode(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_fault_signal_t fault_sig,
                                       mcpwm_action_on_pwmxa_t action_on_pwmxa, mcpwm_action_on_pwmxb_t action_on_pwmxb)
{
    //the driver currently always use the timer x for operator x
    const int op = timer_num;
    MCPWM_TIMER_CHECK(mcpwm_num, op);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_brake_clear_ost(hal->dev, op);
    mcpwm_ll_brake_enable_oneshot_mode(hal->dev, op, fault_sig, true);
    mcpwm_ll_brake_enable_cbc_mode(hal->dev, op, fault_sig, false);
    mcpwm_ll_generator_set_action_on_brake_event(hal->dev, op, 0, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_OPER_BRAKE_MODE_OST, action_on_pwmxa);
    mcpwm_ll_generator_set_action_on_brake_event(hal->dev, op, 0, MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_OST, action_on_pwmxa);
    mcpwm_ll_generator_set_action_on_brake_event(hal->dev, op, 1, MCPWM_TIMER_DIRECTION_DOWN, MCPWM_OPER_BRAKE_MODE_OST, action_on_pwmxb);
    mcpwm_ll_generator_set_action_on_brake_event(hal->dev, op, 1, MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_OST, action_on_pwmxb);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

static void MCPWM_ISR_ATTR mcpwm_default_isr_handler(void *arg)
{
    mcpwm_context_t *curr_context = (mcpwm_context_t *) arg;
    uint32_t intr_status = mcpwm_ll_intr_get_capture_status(curr_context->hal.dev);
    mcpwm_ll_intr_clear_capture_status(curr_context->hal.dev, intr_status);
    bool need_yield = false;
    for (int i = 0; i < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER; ++i) {
        if ((intr_status >> i) & 0x1) {
            if (curr_context->cap_isr_func[i].fn != NULL) {
                cap_event_data_t edata;
                edata.cap_edge = mcpwm_ll_capture_get_edge(curr_context->hal.dev, i) == MCPWM_CAP_EDGE_NEG ? MCPWM_NEG_EDGE
                                 : MCPWM_POS_EDGE;
                edata.cap_value = mcpwm_ll_capture_get_value(curr_context->hal.dev, i);
                if (curr_context->cap_isr_func[i].fn(curr_context->group_id, i, &edata,
                                                     curr_context->cap_isr_func[i].args)) {
                    need_yield = true;
                }
            }
        }
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t mcpwm_capture_enable_channel(mcpwm_unit_t mcpwm_num, mcpwm_capture_channel_id_t cap_channel, const mcpwm_capture_config_t *cap_conf)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(cap_channel < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER, ESP_ERR_INVALID_ARG, TAG, MCPWM_CAPTURE_ERROR);
    ESP_RETURN_ON_FALSE(context[mcpwm_num].cap_isr_func[cap_channel].fn == NULL, ESP_ERR_INVALID_STATE, TAG,
                        MCPWM_CAP_EXIST_ERROR);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    // enable MCPWM module incase user don't use `mcpwm_init` at all. always increase reference count
    periph_module_enable(mcpwm_periph_signals.groups[mcpwm_num].module);

    mcpwm_hal_init_config_t init_config = {
        .group_id = mcpwm_num
    };
    mcpwm_hal_init(hal, &init_config);

    uint32_t clk_src_hz = 0;
    esp_clk_tree_src_get_freq_hz(MCPWM_TIMER_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_hz);
    uint32_t group_resolution = mcpwm_group_get_resolution(mcpwm_num);
    uint32_t group_pre_scale = clk_src_hz / group_resolution;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_group_enable_clock(hal->dev, true);
    mcpwm_ll_group_set_clock_source(hal->dev, (soc_module_clk_t)MCPWM_CAPTURE_CLK_SRC_DEFAULT);
    mcpwm_ll_group_set_clock_prescale(hal->dev, group_pre_scale);
    mcpwm_ll_capture_enable_timer(hal->dev, true);
    mcpwm_ll_capture_enable_channel(hal->dev, cap_channel, true);
    mcpwm_ll_capture_enable_negedge(hal->dev, cap_channel, cap_conf->cap_edge & MCPWM_NEG_EDGE);
    mcpwm_ll_capture_enable_posedge(hal->dev, cap_channel, cap_conf->cap_edge & MCPWM_POS_EDGE);
    mcpwm_ll_capture_set_prescale(hal->dev, cap_channel, cap_conf->cap_prescale);
    // capture feature should be used with interrupt, so enable it by default
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_CAPTURE(cap_channel), true);
    mcpwm_ll_intr_clear_capture_status(hal->dev, 1 << cap_channel);
    mcpwm_critical_exit(mcpwm_num);

    mcpwm_mutex_lock(mcpwm_num);
    context[mcpwm_num].cap_isr_func[cap_channel].fn = cap_conf->capture_cb;
    context[mcpwm_num].cap_isr_func[cap_channel].args = cap_conf->user_data;
    esp_err_t ret = ESP_OK;
    if (context[mcpwm_num].mcpwm_intr_handle == NULL) {
        ret = esp_intr_alloc(mcpwm_periph_signals.groups[mcpwm_num].irq_id, MCPWM_INTR_FLAG,
                             mcpwm_default_isr_handler,
                             (void *) (context + mcpwm_num), &(context[mcpwm_num].mcpwm_intr_handle));
    }
    mcpwm_mutex_unlock(mcpwm_num);

    return ret;
}

esp_err_t mcpwm_capture_disable_channel(mcpwm_unit_t mcpwm_num, mcpwm_capture_channel_id_t cap_channel)
{
    ESP_RETURN_ON_FALSE(mcpwm_num < SOC_MCPWM_GROUPS, ESP_ERR_INVALID_ARG, TAG, MCPWM_GROUP_NUM_ERROR);
    ESP_RETURN_ON_FALSE(cap_channel < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER, ESP_ERR_INVALID_ARG, TAG, MCPWM_CAPTURE_ERROR);

    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_capture_enable_channel(hal->dev, cap_channel, false);
    mcpwm_ll_intr_enable(hal->dev, MCPWM_LL_EVENT_CAPTURE(cap_channel), false);
    mcpwm_critical_exit(mcpwm_num);

    mcpwm_mutex_lock(mcpwm_num);
    context[mcpwm_num].cap_isr_func[cap_channel].fn = NULL;
    context[mcpwm_num].cap_isr_func[cap_channel].args = NULL;
    // if all user defined ISR callback is disabled, free the handle
    bool should_free_handle = true;
    for (int i = 0; i < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER; ++i) {
        if (context[mcpwm_num].cap_isr_func[i].fn != NULL) {
            should_free_handle = false;
            break;
        }
    }
    esp_err_t ret = ESP_OK;
    if (should_free_handle) {
        ret = esp_intr_free(context[mcpwm_num].mcpwm_intr_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "failed to free interrupt handle");
        }
        context[mcpwm_num].mcpwm_intr_handle = NULL;
    }
    mcpwm_mutex_unlock(mcpwm_num);

    // always decrease reference count
    periph_module_disable(mcpwm_periph_signals.groups[mcpwm_num].module);
    return ret;
}

uint32_t MCPWM_ISR_ATTR mcpwm_capture_signal_get_value(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    if (mcpwm_num >= MCPWM_UNIT_MAX || cap_sig >= SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER) {
        return 0;
    }
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    return mcpwm_ll_capture_get_value(hal->dev, cap_sig);
}

uint32_t mcpwm_capture_get_resolution(mcpwm_unit_t mcpwm_num)
{
    if (mcpwm_num >= MCPWM_UNIT_MAX) {
        ESP_LOGE(TAG, "Invalid MCPWM instance");
        return 0;
    }
#if SOC_MCPWM_CAPTURE_CLK_FROM_GROUP
    return mcpwm_group_get_resolution(mcpwm_num);
#else
    return esp_clk_apb_freq();
#endif
}

uint32_t MCPWM_ISR_ATTR mcpwm_capture_signal_get_edge(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig)
{
    if (mcpwm_num >= MCPWM_UNIT_MAX || cap_sig >= SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER) {
        return 0;
    }
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    return mcpwm_ll_capture_get_edge(hal->dev, cap_sig) == MCPWM_CAP_EDGE_NEG ? 2 : 1;
}

esp_err_t mcpwm_sync_configure(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, const mcpwm_sync_config_t *sync_conf)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    ESP_RETURN_ON_FALSE(sync_conf->sync_sig <= MCPWM_SELECT_GPIO_SYNC2, ESP_ERR_INVALID_ARG, TAG, "invalid sync_sig");
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_timer_set_sync_phase_direction(hal->dev, timer_num, sync_conf->count_direction);
    // sync TEP with current setting
    uint32_t set_phase = 0;
    set_phase = mcpwm_ll_timer_get_peak(hal->dev, timer_num, false) * sync_conf->timer_val / 1000;
    mcpwm_ll_timer_set_sync_phase_value(hal->dev, timer_num, set_phase);
    if (sync_conf->sync_sig == MCPWM_SELECT_NO_INPUT) {
        mcpwm_ll_timer_clear_sync_input(hal->dev, timer_num);
    } else if (sync_conf->sync_sig <= MCPWM_SELECT_TIMER2_SYNC) {
        mcpwm_ll_timer_set_timer_sync_input(hal->dev, timer_num, sync_conf->sync_sig - MCPWM_SELECT_TIMER0_SYNC);
    } else {
        mcpwm_ll_timer_set_gpio_sync_input(hal->dev, timer_num, sync_conf->sync_sig - MCPWM_SELECT_GPIO_SYNC0);
    }
    mcpwm_ll_timer_enable_sync_input(hal->dev, timer_num, true);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_sync_disable(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);
    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;

    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_timer_enable_sync_input(hal->dev, timer_num, false);
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

esp_err_t mcpwm_timer_trigger_soft_sync(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_timer_trigger_soft_sync(hal->dev, timer_num);
    mcpwm_critical_exit(mcpwm_num);

    return ESP_OK;
}

esp_err_t mcpwm_sync_invert_gpio_synchro(mcpwm_unit_t mcpwm_num, mcpwm_sync_signal_t sync_sig, bool invert)
{
    ESP_RETURN_ON_FALSE(sync_sig >= MCPWM_SELECT_GPIO_SYNC0 && sync_sig <= MCPWM_SELECT_GPIO_SYNC2,
                        ESP_ERR_INVALID_ARG, TAG, "invalid sync sig");

    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_critical_enter(mcpwm_num);
    mcpwm_ll_invert_gpio_sync_input(hal->dev, sync_sig - MCPWM_SELECT_GPIO_SYNC0, invert);
    mcpwm_critical_exit(mcpwm_num);

    return ESP_OK;
}

esp_err_t mcpwm_set_timer_sync_output(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, mcpwm_timer_sync_trigger_t trigger)
{
    MCPWM_TIMER_CHECK(mcpwm_num, timer_num);

    mcpwm_hal_context_t *hal = &context[mcpwm_num].hal;
    mcpwm_critical_enter(mcpwm_num);
    switch (trigger) {
    case MCPWM_SWSYNC_SOURCE_SYNCIN:
        mcpwm_ll_timer_propagate_input_sync(hal->dev, timer_num);
        break;
    case MCPWM_SWSYNC_SOURCE_TEZ:
        mcpwm_ll_timer_sync_out_on_timer_event(hal->dev, timer_num, MCPWM_TIMER_EVENT_EMPTY);
        break;
    case MCPWM_SWSYNC_SOURCE_TEP:
        mcpwm_ll_timer_sync_out_on_timer_event(hal->dev, timer_num, MCPWM_TIMER_EVENT_FULL);
        break;
    case MCPWM_SWSYNC_SOURCE_DISABLED:
    default:
        mcpwm_ll_timer_disable_sync_out(hal->dev, timer_num);
        break;
    }
    mcpwm_critical_exit(mcpwm_num);
    return ESP_OK;
}

/**
 * @brief This function will be called during start up, to check that this legacy mcpwm driver is not running along with the new MCPWM driver
 */
__attribute__((constructor))
static void check_mcpwm_driver_conflict(void)
{
    // This function was declared as weak here. The new MCPWM driver has the implementation.
    // So if the new MCPWM driver is not linked in, then `mcpwm_acquire_group_handle()` should be NULL at runtime.
    extern __attribute__((weak)) void *mcpwm_acquire_group_handle(int group_id);
    if ((void *)mcpwm_acquire_group_handle != NULL) {
        ESP_EARLY_LOGE(TAG, "CONFLICT! driver_ng is not allowed to be used with the legacy driver");
        abort();
    }
    ESP_EARLY_LOGW(TAG, "legacy driver is deprecated, please migrate to `driver/mcpwm_prelude.h`");
}
