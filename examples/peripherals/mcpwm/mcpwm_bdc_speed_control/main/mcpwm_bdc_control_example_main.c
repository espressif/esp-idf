/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "driver/pulse_cnt.h"
#include "driver/mcpwm.h"
#include "pid_ctrl.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

// Enable this config,  we will print debug formated string, which in return can be captured and parsed by Serial-Studio
#define SERIAL_STUDIO_DEBUG           0

#define BDC_MCPWM_UNIT                0
#define BDC_MCPWM_TIMER               0
#define BDC_MCPWM_GENA_GPIO_NUM       7
#define BDC_MCPWM_GENB_GPIO_NUM       15
#define BDC_MCPWM_FREQ_HZ             1500

#define BDC_ENCODER_PCNT_HIGH_LIMIT   100
#define BDC_ENCODER_PCNT_LOW_LIMIT    -100
#define BDC_ENCODER_PHASEA_GPIO_NUM   36
#define BDC_ENCODER_PHASEB_GPIO_NUM   35

#define BDC_PID_CALCULATION_PERIOD_US 10000
#define BDC_PID_FEEDBACK_QUEUE_LEN    10

static pid_ctrl_parameter_t pid_runtime_param = {
    .kp = 0.6,
    .ki = 0.3,
    .kd = 0.12,
    .cal_type = PID_CAL_TYPE_INCREMENTAL,
    .max_output   = 100,
    .min_output   = -100,
    .max_integral = 1000,
    .min_integral = -1000,
};
static bool pid_need_update = false;
static int expect_pulses = 300;
static int real_pulses;

typedef struct {
    pcnt_unit_handle_t hall_pcnt_encoder;
    int accumu_count;
    QueueHandle_t pid_feedback_queue;
} motor_control_timer_context_t;

typedef struct {
    QueueHandle_t pid_feedback_queue;
    pid_ctrl_block_handle_t pid_ctrl;
} motor_control_task_context_t;

static bool example_pcnt_on_reach(pcnt_unit_handle_t unit, pcnt_watch_event_data_t *edata, void *user_ctx)
{
    motor_control_timer_context_t *ctx = (motor_control_timer_context_t *)user_ctx;
    ctx->accumu_count += edata->watch_point_value;
    return false;
}

static void brushed_motor_set_duty(float duty_cycle)
{
    /* motor moves in forward direction, with duty cycle = duty % */
    if (duty_cycle > 0) {
        mcpwm_set_signal_low(BDC_MCPWM_UNIT, BDC_MCPWM_TIMER, MCPWM_OPR_A);
        mcpwm_set_duty(BDC_MCPWM_UNIT, BDC_MCPWM_TIMER, MCPWM_OPR_B, duty_cycle);
        mcpwm_set_duty_type(BDC_MCPWM_UNIT, BDC_MCPWM_TIMER, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    }
    /* motor moves in backward direction, with duty cycle = -duty % */
    else {
        mcpwm_set_signal_low(BDC_MCPWM_UNIT, BDC_MCPWM_TIMER, MCPWM_OPR_B);
        mcpwm_set_duty(BDC_MCPWM_UNIT, BDC_MCPWM_TIMER, MCPWM_OPR_A, -duty_cycle);
        mcpwm_set_duty_type(BDC_MCPWM_UNIT, BDC_MCPWM_TIMER, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    }
}

static bool motor_ctrl_timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *arg)
{
    static int last_pulse_count = 0;
    BaseType_t high_task_awoken = pdFALSE;
    motor_control_timer_context_t *user_ctx = (motor_control_timer_context_t *)arg;
    pcnt_unit_handle_t pcnt_unit = user_ctx->hall_pcnt_encoder;

    int cur_pulse_count = 0;
    pcnt_unit_get_count(pcnt_unit, &cur_pulse_count);
    cur_pulse_count += user_ctx->accumu_count;

    int delta = cur_pulse_count - last_pulse_count;
    last_pulse_count = cur_pulse_count;
    xQueueSendFromISR(user_ctx->pid_feedback_queue, &delta, &high_task_awoken);

    return high_task_awoken == pdTRUE;
}

static void bdc_ctrl_task(void *arg)
{
    float duty_cycle = 0;
    motor_control_task_context_t *user_ctx = (motor_control_task_context_t *)arg;
    while (1) {
        xQueueReceive(user_ctx->pid_feedback_queue, &real_pulses, portMAX_DELAY);
        float error = expect_pulses - real_pulses;
        pid_compute(user_ctx->pid_ctrl, error, &duty_cycle);
        brushed_motor_set_duty(duty_cycle);
    }
}

static struct {
    struct arg_dbl *kp;
    struct arg_dbl *ki;
    struct arg_dbl *kd;
    struct arg_end *end;
} pid_ctrl_args;

static int do_pid_ctrl_cmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&pid_ctrl_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, pid_ctrl_args.end, argv[0]);
        return 0;
    }
    if (pid_ctrl_args.kp->count) {
        pid_runtime_param.kp = pid_ctrl_args.kp->dval[0];
    }
    if (pid_ctrl_args.ki->count) {
        pid_runtime_param.ki = pid_ctrl_args.ki->dval[0];
    }
    if (pid_ctrl_args.kd->count) {
        pid_runtime_param.kd = pid_ctrl_args.kd->dval[0];
    }

    pid_need_update = true;
    return 0;
}

static void register_pid_console_command(void)
{
    pid_ctrl_args.kp   = arg_dbl0("p", NULL, "<kp>", "Set Kp value of PID");
    pid_ctrl_args.ki   = arg_dbl0("i", NULL, "<ki>", "Set Ki value of PID");
    pid_ctrl_args.kd   = arg_dbl0("d", NULL, "<kd>", "Set Kd value of PID");
    pid_ctrl_args.end  = arg_end(2);
    const esp_console_cmd_t pid_ctrl_cmd = {
        .command = "pid",
        .help = "Set PID parameters",
        .hint = NULL,
        .func = &do_pid_ctrl_cmd,
        .argtable = &pid_ctrl_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&pid_ctrl_cmd));
}

void app_main(void)
{
    static motor_control_timer_context_t my_timer_ctx = {};

    QueueHandle_t pid_fb_queue = xQueueCreate(BDC_PID_FEEDBACK_QUEUE_LEN, sizeof(int));
    assert(pid_fb_queue);

    printf("configure mcpwm gpio\r\n");
    ESP_ERROR_CHECK(mcpwm_gpio_init(BDC_MCPWM_UNIT, MCPWM0A, BDC_MCPWM_GENA_GPIO_NUM));
    ESP_ERROR_CHECK(mcpwm_gpio_init(BDC_MCPWM_UNIT, MCPWM0B, BDC_MCPWM_GENB_GPIO_NUM));
    printf("init mcpwm driver\n");
    mcpwm_config_t pwm_config = {
        .frequency = BDC_MCPWM_FREQ_HZ,
        .cmpr_a = 0,
        .cmpr_b = 0,
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };
    ESP_ERROR_CHECK(mcpwm_init(BDC_MCPWM_UNIT, BDC_MCPWM_TIMER, &pwm_config));

    printf("init and start rotary encoder\r\n");
    pcnt_unit_config_t unit_config = {
        .high_limit = BDC_ENCODER_PCNT_HIGH_LIMIT,
        .low_limit = BDC_ENCODER_PCNT_LOW_LIMIT,
    };
    pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = BDC_ENCODER_PHASEA_GPIO_NUM,
        .level_gpio_num = BDC_ENCODER_PHASEB_GPIO_NUM,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = BDC_ENCODER_PHASEB_GPIO_NUM,
        .level_gpio_num = BDC_ENCODER_PHASEA_GPIO_NUM,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, BDC_ENCODER_PCNT_HIGH_LIMIT));
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, BDC_ENCODER_PCNT_LOW_LIMIT));
    pcnt_event_callbacks_t pcnt_cbs = {
        .on_reach = example_pcnt_on_reach,
    };
    ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(pcnt_unit, &pcnt_cbs, &my_timer_ctx));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    printf("init PID control block\r\n");
    pid_ctrl_block_handle_t pid_ctrl;
    pid_ctrl_config_t pid_config = {
        .init_param = pid_runtime_param,
    };
    ESP_ERROR_CHECK(pid_new_control_block(&pid_config, &pid_ctrl));

    printf("init motor control timer\r\n");
    gptimer_handle_t gptimer;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_APB,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    printf("create motor control task\r\n");
    static motor_control_task_context_t my_ctrl_task_ctx = {};
    my_ctrl_task_ctx.pid_feedback_queue = pid_fb_queue;
    my_ctrl_task_ctx.pid_ctrl = pid_ctrl;
    xTaskCreate(bdc_ctrl_task, "bdc_ctrl_task", 4096, &my_ctrl_task_ctx, 5, NULL);

    printf("start motor control timer\r\n");
    my_timer_ctx.pid_feedback_queue = pid_fb_queue;
    my_timer_ctx.hall_pcnt_encoder = pcnt_unit;
    gptimer_event_callbacks_t gptimer_cbs = {
        .on_alarm = motor_ctrl_timer_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &gptimer_cbs, &my_timer_ctx));
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = BDC_PID_CALCULATION_PERIOD_US,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    printf("install console command line\r\n");
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "dc-motor>";
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
    register_pid_console_command();
    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
        // the following logging format is according to the requirement of serial-studio
        // also see the parser mapping file `serial-studio-proto-map.json` in the project folder
#if SERIAL_STUDIO_DEBUG
        printf("/*%d*/\r\n", real_pulses);
#endif
        if (pid_need_update) {
            pid_update_parameters(pid_ctrl, &pid_runtime_param);
            pid_need_update = false;
        }
    }
}
