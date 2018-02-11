/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "argtable3/argtable3.h"
#include "cmd_decl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/rtc_cntl_reg.h"
#include "sdkconfig.h"

#ifdef CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS
#define WITH_TASKS_INFO 1
#endif

static void register_free();
static void register_restart();
static void register_deep_sleep();
static void register_make();
#if WITH_TASKS_INFO
static void register_tasks();
#endif

void register_system()
{
    register_free();
    register_restart();
    register_deep_sleep();
    register_make();
#if WITH_TASKS_INFO
    register_tasks();
#endif
}

/** 'restart' command restarts the program */

static int restart(int argc, char** argv)
{
    ESP_LOGI(__func__, "Restarting");
    esp_restart();
}

static void register_restart()
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "Restart the program",
        .hint = NULL,
        .func = &restart,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** 'free' command prints available heap memory */

static int free_mem(int argc, char** argv)
{
    printf("%d\n", esp_get_free_heap_size());
    return 0;
}

static void register_free()
{
    const esp_console_cmd_t cmd = {
        .command = "free",
        .help = "Get the total size of heap memory available",
        .hint = NULL,
        .func = &free_mem,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** 'tasks' command prints the list of tasks and related information */
#if WITH_TASKS_INFO

static int tasks_info(int argc, char** argv)
{
    const size_t bytes_per_task = 40; /* see vTaskList description */
    char* task_list_buffer = malloc(uxTaskGetNumberOfTasks() * bytes_per_task);
    if (task_list_buffer == NULL) {
        ESP_LOGE(__func__, "failed to allocate buffer for vTaskList output");
        return 1;
    }
    vTaskList(task_list_buffer);
    fputs(task_list_buffer, stdout);
    free(task_list_buffer);
    return 0;
}

static void register_tasks()
{
    const esp_console_cmd_t cmd = {
        .command = "tasks",
        .help = "Get information about running tasks",
        .hint = NULL,
        .func = &tasks_info,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

#endif // WITH_TASKS_INFO

/** 'deep_sleep' command puts the chip into deep sleep mode */

static struct {
    struct arg_int *wakeup_time;
    struct arg_int *wakeup_gpio_num;
    struct arg_int *wakeup_gpio_level;
    struct arg_end *end;
} deep_sleep_args;


static int deep_sleep(int argc, char** argv)
{
    int nerrors = arg_parse(argc, argv, (void**) &deep_sleep_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, deep_sleep_args.end, argv[0]);
        return 1;
    }
    if (deep_sleep_args.wakeup_time->count) {
        uint64_t timeout = 1000ULL * deep_sleep_args.wakeup_time->ival[0];
        ESP_LOGI(__func__, "Enabling timer wakeup, timeout=%lluus", timeout);
        ESP_ERROR_CHECK( esp_sleep_enable_timer_wakeup(timeout) );
    }
    if (deep_sleep_args.wakeup_gpio_num->count) {
        int io_num = deep_sleep_args.wakeup_gpio_num->ival[0];
        if (!rtc_gpio_is_valid_gpio(io_num)) {
            ESP_LOGE(__func__, "GPIO %d is not an RTC IO", io_num);
            return 1;
        }
        int level = 0;
        if (deep_sleep_args.wakeup_gpio_level->count) {
            level = deep_sleep_args.wakeup_gpio_level->ival[0];
            if (level != 0 && level != 1) {
                ESP_LOGE(__func__, "Invalid wakeup level: %d", level);
                return 1;
            }
        }
        ESP_LOGI(__func__, "Enabling wakeup on GPIO%d, wakeup on %s level",
                io_num, level ? "HIGH" : "LOW");

        ESP_ERROR_CHECK( esp_sleep_enable_ext1_wakeup(1ULL << io_num, level) );
    }
    esp_deep_sleep_start();
}

static void register_deep_sleep()
{
    deep_sleep_args.wakeup_time =
            arg_int0("t", "time", "<t>", "Wake up time, ms");
    deep_sleep_args.wakeup_gpio_num =
            arg_int0(NULL, "io", "<n>",
                     "If specified, wakeup using GPIO with given number");
    deep_sleep_args.wakeup_gpio_level =
            arg_int0(NULL, "io_level", "<0|1>", "GPIO level to trigger wakeup");
    deep_sleep_args.end = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "deep_sleep",
        .help = "Enter deep sleep mode. "
                "Two wakeup modes are supported: timer and GPIO. "
                "If no wakeup option is specified, will sleep indefinitely.",
        .hint = NULL,
        .func = &deep_sleep,
        .argtable = &deep_sleep_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** This command helps maintain sanity when testing console example from a console */

static int make(int argc, char** argv)
{
    int count = REG_READ(RTC_CNTL_STORE0_REG);
    if (++count >= 3) {
        printf("This is not the console you are looking for.\n");
        return 0;
    }
    REG_WRITE(RTC_CNTL_STORE0_REG, count);

    const char* make_output =
R"(LD build/console.elf
esptool.py v2.1-beta1
)";

    const char* flash_output[] = {
R"(Flashing binaries to serial port )" CONFIG_ESPTOOLPY_PORT R"( (app at offset 0x10000)...
esptool.py v2.1-beta1
Connecting....
)",
R"(Chip is ESP32D0WDQ6 (revision 0)
Uploading stub...
Running stub...
Stub running...
Changing baud rate to 921600
Changed.
Configuring flash size...
Auto-detected Flash size: 4MB
Flash params set to 0x0220
Compressed 15712 bytes to 9345...
)",
R"(Wrote 15712 bytes (9345 compressed) at 0x00001000 in 0.1 seconds (effective 1126.9 kbit/s)...
Hash of data verified.
Compressed 333776 bytes to 197830...
)",
R"(Wrote 333776 bytes (197830 compressed) at 0x00010000 in 3.3 seconds (effective 810.3 kbit/s)...
Hash of data verified.
Compressed 3072 bytes to 82...
)",
R"(Wrote 3072 bytes (82 compressed) at 0x00008000 in 0.0 seconds (effective 1588.4 kbit/s)...
Hash of data verified.
Leaving...
Hard resetting...
)"
    };

    const char* monitor_output =
R"(MONITOR
)" LOG_COLOR_W R"(--- idf_monitor on )" CONFIG_ESPTOOLPY_PORT R"( 115200 ---
--- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H --
)" LOG_RESET_COLOR;

    bool need_make = false;
    bool need_flash = false;
    bool need_monitor = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "all") == 0) {
            need_make = true;
        } else if (strcmp(argv[i], "flash") == 0) {
            need_make = true;
            need_flash = true;
        } else if (strcmp(argv[i], "monitor") == 0) {
            need_monitor = true;
        } else if (argv[i][0] == '-') {
            /* probably -j option */
        } else if (isdigit((int) argv[i][0])) {
            /* might be an argument to -j */
        } else {
            printf("make: *** No rule to make target `%s'.  Stop.\n", argv[i]);
            /* Technically this is an error, but let's not spoil the output */
            return 0;
        }
    }
    if (argc == 1) {
        need_make = true;
    }
    if (need_make) {
        printf("%s", make_output);
    }
    if (need_flash) {
        size_t n_items = sizeof(flash_output) / sizeof(flash_output[0]);
        for (int i = 0; i < n_items; ++i) {
            printf("%s", flash_output[i]);
            vTaskDelay(200/portTICK_PERIOD_MS);
        }
    }
    if (need_monitor) {
        printf("%s", monitor_output);
        esp_restart();
    }
    return 0;
}

static void register_make()
{
    const esp_console_cmd_t cmd = {
        .command = "make",
        .help = NULL, /* hide from 'help' output */
        .hint = "all | flash | monitor",
        .func = &make,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}


