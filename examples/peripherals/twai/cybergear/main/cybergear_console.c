/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_console.h"
#include "esp_log.h"
#include "esp_check.h"
#include "argtable3/argtable3.h"
#include "cybergear.h"
#include "cybergear_console.h"

// static motor handle
cybergear_handle_t s_motor;

// Console commands
static int cmd_enable(int argc, char **argv)
{
    cybergear_set_speed_limit(s_motor, CYBERGEAR_SPEED_MAX);
    cybergear_set_current_limit(s_motor, CYBERGEAR_CURRENT_MAX);
    cybergear_enable(s_motor);
    return 0;
}

static int cmd_disable(int argc, char **argv)
{
    cybergear_disable(s_motor, true);
    return 0;
}

static struct {
    struct arg_int *mode;
    struct arg_end *end;
} set_mode_args;

static int cmd_set_mode(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&set_mode_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_mode_args.end, argv[0]);
        return 1;
    }
    cybergear_mode_t mode = set_mode_args.mode->ival[0];
    cybergear_set_mode(s_motor, mode);
    return 0;
}

static struct {
    struct arg_dbl *torque;
    struct arg_dbl *location;
    struct arg_dbl *speed;
    struct arg_dbl *kp;
    struct arg_dbl *kd;
    struct arg_end *end;
} motion_control_args;

static int cmd_motion_control(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&motion_control_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, motion_control_args.end, argv[0]);
        return 1;
    }
    cybergear_motion_control_t motion_control = {
        .torque = motion_control_args.torque->dval[0],
        .location = motion_control_args.location->dval[0],
        .speed = motion_control_args.speed->dval[0],
        .kp = motion_control_args.kp->dval[0],
        .kd = motion_control_args.kd->dval[0],
    };
    cybergear_control_motion(s_motor, &motion_control);
    return 0;
}

static struct {
    struct arg_dbl *speed;
    struct arg_end *end;
} set_speed_args;

static int cmd_set_speed(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&set_speed_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_speed_args.end, argv[0]);
        return 1;
    }
    float speed_ref = set_speed_args.speed->dval[0];
    cybergear_set_speed(s_motor, speed_ref);
    return 0;
}

static struct {
    struct arg_dbl *location;
    struct arg_end *end;
} set_location_args;

static int cmd_set_location(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&set_location_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_location_args.end, argv[0]);
        return 1;
    }
    float location = set_location_args.location->dval[0];
    cybergear_set_location(s_motor, location);
    return 0;
}

static struct {
    struct arg_dbl *current;
    struct arg_end *end;
} set_current_args;

static int cmd_set_current(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&set_current_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_current_args.end, argv[0]);
        return 1;
    }
    float current = set_current_args.current->dval[0];
    cybergear_set_current(s_motor, current);
    return 0;
}

static struct {
    struct arg_int *motor_id;
    struct arg_end *end;
} set_motor_id_args;

static int cmd_set_motor_id(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&set_motor_id_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, set_motor_id_args.end, argv[0]);
        return 1;
    }
    uint8_t motor_id = set_motor_id_args.motor_id->ival[0];
    cybergear_set_motor_id(s_motor, motor_id);
    return 0;
}

static int cmd_get_motor_id(int argc, char **argv)
{
    uint8_t motor_id;
    cybergear_get_motor_id(s_motor, &motor_id);
    printf("Motor ID: %d\n", motor_id);
    return 0;
}

static int cmd_zero_location(int argc, char **argv)
{
    cybergear_reset_zero_location(s_motor);
    return 0;
}

static int cmd_status(int argc, char **argv)
{
    cybergear_status_t status;
    cybergear_get_status(s_motor, &status);
    printf("===== Motor Status =====\n");
    printf("Motor ID: %d\n", status.motor_id);
    printf("Fault Status: %d\n", status.fault);
    printf("State: %d\n", status.state);
    printf("Location: %f\n", status.location);
    printf("Speed: %f\n", status.speed);
    printf("Torque: %f\n", status.torque);
    printf("Temperature: %f\n", status.temperature);
    printf("========================\n");
    return 0;
}

// Register CyberGear Console commands
void register_cybergear_commands(cybergear_handle_t motor_handle)
{
    s_motor = motor_handle;
    const esp_console_cmd_t enable_cmd = {
        .command = "enable",
        .help = "Enable motor",
        .hint = NULL,
        .func = &cmd_enable,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&enable_cmd));

    const esp_console_cmd_t disable_cmd = {
        .command = "disable",
        .help = "Disable motor and clear fault",
        .hint = NULL,
        .func = &cmd_disable,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&disable_cmd));

    set_mode_args.mode = arg_int1(NULL, NULL, "<mode>", "Control mode (0:Motion, 1:Location, 2:Speed, 3:Current)");
    set_mode_args.end = arg_end(1);

    const esp_console_cmd_t set_mode_cmd = {
        .command = "mode",
        .help = "Set control mode",
        .hint = NULL,
        .func = &cmd_set_mode,
        .argtable = &set_mode_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_mode_cmd));

    motion_control_args.torque = arg_dbl1("t", "torque", "<torque>", "Torque (Nm)");
    motion_control_args.location = arg_dbl1("l", "loc", "<location>", "Target location (rad)");
    motion_control_args.speed = arg_dbl1("s", "speed", "<speed>", "Target speed (rad/s)");
    motion_control_args.kp = arg_dbl1("p", "kp", "<kp>", "Proportional gain");
    motion_control_args.kd = arg_dbl1("d", "kd", "<kd>", "Derivative gain");
    motion_control_args.end = arg_end(5);

    const esp_console_cmd_t motion_control_cmd = {
        .command = "motion",
        .help = "Motion control command",
        .hint = NULL,
        .func = &cmd_motion_control,
        .argtable = &motion_control_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&motion_control_cmd));

    set_speed_args.speed = arg_dbl1("s", "set", "<speed>", "Target speed (rad/s)");
    set_speed_args.end = arg_end(1);

    const esp_console_cmd_t set_speed_cmd = {
        .command = "speed",
        .help = "Set motor speed",
        .hint = NULL,
        .func = &cmd_set_speed,
        .argtable = &set_speed_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_speed_cmd));

    set_location_args.location = arg_dbl1("s", "set", "<location>", "Target location (rad)");
    set_location_args.end = arg_end(1);

    const esp_console_cmd_t set_location_cmd = {
        .command = "loc",
        .help = "Set motor location",
        .hint = NULL,
        .func = &cmd_set_location,
        .argtable = &set_location_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_location_cmd));

    set_current_args.current = arg_dbl1("s", "set", "<current>", "Target current (A)");
    set_current_args.end = arg_end(1);

    const esp_console_cmd_t set_current_cmd = {
        .command = "current",
        .help = "Set motor current",
        .hint = NULL,
        .func = &cmd_set_current,
        .argtable = &set_current_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_current_cmd));

    const esp_console_cmd_t zero_location_cmd = {
        .command = "zero",
        .help = "Set current location to zero",
        .hint = NULL,
        .func = &cmd_zero_location,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&zero_location_cmd));

    set_motor_id_args.motor_id = arg_int1(NULL, NULL, "<id>", "Motor ID");
    set_motor_id_args.end = arg_end(1);

    const esp_console_cmd_t set_motor_id_cmd = {
        .command = "setid",
        .help = "Set motor ID",
        .hint = NULL,
        .func = &cmd_set_motor_id,
        .argtable = &set_motor_id_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&set_motor_id_cmd));

    const esp_console_cmd_t get_motor_id_cmd = {
        .command = "getid",
        .help = "Get motor ID",
        .hint = NULL,
        .func = &cmd_get_motor_id,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&get_motor_id_cmd));

    const esp_console_cmd_t status_cmd = {
        .command = "status",
        .help = "Show motor status",
        .hint = NULL,
        .func = &cmd_status,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&status_cmd));
}
