
/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"

#define BLDC_MCPWM_TIMER_RESOLUTION_HZ 10000000 // 10MHz, 1 tick = 0.1us
#define BLDC_MCPWM_PERIOD              500      // 50us, 20KHz
#define BLDC_SPIN_DIRECTION_CCW        false    // define the spin direction
#define BLDC_SPEED_UPDATE_PERIOD_US    200000   // 200ms

#define BLDC_DRV_EN_GPIO          46
#define BLDC_DRV_FAULT_GPIO       10
#define BLDC_PWM_UH_GPIO          47
#define BLDC_PWM_UL_GPIO          21
#define BLDC_PWM_VH_GPIO          14
#define BLDC_PWM_VL_GPIO          13
#define BLDC_PWM_WH_GPIO          12
#define BLDC_PWM_WL_GPIO          11
#define HALL_CAP_U_GPIO           4
#define HALL_CAP_V_GPIO           5
#define HALL_CAP_W_GPIO           6

#define BLDC_MCPWM_OP_INDEX_U     0
#define BLDC_MCPWM_OP_INDEX_V     1
#define BLDC_MCPWM_OP_INDEX_W     2
#define BLDC_MCPWM_GEN_INDEX_HIGH 0
#define BLDC_MCPWM_GEN_INDEX_LOW  1

static const char *TAG = "example";

typedef void (*bldc_hall_phase_action_t)(mcpwm_gen_handle_t (*gens)[2]);

static inline uint32_t bldc_get_hall_sensor_value(bool ccw)
{
    uint32_t hall_val = gpio_get_level(HALL_CAP_U_GPIO) * 4 + gpio_get_level(HALL_CAP_V_GPIO) * 2 + gpio_get_level(HALL_CAP_W_GPIO) * 1;
    return ccw ? hall_val ^ (0x07) : hall_val;
}

static bool IRAM_ATTR bldc_hall_updated(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    TaskHandle_t task_to_notify = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
    vTaskNotifyGiveFromISR(task_to_notify, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

// U+V-
static void bldc_set_phase_up_vm(mcpwm_gen_handle_t (*gens)[2])
{
    // U+ = PWM, U- = _PWM_
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_HIGH], -1, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_LOW], -1, true);

    // V+ = 0, V- = 1  --[because gen_low is inverted by dead time]--> V+ = 0, V- = 0
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_LOW], 0, true);

    // W+ = 0, W- = 0  --[because gen_low is inverted by dead time]--> W+ = 0, W- = 1
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_LOW], 1, true);
}

// W+U-
static void bldc_set_phase_wp_um(mcpwm_gen_handle_t (*gens)[2])
{
    // U+ = 0, U- = 1  --[because gen_low is inverted by dead time]--> U+ = 0, U- = 0
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_LOW], 0, true);

    // V+ = 0, V- = 0  --[because gen_low is inverted by dead time]--> V+ = 0, V- = 1
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_LOW], 1, true);

    // W+ = PWM, W- = _PWM_
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_HIGH], -1, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_LOW], -1, true);
}

// W+V-
static void bldc_set_phase_wp_vm(mcpwm_gen_handle_t (*gens)[2])
{
    // U+ = 0, U- = 0  --[because gen_low is inverted by dead time]--> U+ = 0, U- = 1
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_LOW], 1, true);

    // V+ = 0, V- = 1  --[because gen_low is inverted by dead time]--> V+ = 0, V- = 0
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_LOW], 0, true);

    // W+ = PWM, W- = _PWM_
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_HIGH], -1, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_LOW], -1, true);
}

// V+U-
static void bldc_set_phase_vp_um(mcpwm_gen_handle_t (*gens)[2])
{
    // U+ = 0, U- = 1  --[because gen_low is inverted by dead time]--> U+ = 0, U- = 0
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_LOW], 0, true);

    // V+ = PWM, V- = _PWM_
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_HIGH], -1, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_LOW], -1, true);

    // W+ = 0, W- = 0  --[because gen_low is inverted by dead time]--> W+ = 0, W- = 1
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_LOW], 1, true);
}

// V+W-
static void bldc_set_phase_vp_wm(mcpwm_gen_handle_t (*gens)[2])
{
    // U+ = 0, U- = 0  --[because gen_low is inverted by dead time]--> U+ = 0, U- = 1
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_LOW], 1, true);

    // V+ = PWM, V- = _PWM_
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_HIGH], -1, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_LOW], -1, true);

    // W+ = 0, W- = 1  --[because gen_low is inverted by dead time]--> W+ = 0, W- = 0
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_LOW], 0, true);
}

// U+W- / A+C-
static void bldc_set_phase_up_wm(mcpwm_gen_handle_t (*gens)[2])
{
    // U+ = PWM, U- = _PWM_
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_HIGH], -1, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_U][BLDC_MCPWM_GEN_INDEX_LOW], -1, true);

    // V+ = 0, V- = 0  --[because gen_low is inverted by dead time]--> V+ = 0, V- = 1
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_V][BLDC_MCPWM_GEN_INDEX_LOW], 1, true);

    // W+ = 0, W- = 1  --[because gen_low is inverted by dead time]--> W+ = 0, W- = 0
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true);
    mcpwm_generator_set_force_level(gens[BLDC_MCPWM_OP_INDEX_W][BLDC_MCPWM_GEN_INDEX_LOW], 0, true);
}

static const bldc_hall_phase_action_t s_hall_actions[] = {
    [2] = bldc_set_phase_up_vm,
    [6] = bldc_set_phase_wp_vm,
    [4] = bldc_set_phase_wp_um,
    [5] = bldc_set_phase_vp_um,
    [1] = bldc_set_phase_vp_wm,
    [3] = bldc_set_phase_up_wm,
};

static void update_motor_speed_callback(void *arg)
{
    static int step = 20;
    static int cur_speed = 0;
    if ((cur_speed + step) > 300 || (cur_speed + step) < 0) {
        step *= -1;
    }
    cur_speed += step;

    mcpwm_cmpr_handle_t *cmprs = (mcpwm_cmpr_handle_t *)arg;
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(cmprs[i], cur_speed));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Disable MOSFET gate");
    gpio_config_t drv_en_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << BLDC_DRV_EN_GPIO,
    };
    ESP_ERROR_CHECK(gpio_config(&drv_en_config));
    gpio_set_level(BLDC_DRV_EN_GPIO, 0);

    ESP_LOGI(TAG, "Create MCPWM timer");
    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = BLDC_MCPWM_TIMER_RESOLUTION_HZ,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = BLDC_MCPWM_PERIOD,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    ESP_LOGI(TAG, "Create MCPWM operator");
    mcpwm_oper_handle_t operators[3];
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
    };
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operators[i]));
    }

    ESP_LOGI(TAG, "Connect operators to the same timer");
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operators[i], timer));
    }

    ESP_LOGI(TAG, "Create comparators");
    mcpwm_cmpr_handle_t comparators[3];
    mcpwm_comparator_config_t compare_config = {
        .flags.update_cmp_on_tez = true,
    };
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_new_comparator(operators[i], &compare_config, &comparators[i]));
        // set compare value to 0, we will adjust the speed in a period timer callback
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparators[i], 0));
    }

    ESP_LOGI(TAG, "Create over current fault detector");
    mcpwm_fault_handle_t over_cur_fault = NULL;
    mcpwm_gpio_fault_config_t gpio_fault_config = {
        .gpio_num = BLDC_DRV_FAULT_GPIO,
        .group_id = 0,
        .flags.active_level = 0, // low level means fault, refer to DRV8302 datasheet
    };
    ESP_ERROR_CHECK(mcpwm_new_gpio_fault(&gpio_fault_config, &over_cur_fault));
    // pull up the GPIO internally
    ESP_ERROR_CHECK(gpio_set_pull_mode(BLDC_DRV_FAULT_GPIO, GPIO_PULLUP_ONLY));

    ESP_LOGI(TAG, "Set brake mode on the fault event");
    mcpwm_brake_config_t brake_config = {
        .brake_mode = MCPWM_OPER_BRAKE_MODE_CBC,
        .fault = over_cur_fault,
        .flags.cbc_recover_on_tez = true,
    };
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_operator_set_brake_on_fault(operators[i], &brake_config));
    }

    ESP_LOGI(TAG, "Create PWM generators");
    mcpwm_gen_handle_t generators[3][2] = {};
    mcpwm_generator_config_t gen_config = {};
    const int gen_gpios[3][2] = {
        {BLDC_PWM_UH_GPIO, BLDC_PWM_UL_GPIO},
        {BLDC_PWM_VH_GPIO, BLDC_PWM_VL_GPIO},
        {BLDC_PWM_WH_GPIO, BLDC_PWM_WL_GPIO},
    };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            gen_config.gen_gpio_num = gen_gpios[i][j];
            ESP_ERROR_CHECK(mcpwm_new_generator(operators[i], &gen_config, &generators[i][j]));
        }
    }

    ESP_LOGI(TAG, "Set generator actions");
    // gen_high and gen_low output the same waveform after the following configuration
    // we will use the dead time module to add edge delay, also make gen_high and gen_low complementary
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generators[i][BLDC_MCPWM_GEN_INDEX_HIGH],
                                                                  MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generators[i][BLDC_MCPWM_GEN_INDEX_HIGH],
                                                                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators[i], MCPWM_GEN_ACTION_LOW)));
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_brake_event(generators[i][BLDC_MCPWM_GEN_INDEX_HIGH],
                                                                  MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_LOW)));
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_brake_event(generators[i][BLDC_MCPWM_GEN_INDEX_HIGH],
                                                                  MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_LOW)));

        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generators[i][BLDC_MCPWM_GEN_INDEX_LOW],
                                                                  MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generators[i][BLDC_MCPWM_GEN_INDEX_LOW],
                                                                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators[i], MCPWM_GEN_ACTION_LOW)));
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_brake_event(generators[i][BLDC_MCPWM_GEN_INDEX_LOW],
                                                                  MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_LOW)));
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_brake_event(generators[i][BLDC_MCPWM_GEN_INDEX_LOW],
                                                                  MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_LOW)));
    }

    ESP_LOGI(TAG, "Setup deadtime");
    mcpwm_dead_time_config_t dt_config = {
        .posedge_delay_ticks = 5,
    };
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(generators[i][BLDC_MCPWM_GEN_INDEX_HIGH], generators[i][BLDC_MCPWM_GEN_INDEX_HIGH], &dt_config));
    }
    dt_config = (mcpwm_dead_time_config_t) {
        .negedge_delay_ticks = 5,
        .flags.invert_output = true,
    };
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(generators[i][BLDC_MCPWM_GEN_INDEX_LOW], generators[i][BLDC_MCPWM_GEN_INDEX_LOW], &dt_config));
    }

    ESP_LOGI(TAG, "Turn off all the gates");
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_generator_set_force_level(generators[i][BLDC_MCPWM_GEN_INDEX_HIGH], 0, true));
        // because gen_low is inverted by dead time module, so we need to set force level to 1
        ESP_ERROR_CHECK(mcpwm_generator_set_force_level(generators[i][BLDC_MCPWM_GEN_INDEX_LOW], 1, true));
    }

    ESP_LOGI(TAG, "Create Hall sensor capture channels");
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };
    ESP_ERROR_CHECK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));
    mcpwm_cap_channel_handle_t cap_channels[3];
    mcpwm_capture_channel_config_t cap_channel_config = {
        .prescale = 1,
        .flags.neg_edge = true,
        .flags.pos_edge = true,
    };
    const int cap_chan_gpios[3] = {HALL_CAP_U_GPIO, HALL_CAP_V_GPIO, HALL_CAP_W_GPIO};
    for (int i = 0; i < 3; i++) {
        cap_channel_config.gpio_num = cap_chan_gpios[i];
        ESP_ERROR_CHECK(mcpwm_new_capture_channel(cap_timer, &cap_channel_config, &cap_channels[i]));
        // pull up the GPIO internally
        ESP_ERROR_CHECK(gpio_set_pull_mode(cap_chan_gpios[i], GPIO_PULLUP_ONLY));
    }

    ESP_LOGI(TAG, "Register event callback for capture channels");
    TaskHandle_t task_to_notify = xTaskGetCurrentTaskHandle();
    for (int i = 0; i < 3; i++) {
        mcpwm_capture_event_callbacks_t cbs = {
            .on_cap = bldc_hall_updated,
        };
        ESP_ERROR_CHECK(mcpwm_capture_channel_register_event_callbacks(cap_channels[i], &cbs, task_to_notify));
    }

    ESP_LOGI(TAG, "Enable capture channels");
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_capture_channel_enable(cap_channels[i]));
    }

    ESP_LOGI(TAG, "Enable and start capture timer");
    ESP_ERROR_CHECK(mcpwm_capture_timer_enable(cap_timer));
    ESP_ERROR_CHECK(mcpwm_capture_timer_start(cap_timer));

    ESP_LOGI(TAG, "Start a timer to adjust motor speed periodically");
    esp_timer_handle_t periodic_timer = NULL;
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = update_motor_speed_callback,
        .arg = comparators,
    };
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, BLDC_SPEED_UPDATE_PERIOD_US));

    ESP_LOGI(TAG, "Enable MOSFET gate");
    gpio_set_level(BLDC_DRV_EN_GPIO, 1);

    ESP_LOGI(TAG, "Start the MCPWM timer");
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    uint32_t hall_sensor_value = 0;
    while (1) {
        // The rotation direction is controlled by inverting the hall sensor value
        hall_sensor_value = bldc_get_hall_sensor_value(BLDC_SPIN_DIRECTION_CCW);
        if (hall_sensor_value >= 1 && hall_sensor_value <= 6) {
            s_hall_actions[hall_sensor_value](generators);
        } else {
            ESP_LOGE(TAG, "invalid bldc phase, wrong hall sensor value:%"PRIu32, hall_sensor_value);
        }
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}
