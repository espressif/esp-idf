/* cmd_mcpwm_motor.h

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_log.h"
#include "mcpwm_brushed_dc_control_example.h"

#define MOTOR_CTRL_CMD_CHECK(ins)   if(arg_parse(argc, argv, (void **)&ins)){ \
                                        arg_print_errors(stderr, ins.end, argv[0]); \
                                        return 0;}

static mcpwm_motor_control_t *mc;
extern SemaphoreHandle_t g_motor_mux;

static struct {
    struct arg_str *pid_flag;
    struct arg_int *period;
    struct arg_lit *show;
    struct arg_end *end;

} motor_ctrl_config_args;

static struct {
    struct arg_dbl *max;
    struct arg_dbl *min;
    struct arg_dbl *pace;
    struct arg_dbl *init;
    struct arg_str *mode;
    struct arg_end *end;

} motor_ctrl_expt_args;

static struct {
    struct arg_dbl *kp;
    struct arg_dbl *ki;
    struct arg_dbl *kd;
    struct arg_str *type;
    struct arg_end *end;
} motor_ctrl_pid_args;

static struct {
    struct arg_int *start;
    struct arg_lit *stop;
    struct arg_end *end;
} motor_ctrl_motor_args;


static void print_current_status(void)
{
    printf("\n -----------------------------------------------------------------\n");
    printf("                  Current Configuration Status                \n\n");
    printf(" Configuration\n       Period = %d ms\tPID = %s\n\n",
           mc->cfg.ctrl_period, mc->cfg.pid_enable ? "enabled" : "disabled");
    printf(" PID - %s\n       Kp = %.3f\tKi = %.3f\tKd = %.3f\n\n",
           (mc->pid_param.cal_type == PID_CAL_TYPE_POSITIONAL) ? "Location" : "Increment",
           mc->pid_param.kp, mc->pid_param.ki, mc->pid_param.kd);
    printf(" Expectation - %s\n       init = %.3f\tmax = %.3f\tmin = %.3f\tpace = %.3f\n\n",
           mc->cfg.expt_mode ? (mc->cfg.expt_mode == MOTOR_CTRL_MODE_TRIANGLE ? "Triangle" : "Rectangle") : "Fixed",
           mc->cfg.expt_init, mc->cfg.expt_max, mc->cfg.expt_min, mc->cfg.expt_pace);
    printf(" MCPWM\n       Frequency = %d Hz\n\n", mc->cfg.pwm_freq);
    printf(" Motor\n       Running seconds = %d\n", mc->cfg.running_sec);
    printf(" -----------------------------------------------------------------\n\n");
}


static int do_motor_ctrl_config_cmd(int argc, char **argv)
{
    MOTOR_CTRL_CMD_CHECK(motor_ctrl_config_args);
    xSemaphoreTake(g_motor_mux, portMAX_DELAY);
    if (motor_ctrl_config_args.pid_flag->count) {
        if (!strcmp(*motor_ctrl_config_args.pid_flag->sval, "n") ||
                !strcmp(*motor_ctrl_config_args.pid_flag->sval, "no")) {
            mc->cfg.pid_enable = false;
            printf("config: pid disabled\n");
        } else {
            mc->cfg.pid_enable = true;
            printf("config: pid enabled\n");
        }
    }

    if (motor_ctrl_config_args.period->count) {
        mc->cfg.ctrl_period = motor_ctrl_config_args.period->ival[0];
        motor_ctrl_timer_set_period(mc->cfg.ctrl_period);
        printf("config: control period = mc->cfg.ctrl_period\n");
    }

    if (motor_ctrl_config_args.show->count) {
        print_current_status();
    }
    xSemaphoreGive(g_motor_mux);
    return 0;
}

static int do_motor_ctrl_expt_cmd(int argc, char **argv)
{
    MOTOR_CTRL_CMD_CHECK(motor_ctrl_expt_args);
    xSemaphoreTake(g_motor_mux, portMAX_DELAY);
    if (motor_ctrl_expt_args.init->count) {
        mc->cfg.expt_init = motor_ctrl_expt_args.init->dval[0];
        printf("expt: init = %.3f\n", mc->cfg.expt_init);
    }
    if (motor_ctrl_expt_args.max->count) {
        mc->cfg.expt_max = motor_ctrl_expt_args.max->dval[0];
        printf("expt: max = %.3f\n", mc->cfg.expt_max);
    }
    if (motor_ctrl_expt_args.min->count) {
        mc->cfg.expt_min = motor_ctrl_expt_args.min->dval[0];
        printf("expt: min = %.3f\n", mc->cfg.expt_min);
    }
    if (motor_ctrl_expt_args.pace->count) {
        mc->cfg.expt_pace = motor_ctrl_expt_args.pace->dval[0];
        printf("expt: pace = %.3f\n", mc->cfg.expt_pace);
    }
    if (motor_ctrl_expt_args.mode->count) {
        if (!strcmp(*motor_ctrl_expt_args.mode->sval, "fixed")) {
            mc->cfg.expt_mode = MOTOR_CTRL_MODE_FIXED;
            printf("expt: mode = fixed\n");
        } else if (!strcmp(*motor_ctrl_expt_args.mode->sval, "tri")) {
            mc->cfg.expt_mode = MOTOR_CTRL_MODE_TRIANGLE;
            printf("expt: mode = triangle\n");
        } else if (!strcmp(*motor_ctrl_expt_args.mode->sval, "rect")) {
            mc->cfg.expt_mode = MOTOR_CTRL_MODE_RECTANGLE;
            printf("expt: mode = rectangle\n");
        } else {
            mc->cfg.expt_mode = MOTOR_CTRL_MODE_TRIANGLE;
            printf("expt: mode = triangle\n");
        }
    }
    xSemaphoreGive(g_motor_mux);
    return 0;
}

static int do_motor_ctrl_pid_cmd(int argc, char **argv)
{
    int ret = 0;
    MOTOR_CTRL_CMD_CHECK(motor_ctrl_pid_args);
    xSemaphoreTake(g_motor_mux, portMAX_DELAY);
    if (motor_ctrl_pid_args.kp->count) {
        mc->pid_param.kp = motor_ctrl_pid_args.kp->dval[0];
        printf("pid: kp = %.3f\n", mc->pid_param.kp);
    }
    if (motor_ctrl_pid_args.ki->count) {
        mc->pid_param.ki = motor_ctrl_pid_args.ki->dval[0];
        printf("pid: ki = %.3f\n", mc->pid_param.ki);
    }
    if (motor_ctrl_pid_args.kd->count) {
        mc->pid_param.kd = motor_ctrl_pid_args.kd->dval[0];
        printf("pid: kd = %.3f\n", mc->pid_param.kd);
    }

    if (motor_ctrl_pid_args.type->count) {
        if (!strcmp(motor_ctrl_pid_args.type->sval[0], "loc")) {
            mc->pid_param.cal_type = PID_CAL_TYPE_POSITIONAL;
            printf("pid: type = positional\n");
        } else if (!strcmp(motor_ctrl_pid_args.type->sval[0], "inc")) {
            mc->pid_param.cal_type = PID_CAL_TYPE_INCREMENTAL;
            printf("pid: type = incremental\n");
        } else {
            printf("Invalid pid type:%s\n", motor_ctrl_pid_args.type->sval[0]);
            ret = 1;
        }
    }
    pid_update_parameters(mc->pid, &mc->pid_param);
    xSemaphoreGive(g_motor_mux);
    return ret;
}

static int do_motor_ctrl_motor_cmd(int argc, char **argv)
{
    MOTOR_CTRL_CMD_CHECK(motor_ctrl_motor_args);
    xSemaphoreTake(g_motor_mux, portMAX_DELAY);
    if (motor_ctrl_motor_args.start->count) {
        mc->cfg.running_sec = motor_ctrl_motor_args.start->ival[0];
        // Start the motor
        brushed_motor_start(mc);
        mc->cfg.running_sec ?
        printf("motor: motor starts to run in %d seconds\n", mc->cfg.running_sec) :
        printf("motor: motor starts to run, input 'motor -d' to stop it\n");
    }

    if (motor_ctrl_motor_args.stop->count) {
        // Stop the motor
        brushed_motor_stop(mc);
        printf("motor: motor stoped\n");
    }
    xSemaphoreGive(g_motor_mux);
    return 0;
}

static void register_motor_ctrl_config(void)
{
    motor_ctrl_config_args.pid_flag = arg_str0(NULL, "pid", "<y|n>", "Enable or disable PID algorithm");
    motor_ctrl_config_args.period = arg_int0("T", "period", "<ms>", "Set motor control period");
    motor_ctrl_config_args.show = arg_lit0("s", "show", "Show current configurations");
    motor_ctrl_config_args.end = arg_end(2);
    const esp_console_cmd_t motor_ctrl_cfg_cmd = {
        .command = "config",
        .help = "Enable or disable PID and set motor control period",
        .hint = "config -s",
        .func = &do_motor_ctrl_config_cmd,
        .argtable = &motor_ctrl_config_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&motor_ctrl_cfg_cmd));
}

static void register_motor_ctrl_expt(void)
{
    motor_ctrl_expt_args.init = arg_dbl0("i", "init", "<duty>", "Initial expectation. Usually between -100~100");
    motor_ctrl_expt_args.max  = arg_dbl0(NULL, "max", "<duty>", "Max limitation for dynamic expectation");
    motor_ctrl_expt_args.min  = arg_dbl0(NULL, "min", "<duty>", "Min limitation for dynamic expectation");
    motor_ctrl_expt_args.pace = arg_dbl0("p", "pace", "<double>", "The increasing pace of expectation every 50ms");
    motor_ctrl_expt_args.mode = arg_str0("m", "mode", "<fixed/tri/rect>",
                                         "Select static or dynamic expectation wave mode. 'fixed' for static, 'tri' for triangle, 'rect' for rectangle");
    motor_ctrl_expt_args.end  = arg_end(2);

    const esp_console_cmd_t motor_ctrl_expt_cmd = {
        .command = "expt",
        .help = "Set initial value, limitation and wave mode of expectation. Both dynamic and static mode are available",
        .hint = "expt -i <duty> -m <fixed/tri/rect> -p <double> --max <duty> --min <duty>",
        .func = &do_motor_ctrl_expt_cmd,
        .argtable = &motor_ctrl_expt_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&motor_ctrl_expt_cmd));
}

static void register_motor_ctrl_pid(void)
{
    motor_ctrl_pid_args.kp   = arg_dbl0("p", "kp", "<double>", "Set Kp value for PID");
    motor_ctrl_pid_args.ki   = arg_dbl0("i", "ki", "<double>", "Set Ki value for PID");
    motor_ctrl_pid_args.kd   = arg_dbl0("d", "kd", "<double>", "Set Kd value for PID");
    motor_ctrl_pid_args.type = arg_str0("t", "type", "<loc/inc>", "Select locational PID or incremental PID");
    motor_ctrl_pid_args.end  = arg_end(2);

    const esp_console_cmd_t motor_ctrl_pid_cmd = {
        .command = "pid",
        .help = "Set parameters and type for PID algorithm",
        .hint = "pid -p <double> -i <double> -d <double> -t <loc/inc>",
        .func = &do_motor_ctrl_pid_cmd,
        .argtable = &motor_ctrl_pid_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&motor_ctrl_pid_cmd));
}

static void register_motor_ctrl_motor(void)
{
    motor_ctrl_motor_args.start = arg_int0("u", "start", "<seconds>", "Set running seconds for motor, set '0' to keep motor running");
    motor_ctrl_motor_args.stop = arg_lit0("d", "stop", "Stop the motor");
    motor_ctrl_motor_args.end = arg_end(2);

    const esp_console_cmd_t motor_ctrl_motor_cmd = {
        .command = "motor",
        .help = "Start or stop the motor",
        .hint = "motor -u 10",
        .func = &do_motor_ctrl_motor_cmd,
        .argtable = &motor_ctrl_motor_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&motor_ctrl_motor_cmd));
}

void cmd_mcpwm_motor_init(mcpwm_motor_control_t *motor_ctrl)
{
    mc = motor_ctrl;
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "mcpwm-motor>";

    // install console REPL environment
#if CONFIG_ESP_CONSOLE_UART
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_CDC
    esp_console_dev_usb_cdc_config_t cdc_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&cdc_config, &repl_config, &repl));
#endif

    register_motor_ctrl_config();
    register_motor_ctrl_expt();
    register_motor_ctrl_pid();
    register_motor_ctrl_motor();

    printf("\n =================================================================\n");
    printf(" |             Example of Motor Control                          |\n");
    printf(" |                                                               |\n");
    printf(" |  1. Try 'help', check all supported commands                  |\n");
    printf(" |  2. Try 'config' to set control period or pwm frequency       |\n");
    printf(" |  3. Try 'pid' to configure pid paremeters                     |\n");
    printf(" |  4. Try 'expt' to set expectation value and mode              |\n");
    printf(" |  5. Try 'motor' to start motor in several seconds or stop it  |\n");
    printf(" |                                                               |\n");
    printf(" =================================================================\n\n");

    printf("Default configuration are shown as follows.\nYou can input 'config -s' to check current status.");
    print_current_status();

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
