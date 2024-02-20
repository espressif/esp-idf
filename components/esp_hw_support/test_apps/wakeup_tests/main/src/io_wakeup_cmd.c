/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "test_utils.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"

static const char* TAG = "io_wakeup_test";

typedef enum {
    IO_WAKEUP_LEVEL_LOW = 0,
    IO_WAKEUP_LEVEL_HIGH,
    IO_WAKEUP_LEVEL_MAX,
} io_wakeup_level_t;

static struct {
    struct arg_int *mode;
    struct arg_end *end;
} sleep_args;

static int process_sleep(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &sleep_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, sleep_args.end, argv[0]);
        return 1;
    }

    if (sleep_args.mode->count == 1) {
        if (sleep_args.mode->ival[0] == 0) {
            esp_light_sleep_start();
            ESP_LOGI(TAG, "esp_light_sleep_start");
        } else if (sleep_args.mode->ival[0] == 1) {
            esp_deep_sleep_start();
        } else {
            ESP_LOGE(TAG, "no valid arguments");
            return 1;
        }
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }
    return 0;
}

static void register_sleep(void)
{
    sleep_args.mode =
        arg_int0("m", "mode", "<mode>", "configure the sleep mode, 0: light; 1: deep");
    sleep_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "sleep",
        .help = "select the sleep mode",
        .hint = NULL,
        .func = &process_sleep,
        .argtable = &sleep_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

#if SOC_PM_SUPPORT_EXT1_WAKEUP
static struct {
    struct arg_int *pin;
    struct arg_int *mode;
    struct arg_lit *disable;
    struct arg_end *end;
} ext1_wakeup_args;

static int process_ext1_wakeup(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &ext1_wakeup_args);
    int io_wakeup_num = 0, io_wakeup_level = 0;
    if (nerrors != 0) {
        arg_print_errors(stderr, ext1_wakeup_args.end, argv[0]);
        return 1;
    }

    if (ext1_wakeup_args.pin->count == 1) {
        io_wakeup_num = ext1_wakeup_args.pin->ival[0];
        ESP_LOGI(TAG, "io_wakeup_num = %d\n", io_wakeup_num);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }

    if (ext1_wakeup_args.mode->count) {
        if (ext1_wakeup_args.mode->count == 1) {
            io_wakeup_level = ext1_wakeup_args.mode->ival[0];
        } else {
            ESP_LOGE(TAG, "no valid arguments");
            return 1;
        }
    }
    ESP_LOGI(TAG, "io_wakeup_level = %d\n", io_wakeup_level);

    if (ext1_wakeup_args.disable->count) {
        ESP_ERROR_CHECK(esp_sleep_disable_ext1_wakeup_io(1ULL << io_wakeup_num));
    } else {
#if CONFIG_IDF_TARGET_ESP32
        ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io(1ULL << io_wakeup_num, io_wakeup_level == 0 ? ESP_EXT1_WAKEUP_ALL_LOW : ESP_EXT1_WAKEUP_ANY_HIGH));
#else
        ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io(1ULL << io_wakeup_num, io_wakeup_level == 0 ? ESP_EXT1_WAKEUP_ANY_LOW : ESP_EXT1_WAKEUP_ANY_HIGH));
#endif
    }
    return 0;
}

static void register_ext1_wakeup(void)
{
    ext1_wakeup_args.pin =
        arg_int0("p", "pin", "<pin>", "configure the ext1 wakeup pin num");
    ext1_wakeup_args.mode =
        arg_int0("m", "mode", "<mode>", "configure the ext1 wakeup mode");
    ext1_wakeup_args.disable =
        arg_lit0("d", "disable", "disable the ext1 wakeup on certain pin");
    ext1_wakeup_args.end = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "ext1",
        .help = "configue ext1 wakeup",
        .hint = NULL,
        .func = &process_ext1_wakeup,
        .argtable = &ext1_wakeup_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
#endif

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
static struct {
    struct arg_int *pin;
    struct arg_int *level;
    struct arg_lit *disable;
    struct arg_end *end;
} rtcio_wakeup_args;
static int process_rtcio_wakeup(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &rtcio_wakeup_args);
    int io_wakeup_num = 0, io_wakeup_level = 0;
    if (nerrors != 0) {
        arg_print_errors(stderr, rtcio_wakeup_args.end, argv[0]);
        return 1;
    }

    if (rtcio_wakeup_args.pin->count == 1) {
        io_wakeup_num = rtcio_wakeup_args.pin->ival[0];
        ESP_LOGI(TAG, "io_wakeup_num = %d\n", io_wakeup_num);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }

    if (rtcio_wakeup_args.level->count) {
        if (rtcio_wakeup_args.level->count == 1) {
            io_wakeup_level = rtcio_wakeup_args.level->ival[0];
        } else {
            ESP_LOGE(TAG, "no valid arguments");
            return 1;
        }
    }
    ESP_LOGI(TAG, "io_wakeup_level = %d\n", io_wakeup_level);

    if (rtcio_wakeup_args.disable->count) {
        ESP_ERROR_CHECK(gpio_deep_sleep_wakeup_disable(io_wakeup_num));
    } else {
        gpio_config_t config = {
            .pin_bit_mask = BIT64(io_wakeup_num),
            .mode = GPIO_MODE_INPUT,
            .pull_down_en = false,
            .pull_up_en = false,
            .intr_type = GPIO_INTR_DISABLE
        };
        ESP_ERROR_CHECK(gpio_config(&config));

        /* Enable wake up from GPIO */
        ESP_ERROR_CHECK(esp_deep_sleep_enable_gpio_wakeup(BIT64(io_wakeup_num), io_wakeup_level));
    }

    return 0;
}

static void register_rtcio_wakeup(void)
{
    rtcio_wakeup_args.pin =
        arg_int0("p", "pin", "<pin>", "configure the rtcio wakeup pin num");
    rtcio_wakeup_args.level =
        arg_int0("l", "level", "<level>", "configure the rtcio wakeup level");
    rtcio_wakeup_args.disable =
        arg_lit0("d", "disable", "disable the rtcio wakeup on certain pin");
    rtcio_wakeup_args.end = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "rtcio",
        .help = "configue rtcio wakeup",
        .hint = NULL,
        .func = &process_rtcio_wakeup,
        .argtable = &rtcio_wakeup_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
#endif

static struct {
    struct arg_int *pin;
    struct arg_int *level;
    struct arg_lit *disable;
    struct arg_end *end;
} gpio_wakeup_args;

static int process_gpio_wakeup(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &gpio_wakeup_args);
    int io_wakeup_num = 0, io_wakeup_level = 0;
    if (nerrors != 0) {
        arg_print_errors(stderr, gpio_wakeup_args.end, argv[0]);
        return 1;
    }

    if (gpio_wakeup_args.pin->count == 1) {
        io_wakeup_num = gpio_wakeup_args.pin->ival[0];
        ESP_LOGI(TAG, "io_wakeup_num = %d\n", io_wakeup_num);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }

    if (gpio_wakeup_args.level->count) {
        if (gpio_wakeup_args.level->count == 1) {
            io_wakeup_level = gpio_wakeup_args.level->ival[0];
        } else {
            ESP_LOGE(TAG, "no valid arguments");
            return 1;
        }
    }
    ESP_LOGI(TAG, "io_wakeup_level = %d\n", io_wakeup_level);

    if (gpio_wakeup_args.disable->count) {
        ESP_ERROR_CHECK(gpio_wakeup_disable(io_wakeup_num));
    } else {
        gpio_config_t config = {
            .pin_bit_mask = BIT64(io_wakeup_num),
            .mode = GPIO_MODE_INPUT,
            .pull_down_en = false,
            .pull_up_en = false,
            .intr_type = GPIO_INTR_DISABLE
        };
        ESP_ERROR_CHECK(gpio_config(&config));

        /* Enable wake up from GPIO */
        ESP_ERROR_CHECK(gpio_wakeup_enable(io_wakeup_num, io_wakeup_level == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL));
        ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup());
    }

    return 0;
}

static void register_gpio_wakeup(void)
{
    gpio_wakeup_args.pin =
        arg_int0("p", "pin", "<pin>", "configure the gpio wakeup pin num");
    gpio_wakeup_args.level =
        arg_int0("l", "level", "<level>", "configure the gpio wakeup level");
    gpio_wakeup_args.disable =
        arg_lit0("d", "disable", "disable the gpio wakeup on certain pin");
    gpio_wakeup_args.end = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "gpio",
        .help = "configue gpio wakeup",
        .hint = NULL,
        .func = &process_gpio_wakeup,
        .argtable = &gpio_wakeup_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *pin;
    struct arg_int *level;
    struct arg_end *end;
} gpio_control_args;

static int process_gpio_control(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &gpio_control_args);
    int io_num = 0, io_level = 0;
    if (nerrors != 0) {
        arg_print_errors(stderr, gpio_control_args.end, argv[0]);
        return 1;
    }

    if (gpio_control_args.pin->count == 1) {
        io_num = gpio_control_args.pin->ival[0];
        ESP_LOGI(TAG, "io_num = %d\n", io_num);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }

    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << io_num)
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    if (gpio_control_args.level->count == 1) {
        io_level = gpio_control_args.level->ival[0];
        ESP_LOGI(TAG, "io_level = %d\n", io_level);
    } else {
        ESP_LOGI(TAG, "io_num = %d\n", io_num);
        ESP_LOGE(TAG, "no valid io_level argument");
    }

    ESP_ERROR_CHECK(gpio_set_level(io_num, io_level == 0 ? 0 : 1));

    return 0;
}

static void register_gpio_control(void)
{
    gpio_control_args.pin =
        arg_int0("p", "pin", "<pin>", "configure the gpio control pin num");
    gpio_control_args.level =
        arg_int0("l", "level", "<level>", "configure the gpio control pin level");
    gpio_control_args.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "gpio_control",
        .help = "configue gpio control",
        .hint = NULL,
        .func = &process_gpio_control,
        .argtable = &gpio_control_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_end *end;
} wakeup_cause_args;

static int process_get_wakeup_cause(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &wakeup_cause_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, sleep_args.end, argv[0]);
        return 1;
    }

    switch (esp_sleep_get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_EXT1: {
        printf("Wake up from EXT1\n");
        break;
    }
    case ESP_SLEEP_WAKEUP_GPIO: {
        printf("Wake up from GPIO\n");
        break;
    }
    default: {
        printf("Wakeup cause err\n");
    }
    }
    return 0;
}

static void register_get_wakeup_cause(void)
{
    wakeup_cause_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "cause",
        .help = "get the wakeup cause",
        .hint = NULL,
        .func = &process_get_wakeup_cause,
        .argtable = &wakeup_cause_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void register_io_wakeup_cmd(void)
{
    register_sleep();
#if SOC_PM_SUPPORT_EXT1_WAKEUP
    register_ext1_wakeup();
#endif
    register_gpio_control();
    register_gpio_wakeup();
#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
    register_rtcio_wakeup();
#endif
    register_get_wakeup_cause();
}
